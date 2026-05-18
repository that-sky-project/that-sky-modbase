#include "ModInternal.hpp"
#include "SmbiProxyMetaSystem.hpp"

#pragma warning(disable: 6386)

// ----------------------------------------------------------------------------
// [SECTION] Sky/ProxyMetaSystemDataContainer
// ----------------------------------------------------------------------------

ProxyMetaSystemDataContainer &ProxyMetaSystemDataContainer::operator=(
  const MetaSystemDataContainer &rhs
) {
  m_metaTypes = rhs.m_metaTypes;
  m_metaConstants = rhs.m_metaConstants;
  m_metaVariables = rhs.m_metaVariables;
  m_metaFunctions = rhs.m_metaFunctions;
  m_metaClasses = rhs.m_metaClasses;
  unk_6 = rhs.unk_6;
  unk_7 = rhs.unk_7;
  unk_8 = rhs.unk_8;
  m_count = 0;

  return *this;
}

// ----------------------------------------------------------------------------
// [SECTION] Sky/SmbiProxyMetaSystem
// ----------------------------------------------------------------------------

SmbiProxyMetaSystem *SmbiProxyMetaSystem::Create(
  u32 maxClasses
) {
  ProxyMetaSystemDataContainer *pData = new ProxyMetaSystemDataContainer();
  pData->m_maxClasses = maxClasses;
  pData->m_count = 0;

  SmbiProxyMetaSystem *pMetaSystem = (SmbiProxyMetaSystem *)operator new(
    sizeof(SmbiProxyMetaSystem) + sizeof(const MetaClass *) * maxClasses);
  new (pMetaSystem) SmbiProxyMetaSystem();
  pMetaSystem->m_data = pData;

  //for (u32 i = 0; i < maxClasses; i++)
  //  pMetaSystem->m_classes[i] = GetMetaClass();
  memset(pMetaSystem->m_classes, 0, sizeof(const MetaClass *) * maxClasses);

  return pMetaSystem;
}

void SmbiProxyMetaSystem::Initialize(
  const MetaSystem *p,
  u32 count
) {
  HTAssertMsg(
    m_data,
    "not initialized");
  HTAssertMsg(
    count <= m_data->m_maxClasses,
    "m_maxClasses is too small (%d <= %d), try to reserve more space",
    m_data->m_maxClasses,
    count);

  m_metaClassId = p->m_metaClassId;
  *m_data = *p->m_data;

  // Actually we don't know how many classes we need to copy at compile time, 
  // so we resolve sizeof(MetaSystem) to get number of classes.
  memcpy(m_classes, (const void *)p->m_classes, count * sizeof(const MetaClass *));

  m_data->m_count = count;
}

bool SmbiProxyMetaSystem::AddClass(
  MetaClass *clazz
) {
  if (!m_data)
    return false;

  if (m_data->m_metaTypes.find(clazz->m_name) != m_data->m_metaTypes.end())
    return false;

  if (!clazz->AsClass())
    return false;

  // Copy the class and class name.
  char *name = new char[strlen(clazz->m_name) + 1];
  strcpy(name, clazz->m_name);

  auto *mc = (MetaClass *)clazz->Copy();
  mc->m_name = name;
  mc->m_self = clazz->m_self = mc;

  // Reset global id.
  mc->m_globalId = m_data->m_count++;
  // Allocate MetaDataContainer.
  if (!mc->m_metaDataContainer)
    mc->m_metaDataContainer = new MetaDataContainer();

  // Push into the class array.
  m_classes[mc->m_globalId] = mc;

  // Push into maps.
  m_data->m_metaTypes[clazz->m_name] = mc;
  m_data->m_metaClasses[clazz->m_name] = mc;

  return true;
}

bool SmbiProxyMetaSystem::AddType(
  MetaType *type
) {
  if (!m_data)
    return false;

  if (m_data->m_metaTypes.find(type->m_name) != m_data->m_metaTypes.end())
    return false;

  // Copy the type name.
  char *name = new char[strlen(type->m_name) + 1];
  strcpy(name, type->m_name);

  // Copy the type descriptor.
  auto *mt = (MetaType *)type->Copy();
  mt->m_name = name;
  mt->m_self = type->m_self = mt;

  // Push into maps.
  m_data->m_metaTypes[mt->m_name] = mt;

  return true;
}

const MetaClass *SmbiProxyMetaSystem::GetMetaClassById(
  i32 id
) const {
  if (!m_data)
    return nullptr;

  if (id >= m_data->m_count)
    return nullptr;

  return m_classes[id];
}

const MetaClass *SmbiProxyMetaSystem::GetMetaClassByName(
  cstring name,
  bool isConstStr
) const {
  (void)isConstStr;

  if (!m_data)
    return nullptr;

  const auto &it = m_data->m_metaClasses.find(name);
  if (it == m_data->m_metaClasses.end())
    return nullptr;

  return it->second;
}

const MetaType *SmbiProxyMetaSystem::GetMetaType(
  cstring name
) const {
  if (!m_data)
    return nullptr;

  const auto &it = m_data->m_metaTypes.find(name);
  if (it == m_data->m_metaTypes.end())
    return nullptr;

  return it->second;
}
