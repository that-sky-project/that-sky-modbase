#include "ModInternal.hpp"
#include "mod/SmbiModInitializer.hpp"
#include "mod/MetaBinary.hpp"
#include "sky/SmbiProxyMetaSystem.hpp"

#pragma warning(disable: 4267)

// ----------------------------------------------------------------------------
// [SECTION] Api/MetaSystem/declarations
// ----------------------------------------------------------------------------

typedef void *Game;

// - Structs.

struct FakeMetaSystem: public Object {
  MetaSystemDataContainer *m_data;
};

class MetaTypeGeneral: public MetaType {
public:
  using PFN_ConstructType = void (*)(void *);
  using PFN_DestructType = void (*)(void *);

  MetaTypeGeneral(
    cstring name,
    const MetaTypeInitTypeDef *init
  )
    : MetaType(name, MetaType::noChainList)
    , m_typeSize(init->size)
    , m_typeAlign(init->align)
    , m_constructor(init->construct)
    , m_destructor(init->destruct)
  { }

  virtual size_t SizeOfType() const override {
    return m_typeSize;
  }

  virtual size_t AlignOfType() const override {
    return m_typeAlign;
  }

  virtual void *CreateByType() const override { }

  virtual void DeleteByType(void *p) const override { }

  virtual void *ConstructByType(void *p) const override { }

  virtual void DestructByType(void *p) const override { }

  virtual void DynamicCast(
    void *targetObject,
    void *sourceObject,
    const MetaType *sourceType
  ) const override { }

  virtual bool IsNumber() const override { }

  virtual bool IsString() const override { }

  virtual lua_Number ToNumber(
    void *object
  ) const override { }

  virtual const char *ToString(
    void *object
  ) const override { }

  virtual const MetaClass *AsClass() const override { }

  virtual void WriteType(
    lua_State *L,
    void *object
  ) const override { }

  virtual void ReadType(
    lua_State *L,
    int index,
    void *object
  ) const override { }

  virtual MetaType *Copy() const override { }

  size_t m_typeSize;
  size_t m_typeAlign;
  PFN_ConstructType m_constructor;
  PFN_DestructType m_destructor;
};

class MetaClassGeneral: public MetaClass {
public:
  using PFN_ConstructObject = void (*)(void *);
  using PFN_DestructObject = void (*)(void *);

  MetaClassGeneral(
    cstring name,
    PFN_RegisterClass parent,
    const MetaClassInitTypeDef *init
  )
    : MetaClass(name, MetaType::noChainList)
    , m_typeSize(init->size)
    , m_typeAlign(init->align)
    , m_constructor(init->construct)
    , m_destructor(init->destruct)
  { }

  size_t m_typeSize;
  size_t m_typeAlign;
  PFN_ConstructObject m_constructor;
  PFN_DestructObject m_destructor;
};

// - Functions.

static HTStatus fnInit_MetaSystem(
  HMODULE,
  const SmbiModInitializer *);

static void hook_MetaSystem_Initialize(
  FakeMetaSystem *self);
static void hook_Game_Alloc(
  Game *self);
static const MetaClass *hook_GetMetaClassById(
  u32 id);
static const MetaClass *hook_GetMetaClassByName(
  cstring name,
  bool isConstString);

// ----------------------------------------------------------------------------
// [SECTION] Api/MetaSystem/init
// ----------------------------------------------------------------------------

static SmbiModInitializer gInit_MetaSystem = {
  fnInit_MetaSystem,
  "MetaSystem"
};

static MetaBinaryFunction bin_MetaSystem_Initialize = {
  "MetaSystem::Initialize()",
  hook_MetaSystem_Initialize
};

static MetaBinaryFunction bin_Game_Alloc = {
  "Game::Alloc()",
  hook_Game_Alloc
};

static MetaBinaryFunction bin_GetMetaClassById = {
  "GetMetaClassById()",
  hook_GetMetaClassById
};

static MetaBinaryFunction bin_GetMetaClassByName = {
  "GetMetaClassByName()",
  hook_GetMetaClassByName
};

static HTStatus fnInit_MetaSystem(
  HMODULE hModuleDll,
  const SmbiModInitializer *self
) {
  (void)hModuleDll;
  (void)self;

  if (
    bin_MetaSystem_Initialize.Hook()
    && bin_Game_Alloc.Hook()
    && bin_GetMetaClassById.Hook()
    && bin_GetMetaClassByName.Hook()
  )
    return HT_SUCCESS;

  return HT_FAIL;
}

// ----------------------------------------------------------------------------
// [SECTION] Api/MetaSystem/variables
// ----------------------------------------------------------------------------

static FakeMetaSystem *gMetaSystem = nullptr;
static SmbiProxyMetaSystem *gProxyMetaSystem = nullptr;

// ----------------------------------------------------------------------------
// [SECTION] Api/MetaSystem/functions
// ----------------------------------------------------------------------------

// Save MetaSystem pointer.
// NOTE: The pointer also can be resolved with Game::metaSystem.
static void hook_MetaSystem_Initialize(
  FakeMetaSystem *self
) {
  bin_MetaSystem_Initialize.Call<void>(self);
  gMetaSystem = self;
}

