#include <includes/htmodloader.h>
#include <cJSON.h>
#include <string.h>
#include "utils/PathUtils.hpp"
#include "utils/StringUtils.hpp"
#include "mod/SmbiBinaryDescriptor.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Modbase/SmbiBinaryDescriptor/helpers
// ----------------------------------------------------------------------------

static cstring jsonGetString(
  const cJSON *json,
  cstring key
) {
  const cJSON *kv;

  kv = cJSON_GetObjectItemCaseSensitive(json, key);
  if (!cJSON_IsString(kv))
    return nullptr;

  return cJSON_GetStringValue(kv);
}

static bool jsonGetUint32(
  u32 &result,
  const cJSON *json,
  cstring key
) {
  const cJSON *kv;
  cstring s;
  u32 value;

  kv = cJSON_GetObjectItemCaseSensitive(json, key);
  if (cJSON_IsString(kv)) {
    s = cJSON_GetStringValue(kv);
    if (!s)
      return false;

    char *p;
    value = (u32)strtoll(s, &p, 0);
    if (p == s)
      return false;
  } else if (cJSON_IsNumber(kv)) {
    value = (u32)cJSON_GetNumberValue(kv);
  } else {
    return false;
  }

  result = value;

  return true;
}

class SmbiInitThreadPool: public SmbiMutexObject {
public:
  SmbiInitThreadPool() = default;
  ~SmbiInitThreadPool() = default;

private:
  struct Thread {
    std::thread m_handle;
    int m_load;
  };

  static u32 m_Concurrency() {
    u32 hw = std::thread::hardware_concurrency();
    if (hw == 0)
      hw = 2;
    return (hw > 2) ? hw - 1 : 2;
  }
};

// ----------------------------------------------------------------------------
// [SECTION] Modbase/SmbiBinaryDescriptor
// ----------------------------------------------------------------------------

bool SmbiBinaryDescriptor::Initialize(
  const cJSON *json
) {
  cstring s;

  // Read descriptor name.
  s = jsonGetString(json, "name");
  if (!s)
    return false;

  m_name = s;

  // Read module name.
  s = jsonGetString(json, "module");
  if (s) {
    m_module = utf8towcs(s);
  } else {
    wchar_t fullModuleName[PathUtils::kMaxPathLen];
    GetModuleFileNameW(nullptr, fullModuleName, PathUtils::kMaxPathLen);
    m_module = PathUtils::FileName(fullModuleName);
  }

  // Read data type.
  s = jsonGetString(json, "type");
  if (!s)
    return false;

  if (!strcmp(s, "function"))
    m_dataType = kDataType_Function;
  else if (!strcmp(s, "variable"))
    m_dataType = kDataType_Variable;
  else if (!strcmp(s, "virtual_function")) {
    m_dataType = kDataType_VirtualFunction;
    m_initType = kInitType_SlotIndex;
  } else if (!strcmp(s, "member_variable")) {
    m_dataType = kDataType_MemberVariable;
    m_initType = kInitType_Offset;
  } else
    return false;

  // Read initializer type.
  if (!m_initType) {
    s = jsonGetString(json, "method");
    if (!s)
      return false;

    if (!strcmp(s, "offset"))
      m_initType = kInitType_Offset;
    else if (!strcmp(s, "symbol"))
      m_initType = kInitType_GetProcAddr;
    else if (!strcmp(s, "direct")) {
      m_initType = kInitType_Pattern;
      m_signature.indirect = HT_SCAN_DIRECT;
    } else if (!strcmp(s, "e8")) {
      m_initType = kInitType_Pattern;
      m_signature.indirect = HT_SCAN_E8;
    } else if (!strcmp(s, "ff15")) {
      m_initType = kInitType_Pattern;
      m_signature.indirect = HT_SCAN_FF15;
    } else
      return false;
  }

  // Read initializer params.
  switch (m_initType) {
    case kInitType_SlotIndex: {
      // Read vftable slot index.
      if (!jsonGetUint32(m_slotIdx, json, "slot_index"))
        return false;
      break;
    }
    case kInitType_Offset: {
      // Read executable hard-coded offset.
      if (!jsonGetUint32(m_offset, json, "offset"))
        return false;
      break;
    }
    case kInitType_GetProcAddr: {
      // Read symbol name.
      s = jsonGetString(json, "symbol");
      if (!s)
        return false;
      m_param = s;
      break;
    }
    case kInitType_Pattern: {
      // Read pattern.
      s = jsonGetString(json, "pattern");
      if (!s)
        return false;

      m_param = s;
      m_signature.sig = m_param.c_str();

      if (!jsonGetUint32(m_signature.offset, json, "offset"))
        return false;
      break;
    }
    default: {
      return false;
    }
  }

  return true;
}

