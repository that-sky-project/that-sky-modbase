#include "ModInternal.hpp"
#include "utils/TypeAliases.hpp"
#include "utils/StringUtils.hpp"
#include "sky/SmbiSkyResource.hpp"
#include "mod/SmbiModInitializer.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Api/Resources/declarations
// ----------------------------------------------------------------------------

typedef void *ResourceManifest;

typedef const ResourceManifestEntry *(__fastcall *PFN_ResourceManifest_LookUp)(
  ResourceManifest *, const char *);

static HTStatus fnInit_ResourceManifest(
  HMODULE,
  const SmbiModInitializer *);
static const ResourceManifestEntry *hook_ResourceManifest_LookUp(
  ResourceManifest *,
  const char *);

// ----------------------------------------------------------------------------
// [SECTION] Api/Resources/variables
// ----------------------------------------------------------------------------

static SmbiSkyResourceBarn gResourceBarn;

// ----------------------------------------------------------------------------
// [SECTION] Api/Resources/init
// ----------------------------------------------------------------------------

static SmbiModInitializer gInit_ResourceManifest{
  fnInit_ResourceManifest,
  "ResourceManifest::LookUp()"
};

static const HTAsmSig sigE8_ResourceManifest_LookUp = {
  "48 89 CF 48 81 C1 ?  ?  ?  ?  E8 ?  ?  ?  ?  48 "
  "85 C0 0F 84",
  HT_SCAN_E8,
  0x0A
};

static HTAsmFunction sfn_ResourceManifest_LookUp = {
  "ResourceManifest::LookUp()",
  nullptr,
  nullptr,
  nullptr
};

static HTStatus fnInit_ResourceManifest(
  HMODULE hModuleDll,
  const SmbiModInitializer *self
) {
  (void)hModuleDll;
  (void)self;

  gResourceBarn.Initialize();

  sfn_ResourceManifest_LookUp.detour = (void *)hook_ResourceManifest_LookUp;

  return smbiCreateAndEnableHook(
    &sigE8_ResourceManifest_LookUp,
    &sfn_ResourceManifest_LookUp);;
}

// ----------------------------------------------------------------------------
// [SECTION] Api/Resources/functions
// ----------------------------------------------------------------------------

static const ResourceManifestEntry *hook_ResourceManifest_LookUp(
  ResourceManifest *pThis,
  const char *name
) {
  const SmbiResourceManifestEntry *p1 = gResourceBarn.Find(name);
  if (p1)
    return p1->GetEntry();

  return ((PFN_ResourceManifest_LookUp)sfn_ResourceManifest_LookUp.origin)(
    pThis,
    name);
}

// Get the assets path of the mod.
static HTStatus getBundlePathFor(
  const std::wstring &modFolder,
  std::string &bundlePath
) {
  wchar_t buffer[kMaxPathLen];

  // TODO: Maybe we don't need to use HTPathJoin().
  const wchar_t *paths[] = {
    modFolder.c_str(),
    L"assets",
    nullptr
  };
  HTPathJoin(buffer, paths, kMaxPathLen);

  // We assume the current working directory is the directory containing the
  // game executable.
  HTPathRelative(buffer, L"data/assets", buffer, kMaxPathLen);

  bundlePath = wcstoansi(buffer);
  return HT_SUCCESS;
}

static HTStatus verifyPath(
  TgcString &result,
  const TgcWString &modFolder,
  cstring path
) {
  wchar_t buffer[kMaxPathLen];
  TgcWString pathWide = ansitowcs(path);

  const wchar_t *paths[] = {
    modFolder.c_str(),
    L"assets",
    pathWide.c_str(),
    nullptr
  };
  HTPathJoin(buffer, paths, kMaxPathLen);

  if (!smbiIsPathWithin(buffer, modFolder))
    return smbiFail(HTError_AccessDenied);

  result = wcstoansi(buffer);
  return HT_SUCCESS;
}

// ----------------------------------------------------------------------------
// [SECTION] Api/Resources/exports
// ----------------------------------------------------------------------------

SMB_API_ATTR HTStatus SMB_API SkyEx_Resources_RegisterSingleEx(
  HMODULE hModule,
  LPCSTR path,
  LPCSTR name,
  BOOL forceUpdate
) {
  if (!hModule)
    return smbiFail(HTError_InvalidHandle);

  if (!name || !path)
    return smbiFail(HTError_InvalidParam);

  if (gResourceBarn.Find(name) && !forceUpdate)
    return smbiFail(HTError_AlreadyExists);

  std::wstring modFolder;
  if (!smbiGetModFolder(modFolder, hModuleDll))
    return HT_FAIL;

  std::string bundle;
  if (!getBundlePathFor(modFolder, bundle))
    return HT_FAIL;

  std::string realPath;
  if (!verifyPath(realPath, modFolder, path))
    return HT_FAIL;

  if (name)
    gResourceBarn.AddEntry(bundle, path, name);
  else
    gResourceBarn.AddEntry(bundle, path);

  return smbiSuccess();
}