// Replace Game::metaSystem with gProxyMetaSystem.
static void hook_Game_Alloc(
  Game *self
) {
  if (!gMetaSystem)
    // Not initialized, exit.
    return bin_Game_Alloc.Call<void>(self);

  const auto &classes = gMetaSystem->m_data->m_metaClasses;

  // Resolve sizeof(MetaSystem) and maximum metaclass count.
  const auto &itMetaSystem = classes.find("MetaSystem");
  if (itMetaSystem == classes.end())
    return bin_Game_Alloc.Call<void>(self);

  const MetaClass *pmcMetaSystem = itMetaSystem->second;
  size_t sizeMetaSystem = pmcMetaSystem->SizeOfObject();
  u32 classCount = (sizeMetaSystem - sizeof(FakeMetaSystem)) / sizeof(const MetaClass *);

  // Resolve Game.metaSystem.
  const auto &itGame = classes.find("Game");
  if (itGame == classes.end())
    // Not found, exit.
    return bin_Game_Alloc.Call<void>(self);

  const MetaClass *pmcGame = itGame->second;
  const auto &variables = pmcGame->m_metaDataContainer->m_variables;
  const auto &itVars = variables.find("metaSystem");
  if (itVars == variables.end())
    // Not found, exit.
    return bin_Game_Alloc.Call<void>(self);

  // Override Game.metaSystem.
  const MetaMemberVariable *pmmvMetaSystem = itVars->second;
  SmbiProxyMetaSystem **ppGameMetaSystem = (SmbiProxyMetaSystem **)(
    (char *)self + pmmvMetaSystem->m_offsetOf);

  // Create ProxyMetaSystem from MetaSystem.
  gProxyMetaSystem = SmbiProxyMetaSystem::Create();
  if (!gProxyMetaSystem)
    // Create gProxyMetaSystem failed, exit.
    return bin_Game_Alloc.Call<void>(self);

  gProxyMetaSystem->Initialize(
    reinterpret_cast<const MetaSystem *>(gMetaSystem),
    classCount);

  const MetaSystem *old = reinterpret_cast<const MetaSystem *>(*ppGameMetaSystem);

  // Call destructor of MetaStrMap.
  delete old->m_data;
  // Directly free the memory.
  operator delete((void *)old);

  // Override metasystem pointer in Game.
  *ppGameMetaSystem = gProxyMetaSystem;

  // Continue the execution flow.
  bin_Game_Alloc.Call<void>(self);
}

static const MetaClass *hook_GetMetaClassById(
  u32 id
) {
  if (!gProxyMetaSystem)
    return bin_GetMetaClassById.Call<const MetaClass *>(id);

  return gProxyMetaSystem->GetMetaClassById(id);
}

static const MetaClass *hook_GetMetaClassByName(
  cstring name,
  bool isConstString
) {
  if (!gProxyMetaSystem)
    return bin_GetMetaClassById.Call<const MetaClass *>(name, isConstString);

  return gProxyMetaSystem->GetMetaClassByName(name, isConstString);
}

// ----------------------------------------------------------------------------
// [SECTION] Api/MetaSystem/exports/MetaHandle
// ----------------------------------------------------------------------------

SMB_API_ATTR PMetaType SMB_API Sky_MetaType_Create(
  LPCSTR name,
  const MetaTypeInitTypeDef *init
) {
  return nullptr;//rcast(PMetaType)(new MetaTypeGeneral(name, init));
}

SMB_API_ATTR PMetaClass SMB_API Sky_MetaClass_Create(
  LPCSTR name,
  PFN_RegisterMetaClass parent
) {
  return nullptr;
}

SMB_API_ATTR PMetaType SMB_API Sky_MetaType_Destroy(
  PMetaType hMetaType
) {
  return nullptr;
}

SMB_API_ATTR PMetaType SMB_API Sky_MetaClass_Destroy(
  PMetaClass hMetaClass
) {
  return nullptr;
}

SMB_API_ATTR LPCSTR SMB_API Sky_MetaType_GetName(
  PCMetaType hMetaType
) {
  return nullptr;//rcast(const MetaType *)(hMetaType)->m_name;
}

SMB_API_ATTR PCMetaType SMB_API Sky_MetaType_GetActive(
  PCMetaType hMetaType
) {
  //const MetaType *p = rcast(const MetaType *)(hMetaType)->m_self;
  //return rcast(PCMetaType)(p);
  return nullptr;
}

SMB_API_ATTR HTStatus SMB_API Sky_MetaSystem_SubmitChain(
  HMODULE hModuleOwner,
  PMetaType hMetaType
) {
  /*MetaType *p = rcast(MetaType *)(hMetaType);
  for (; p; p = p->m_prev) {
    if (p->AsClass())
      gProxyMetaSystem->AddClass((MetaClass *)p);
    else
      gProxyMetaSystem->AddType(p);
  }*/
  return HT_FAIL;
}
