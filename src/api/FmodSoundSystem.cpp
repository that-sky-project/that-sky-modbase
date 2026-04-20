#include <fmod_studio.hpp>
#include "ModInternal.hpp"
#include "mod/SmbiModInitializer.hpp"
#include "sky/SmbiFmodSoundSystem.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Sky/SmbiFmodPathBarn/declarations
// ----------------------------------------------------------------------------

typedef void **FmodSoundSystem;
typedef void *FmodSoundResource;

typedef FMOD_RESULT (F_API *PFN_EventDescription_getPath)(
  FMOD::Studio::EventDescription *, char *, int, int *);
typedef i08 (__fastcall *PFN_FmodSoundSystem_LoadSoundBanks)(
  FmodSoundSystem *, const cstring *, u32, const char *, i08);
typedef FmodSoundResource *(__fastcall *PFN_FmodSoundSystem_GetSoundResource)(
  FmodSoundSystem *, const char *);
typedef FmodSoundSystem *(__fastcall *PFN_CreateFmodSoundSystem)();

static HTStatus fnInit_FmodSoundSystem(
  HMODULE,
  const SmbiModInitializer *);

static FMOD_RESULT F_API hook_EventDescription_getPath(
  FMOD::Studio::EventDescription *,
  char *,
  int,
  int *);

// ----------------------------------------------------------------------------
// [SECTION] Sky/SmbiFmodPathBarn/variables
// ----------------------------------------------------------------------------

static SmbiFmodPathBarn gFmodPathBarn;
static FmodSoundSystem *gSoundSystem = nullptr;

// ----------------------------------------------------------------------------
// [SECTION] Sky/SmbiFmodPathBarn/init
// ----------------------------------------------------------------------------

static const HTAsmSig sigE8_CreateFmodSoundSystem = {
  "90 ?  ?  ?  ?  48 8B 4E 20 E8 ?  ?  ?  ?  E8 ?  "
  "?  ?  ?  48 89",
  HT_SCAN_E8,
  0x0E
};

static HTAsmFunction sfn_EventDescription_GetPath{
  "FMOD::Studio::EventDescription::getPath()",
  nullptr,
  nullptr,
  nullptr
};

static HTAsmFunction sfn_CreateFmodSoundSystem = {
  "CreateFmodSoundSystem()",
  nullptr,
  nullptr,
  nullptr
};

static HTAsmFunction sfn_FmodSoundSystem_LoadSoundBanks = {
  "FmodStudioSoundSystem::LoadSoundBanks()",
  nullptr,
  nullptr,
  nullptr
};

static HTAsmFunction sfn_FmodSoundSystem_GetSoundResource = {
  "FmodStudioSoundSystem::GetSoundResource()",
  nullptr,
  nullptr,
  nullptr
};

static SmbiModInitializer gInit_FmodSoundSystem{
  fnInit_FmodSoundSystem,
  "FmodStudioSoundSystem::*"
};

// ----------------------------------------------------------------------------
// [SECTION] Sky/SmbiFmodPathBarn/functions
// ----------------------------------------------------------------------------

static HTStatus fnInit_FmodSoundSystem(
  HMODULE hModuleDll,
  const SmbiModInitializer *self
) {
  (void)hModuleDll;
  (void)self;

  gFmodPathBarn.Initialize();

  HMODULE hDllFmodStudio = GetModuleHandleA("fmodstudio.dll");
  sfn_EventDescription_GetPath.fn = (void *)GetProcAddress(
    hDllFmodStudio,
    "?getPath@EventDescription@Studio@FMOD@@QEBA?AW4FMOD_RESULT@@PEADHPEAH@Z");
  sfn_EventDescription_GetPath.detour = (void *)hook_EventDescription_getPath;

  return smbiCreateAndEnableHook(
    nullptr,
    &sfn_EventDescription_GetPath);
}

