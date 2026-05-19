#include "htskymodbase.h"
#include "mod/SmbiModInitializer.hpp"
#include "mod/Mod.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Api/Definitions/declarations
// ----------------------------------------------------------------------------

typedef void *NetLevelList;

static void hook_NetLevelList_OnLevelLoad(
  NetLevelList *,
  cstring);

static HTStatus fnInit_Definitions(
  HMODULE,
  const SmbiModInitializer *);

// ----------------------------------------------------------------------------
// [SECTION] Api/Definitions/init
// ----------------------------------------------------------------------------

static SmbiModInitializer gInit_LifeCycleHook = {
  fnInit_Definitions,
  "Definitions"
};

static MetaBinaryFunction bin_NetLevelList_OnLevelLoad = {
  "NetLevelList::OnLevelLoad()",
  hook_NetLevelList_OnLevelLoad
};

static MetaBinaryMemberOffset bin_NetLevelList_levelId = {
  "NetLevelList::levelId"
};

static HTStatus fnInit_Definitions(
  HMODULE hModuleDll,
  const SmbiModInitializer *self
) {
  (void)hModuleDll;
  (void)self;

  gMod->levelList->Initialize();

  if (!bin_NetLevelList_OnLevelLoad.Hook())
    return HT_FAIL;

  return HT_SUCCESS;
}

// ----------------------------------------------------------------------------
// [SECTION] Api/Definitions/functions
// ----------------------------------------------------------------------------

static void hook_NetLevelList_OnLevelLoad(
  NetLevelList *self,
  cstring levelName
) {
  const SmbiLevel *entry = gMod->levelList->Find(levelName);
  if (entry)
    bin_NetLevelList_levelId.ResolveRef<u32>(self) = entry->levelHash;
  else
    bin_NetLevelList_OnLevelLoad.Call<void>(self, levelName);
}

// ----------------------------------------------------------------------------
// [SECTION] Api/Definitions/exports/lua
// ----------------------------------------------------------------------------

SMB_API_ATTR VOID SMB_API SkyEx_Definitions_Submit(
  HMODULE hModuleDll
) {

}

// ----------------------------------------------------------------------------
// [SECTION] Api/Definitions/exports/level
// ----------------------------------------------------------------------------

SMB_API_ATTR HTStatus SMB_API SkyEx_Definitions_RegisterLevel(
  LPCSTR levelName
) {
  if (!levelName)
    return smbiFail(HTError_InvalidParam);

  gMod->levelList->AddLevel(levelName);

  return smbiSuccess();
}

SMB_API_ATTR HTStatus SMB_API SkyEx_Definitions_RegisterLevelAsFile(
  HMODULE hModule,
  LPCSTR levelName
) {
  char buffer[0x40];
  HTStatus s;

  if (!hModule)
    return smbiFail(HTError_InvalidHandle);
  if (!levelName || strlen(levelName) >= 0x20)
    return smbiFail(HTError_InvalidParam);

  snprintf(buffer, sizeof(buffer), "Data/Levels/%s/Resources.lua", levelName);
  s = SkyEx_Resources_RegisterSingleEx(hModule, buffer, nullptr, false);
  if (!s) return HT_FAIL;

  snprintf(buffer, sizeof(buffer), "Data/Levels/%s/Objects.level.bin", levelName);
  s = SkyEx_Resources_RegisterSingleEx(hModule, buffer, nullptr, false);
  if (!s) return HT_FAIL;

  snprintf(buffer, sizeof(buffer), "Data/Levels/%s/BstBaked.meshes", levelName);
  s = SkyEx_Resources_RegisterSingleEx(hModule, buffer, nullptr, false);
  if (!s) return HT_FAIL;

  gMod->levelList->AddLevel(levelName);

  return smbiSuccess();
}
