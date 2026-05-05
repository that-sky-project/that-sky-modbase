#ifndef __MOD_SMBIBINARYDESCRIPTOR_HPP__
#define __MOD_SMBIBINARYDESCRIPTOR_HPP__

#include <Windows.h>
#include <unordered_map>
#include <includes/htmodloader.h>
#include <cJSON.h>
#include <Meta.hpp>
#include "utils/SmbiObject.hpp"
#include "utils/TypeAliases.hpp"
#include "mod/MetaBinary.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Modbase/SmbiBinaryDescriptor
// ----------------------------------------------------------------------------

class SmbiBinaryDescriptor {
public:
  enum DataType {
    kDataType_None = 0,
    kDataType_Function,
    kDataType_Variable,
    kDataType_VirtualFunction,
    kDataType_MemberVariable,
  };

  enum InitType {
    kInitType_None = 0,
    kInitType_Pattern,
    kInitType_Offset,
    kInitType_GetProcAddr,
    kInitType_SlotIndex,
  };

  SmbiBinaryDescriptor() = default;
  ~SmbiBinaryDescriptor() = default;

  // Get the type of MetaBinary object related to the descriptor.
  inline DataType GetDataType() const { return m_dataType; }
  // Get the initialization method of the descriptor.
  inline InitType GetInitType() const { return m_initType; }
  // Get resolved address or object.
  inline uintptr_t GetResolved() const { return m_resolved; }
  // Get the name of the descriptor.
  inline cstring GetName() const { return m_name.c_str(); }
  // Check whether the scanning is done.
  inline bool IsValid() const { return m_done; }

  // Set the basic data from json object.
  bool Initialize(const cJSON *json);

  // Do the scanning.
  void Resolve();

  // Print the detail of the descriptor.
  void DebugPrint();

private:
  // Descriptor name, must be the same as the name of MetaBinary objects.
  TgcString m_name = "";
  // Type of MetaBinary object related to the descriptor.
  DataType m_dataType = kDataType_None;
  // Initialization method of the descriptor.
  InitType m_initType = kInitType_None;
  union {
    // Offset inside the module.
    u32 m_offset = 0;
    // Virtual function table slot index.
    u32 m_slotIdx;
    // Signature pattern.
    HTAsmSig m_signature;
  };
  // Module name.
  TgcWString m_module = L"";
  // Parameter string, can be the pattern of kInitType_Pattern or the symbol name
  // of kInitType_GetProcAddr.
  TgcString m_param = "";
  // Set to true when the initialization is done.
  bool m_done = false;
  // Resolved address or object.
  uintptr_t m_resolved = 0;
};

// ----------------------------------------------------------------------------
// [SECTION] Modbase/SmbiBinaryDescriptorBarn
// ----------------------------------------------------------------------------

class SmbiBinaryDescriptorBarn: public SmbiObject {
public:
  SmbiBinaryDescriptorBarn() = default;
  ~SmbiBinaryDescriptorBarn() = default;

  void Initialize(const cJSON *json);

  void ResolveAll();

  void Apply(MetaBinaryFunction *metaObject) const;
  void Apply(MetaBinaryOffset *metaObject) const;
  void Apply(MetaBinaryVirtualFunction *metaObject) const;
  void Apply(MetaBinaryMemberOffset *metaObject) const;

private:
  const SmbiBinaryDescriptor *m_Lookup(cstring name) const;

  std::unordered_map<TgcString, SmbiBinaryDescriptor> m_descriptors = {};
};

#endif
