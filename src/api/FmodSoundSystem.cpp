#include <fmod_studio.hpp>
#include "ModInternal.hpp"
#include "mod/MetaBinary.hpp"
#include "mod/SmbiModInitializer.hpp"
#include "sky/SmbiFmodSoundSystem.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Api/FmodSoundSystem/declarations
// ----------------------------------------------------------------------------

typedef void **FmodSoundSystem;
typedef void *FmodSoundResource;

static HTStatus fnInit_FmodSoundSystem(
  HMODULE,
  const SmbiModInitializer *);

static FmodSoundResource *hook_FmodSoundSystem_GetSoundResource(
  FmodSoundSystem *pThis,
  const char *name);

static FMOD_RESULT F_API hook_EventDescription_getPath(
  FMOD::Studio::EventDescription *,
  char *,
  int,
  int *);

static FmodSoundSystem *hook_CreateFmodSoundSystem();

// ----------------------------------------------------------------------------
// [SECTION] Api/FmodSoundSystem/variables
// ----------------------------------------------------------------------------

static SmbiFmodPathBarn gFmodPathBarn;
static SmbiSoundReplacementBarn gSoundReplacementBarn;
static FmodSoundSystem *gSoundSystem = nullptr;

// ----------------------------------------------------------------------------
// [SECTION] Api/FmodSoundSystem/init
// ----------------------------------------------------------------------------

static MetaBinaryFunction bin_EventDescription_GetPath = {
  "FMOD::Studio::EventDescription::getPath()",
  hook_EventDescription_getPath
};

static MetaBinaryFunction bin_CreateFmodSoundSystem = {
  "CreateFmodSoundSystem()",
  hook_CreateFmodSoundSystem
};

static MetaBinaryVirtualFunction bin_FmodSoundSystem_GetSoundResource = {
  "FmodStudioSoundSystem::GetSoundResource()",
  hook_FmodSoundSystem_GetSoundResource
};

static MetaBinaryVirtualFunction bin_FmodSoundSystem_LoadSoundBanks = {
  "FmodStudioSoundSystem::LoadSoundBanks()"
};

static SmbiModInitializer gInit_FmodSoundSystem{
  fnInit_FmodSoundSystem,
  "FmodSoundSystem"
};

// ----------------------------------------------------------------------------
// [SECTION] Api/FmodSoundSystem/functions
// ----------------------------------------------------------------------------

static HTStatus fnInit_FmodSoundSystem(
  HMODULE hModuleDll,
  const SmbiModInitializer *self
) {
  (void)hModuleDll;
  (void)self;

  gFmodPathBarn.Initialize();
  gSoundReplacementBarn.Initialize();

  if (!bin_EventDescription_GetPath.Hook())
    return HT_FAIL;

  if (!bin_CreateFmodSoundSystem.Hook())
    return HT_FAIL;

  return HT_SUCCESS;
}

// Note that this function will affect every event.
static FMOD_RESULT F_API hook_EventDescription_getPath(
  FMOD::Studio::EventDescription *eventdescription,
  char *path,
  int size,
  int *retrieved
) {
  // Get path from Master.strings.bank at first.
  FMOD_RESULT first = bin_EventDescription_GetPath.Call<FMOD_RESULT>(
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

  // Return the result.
  int num = (int)result.length();
  strncpy(path, result.c_str(), size);
  path[size - 1] = 0;
  *retrieved = num > size ? size : num;

  return FMOD_OK;
}

static FmodSoundResource *hook_FmodSoundSystem_GetSoundResource(
  FmodSoundSystem *pThis,
  const char *name
) {
  TgcString realName = gSoundReplacementBarn.GetActualSoundResource(name);
  FmodSoundResource *result = bin_FmodSoundSystem_GetSoundResource.Call<FmodSoundResource *>(
    pThis, realName.c_str());
  return result;
}

static FmodSoundSystem *hook_CreateFmodSoundSystem() {
  FmodSoundSystem *result = bin_CreateFmodSoundSystem.Call<FmodSoundSystem *>();

  if (!gSoundSystem) {
    gSoundSystem = result;
    smbiLogI("gSoundSystem = %p", gSoundSystem);
  }

  bin_FmodSoundSystem_GetSoundResource.Hook(gSoundSystem);

  return result;
}

// ----------------------------------------------------------------------------
// [SECTION] Api/FmodSoundSystem/exports
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
    return smbiSetLastError(0, HTError_AccessDenied);

  bin_FmodSoundSystem_LoadSoundBanks.Call<void>(
    gSoundSystem,
    paths,
    count,
    "",
    !!isAsync);

  return smbiSetLastError(count, HTError_Success);
}

SMB_API_ATTR HTStatus SMB_API SkyEx_FmodSoundSystem_ReplaceSoundResource(
  LPCSTR src,
  LPCSTR dest
) {
  gSoundReplacementBarn.Replace(src, dest);
  return smbiSuccess();
}

SMB_API_ATTR HTStatus SMB_API SkyEx_FmodSoundSystem_RestoreSoundResource(
  LPCSTR name
) {
  gSoundReplacementBarn.Restore(name);
  return smbiSuccess();
}
