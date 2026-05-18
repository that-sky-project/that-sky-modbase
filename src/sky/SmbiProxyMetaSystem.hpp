#ifndef __SKY_SMBIPROXYMETASYSTEM_HPP__
#define __SKY_SMBIPROXYMETASYSTEM_HPP__

#include <queue>
#include "Meta.hpp"
#include "ModInternal.hpp"

#pragma warning(disable: 4200)

// ----------------------------------------------------------------------------
// [SECTION] Sky/ProxyMetaSystemDataContainer
// ----------------------------------------------------------------------------

class ProxyMetaSystemDataContainer: public MetaSystemDataContainer {
public:
  ProxyMetaSystemDataContainer() = default;
  ~ProxyMetaSystemDataContainer() = default;

  ProxyMetaSystemDataContainer &operator=(const MetaSystemDataContainer &);

  u32 m_count = 0;
  u32 m_maxClasses = 0;
};

// ----------------------------------------------------------------------------
// [SECTION] Sky/SmbiProxyMetaSystem
// ----------------------------------------------------------------------------

using PFN_GetMetaTypeByType = const MetaType *(*);
using PFN_GetMetaClassByType = const MetaClass *(*);

class SmbiProxyMetaSystem {
public:
  static SmbiProxyMetaSystem *Create(
    u32 maxClasses = 0x100000);

  SmbiProxyMetaSystem() = default;
  ~SmbiProxyMetaSystem() = default;

  void Initialize(
    const MetaSystem *pMetaSystem,
    u32 count);

  bool AddClass(
    MetaClass *clazz);

  bool AddType(
    MetaType *type);

  const MetaClass *GetMetaClassById(
    i32 id
  ) const;

  const MetaClass *GetMetaClassByName(
    cstring name,
    bool isConstStr
  ) const;

  const MetaType *GetMetaType(
    cstring name
  ) const;

  PFN_GetMetaClassByType GetMetaClassRegister(
    cstring name
  ) const;

  PFN_GetMetaTypeByType GetMetaTypeRegister(
    cstring name
  ) const;

  u32 m_metaClassId = -1;
  ProxyMetaSystemDataContainer *m_data = nullptr;
  const MetaClass *m_classes[];
};

// ----------------------------------------------------------------------------
// [SECTION] Sky/SmbiMetaRequestBarn
// ----------------------------------------------------------------------------

class SmbiMetaRequestBarn: public SmbiMutexObject {
public:
  SmbiMetaRequestBarn() = default;
  ~SmbiMetaRequestBarn() = default;

  void Initialize(
    SmbiProxyMetaSystem *proxyMetaSystem);

  void Ready();

  void SubmitChain(
    MetaType *metaChain);

  void SubmitChain(
    MetaMemberFunction *metaChain);

  void SubmitChain(
    MetaMemberVariable *metaChain);

  void SubmitChain(
    MetaFunction *metaChain);

  void SubmitChain(
    MetaVariable *metaChain);

private:
  SmbiProxyMetaSystem *m_proxyMetaSystem = nullptr;
  std::queue<MetaType *> m_metaTypes;
  std::queue<MetaMemberFunction *> m_memberFunctions;
  std::queue<MetaMemberVariable *> m_memberVariables;
  std::queue<MetaFunction *> m_functions;
  std::queue<MetaVariable *> m_variables;
};

#endif