void SmbiBinaryDescriptor::Resolve() {
  switch (m_initType) {
    case kInitType_Pattern: {
      // Reset sigcode string to avoid wild pointer unref.
      m_signature.sig = m_param.c_str();
      m_resolved = (uintptr_t)HTSigScanEx(m_module.c_str(), &m_signature);
      if (!m_resolved)
        break;
      m_done = true;
      break;
    }
    case kInitType_Offset: {
      m_resolved = (uintptr_t)GetModuleHandleW(m_module.c_str()) + m_offset;
      if (!m_resolved)
        break;
      m_done = true;
      break;
    }
    case kInitType_GetProcAddr: {
      m_resolved = (uintptr_t)GetProcAddress(GetModuleHandleW(m_module.c_str()), m_param.c_str());
      if (!m_resolved)
        break;
      m_done = true;
      break;
    }
    case kInitType_SlotIndex: {
      m_resolved = m_slotIdx;
      m_done = true;
      break;
    }
    default: {
      m_done = false;
      break;
    }
  }
}

void SmbiBinaryDescriptor::DebugPrint() {
  smbiLogI("--------- SmbiBinaryDescriptor ---------");
  smbiLogI("  Name: %s", m_name.c_str());
  smbiLogI("  Type: %d", m_dataType);
  smbiLogI("  Method: %d", m_initType);
  if (m_initType == kInitType_GetProcAddr)
    smbiLogI("  SymbolName: %s", m_param.c_str());
  else if (m_initType == kInitType_Pattern) {
    smbiLogI("  Pattern: %s", m_signature.sig);
    smbiLogI("  Offset: %p", m_signature.offset);
  }
  smbiLogI("  Module: %ls", m_module.c_str());
  smbiLogI("  Resolved: 0x%p", m_resolved);
}

// ----------------------------------------------------------------------------
// [SECTION] Modbase/SmbiBinaryDescriptorBarn
// ----------------------------------------------------------------------------

void SmbiBinaryDescriptorBarn::Initialize(
  const cJSON *json
) {
  if (!json)
    return;

  const cJSON *it;
  cJSON_ArrayForEach(it, json) {
    SmbiBinaryDescriptor desc;
    if (!desc.Initialize(it))
      continue;
    m_descriptors.emplace(desc.GetName(), desc);
  }
}

void SmbiBinaryDescriptorBarn::ResolveAll() {
  for (auto &it: m_descriptors) {
    it.second.Resolve();
    //it.second.DebugPrint();
  }
}

void SmbiBinaryDescriptorBarn::Apply(
  MetaBinaryFunction *metaObject
) const {
  const auto *desc = m_Lookup(metaObject->m_name);
  if (!desc || !desc->IsValid())
    return;

  if (desc->GetDataType() != SmbiBinaryDescriptor::kDataType_Function)
    return;

  metaObject->Initialize((void *)desc->GetResolved());
}

void SmbiBinaryDescriptorBarn::Apply(
  MetaBinaryOffset *metaObject
) const {
  const auto *desc = m_Lookup(metaObject->m_name);
  if (!desc || !desc->IsValid())
    return;

  if (desc->GetDataType() != SmbiBinaryDescriptor::kDataType_Variable)
    return;

  metaObject->Initialize((void *)desc->GetResolved());
}

void SmbiBinaryDescriptorBarn::Apply(
  MetaBinaryVirtualFunction *metaObject
) const {
  const auto *desc = m_Lookup(metaObject->m_name);
  if (!desc || !desc->IsValid())
    return;

  if (desc->GetDataType() != SmbiBinaryDescriptor::kDataType_VirtualFunction)
    return;

  metaObject->Initialize(desc->GetResolved());
}

void SmbiBinaryDescriptorBarn::Apply(
  MetaBinaryMemberOffset *metaObject
) const {
  const auto *desc = m_Lookup(metaObject->m_name);
  if (!desc || !desc->IsValid())
    return;

  if (desc->GetDataType() != SmbiBinaryDescriptor::kDataType_MemberVariable)
    return;

  metaObject->Initialize(desc->GetResolved());
}

const SmbiBinaryDescriptor *SmbiBinaryDescriptorBarn::m_Lookup(
  cstring name
) const {
  const auto &it = m_descriptors.find(name);
  if (it == m_descriptors.end())
    return nullptr;
  return &it->second;
}