// Note that this function will affect every event.
static FMOD_RESULT F_API hook_EventDescription_getPath(
  FMOD::Studio::EventDescription *eventdescription,
  char *path,
  int size,
  int *retrieved
) {
  // Get path from Master.strings.bank at first.
  FMOD_RESULT first = ((PFN_EventDescription_getPath)sfn_EventDescription_GetPath.origin)(
    eventdescription, path, size, retrieved);
  if (first != FMOD_ERR_EVENT_NOTFOUND)
    return first;

  // Try to get event id.
  FmodGuid guid;
  FMOD_RESULT second = eventdescription->getID(&guid);
  if (second != FMOD_OK)
    return FMOD_ERR_EVENT_NOTFOUND;

  // Get registered event.
  std::string result;
  bool registered = gFmodPathBarn.FindPathByGuid(result, guid);
  if (!registered)
    return FMOD_ERR_EVENT_NOTFOUND;

  // Return result.
  int num = (int)result.length();
  strncpy(path, result.c_str(), size);
  path[size - 1] = 0;
  *retrieved = num > size ? size : num;

  return FMOD_OK;
}

FmodSoundResource *hook_FmodSoundSystem_GetSoundResource(
  FmodSoundSystem *pThis,
  const char *name
) {
  
  FmodSoundResource *result = ((PFN_FmodSoundSystem_GetSoundResource)sfn_FmodSoundSystem_GetSoundResource.origin)(
    pThis,
    name);
  return result;
}

FmodSoundSystem *hook_CreateFmodSoundSystem() {
  FmodSoundSystem *result = ((PFN_CreateFmodSoundSystem)sfn_CreateFmodSoundSystem.origin)();

  if (!gSoundSystem) {
    gSoundSystem = result;
    //HTTellText("FmodSoundSystem: 0x%p\n", gSoundSystem);
  }

  if (!sfn_FmodSoundSystem_LoadSoundBanks.fn) {
    // Virtual function #12.
    sfn_FmodSoundSystem_LoadSoundBanks.fn = (PFN_FmodSoundSystem_LoadSoundBanks)(*gSoundSystem)[12];
    //sfn_FmodSoundSystem_LoadSoundBanks.detour = (void *)hook_FmodSoundSystem_LoadSoundBanks;
    //smbiCreateAndEnableHook(NULL, &sfn_FmodSoundSystem_LoadSoundBanks);
  }

  if (!sfn_FmodSoundSystem_GetSoundResource.fn) {
    // Virtual function #16.
    sfn_FmodSoundSystem_GetSoundResource.fn = (PFN_FmodSoundSystem_LoadSoundBanks)(*gSoundSystem)[16];
    sfn_FmodSoundSystem_GetSoundResource.detour = (void *)hook_FmodSoundSystem_GetSoundResource;
    smbiCreateAndEnableHook(nullptr, &sfn_FmodSoundSystem_GetSoundResource);
  }

  return result;
}

// ----------------------------------------------------------------------------
// [SECTION] Sky/SmbiFmodPathBarn/exports
// ----------------------------------------------------------------------------

SMB_API_ATTR HTStatus SMB_API SkyEx_FmodSoundSystem_RegisterGuids(
  const LPCSTR *pairs
) {
  if (!pairs)
    return smbiFail(HTError_InvalidParam);

  const char *guid, *eventPath;
  int i = 0;

  while (1) {
    guid = pairs[i];
    if (!guid)
      break;

    eventPath = pairs[i + 1];
    if (!eventPath)
      break;
    i += 2;

    FmodGuid key{guid};
    gFmodPathBarn.AddFmodPath(key, eventPath);
  }

  return smbiSuccess();
}

SMB_API_ATTR HTStatus SMB_API SkyEx_FmodSoundSystem_RegisterGuidByFile(
  HMODULE hModule,
  LPCWSTR path
) {
  return HT_FAIL;
}

SMB_API_ATTR UINT32 SMB_API SkyEx_FmodSoundSystem_LoadBanks(
  UINT32 count,
  const LPCSTR *paths,
  BOOL isAsync
) {
  if (!count || !paths)
    return smbiSetLastError(0, HTError_InvalidParam);
  
  if (!gSoundSystem)
    return smbiSetLastError(0, HTError_AlreadyExists);

  ((PFN_FmodSoundSystem_LoadSoundBanks)sfn_FmodSoundSystem_LoadSoundBanks.origin)(
    gSoundSystem,
    paths,
    count,
    "",
    !!isAsync);

  return smbiSetLastError(count, HTError_Success);
}
