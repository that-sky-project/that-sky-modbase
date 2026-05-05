#include "ModInternal.hpp"
#include "utils/TypeAliases.hpp"
#include "utils/StringUtils.hpp"
#include "sky/SmbiSkyResource.hpp"
#include "mod/SmbiModInitializer.hpp"
#include "mod/MetaBinary.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Api/Resources/declarations
// ----------------------------------------------------------------------------

typedef void *ResourceManifest;

static HTStatus fnInit_Resources(
  HMODULE hModuleDll,
  const SmbiModInitializer *self);
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

static MetaBinaryFunction bin_ResourceManifest_LookUp = {
  "ResourceManifest::LookUp()",
  hook_ResourceManifest_LookUp
};

static SmbiModInitializer gInit_Resources = {
  fnInit_Resources,
  "Resources.cpp"
};

static HTStatus fnInit_Resources(
  HMODULE hModuleDll,
  const SmbiModInitializer *self
) {
  (void)hModuleDll;
  (void)self;

  gResourceBarn.Initialize();

  return bin_ResourceManifest_LookUp.Hook()
    ? HT_SUCCESS
    : HT_FAIL;
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

  return bin_ResourceManifest_LookUp.Call<const ResourceManifestEntry *>(
    pThis,
    name);
}

// Get the assets path of the mod.
static HTStatus getBundlePathFor(
  const std::wstring &modFolder,
  std::string &bundlePath
) {
  // TODO: Maybe we don't need to use HTPathJoin().
  TgcWString t = PathUtils::Join({modFolder.c_str(), L"assets"});

  // We assume the current working directory is the directory containing the
  // game executable.
  t = PathUtils::Relative(t, L"data/assets");

  bundlePath = wcstoansi(t);

  return HT_SUCCESS;
}

static HTStatus verifyPath(
  const TgcWString &modFolder,
  cstring path
) {
  TgcWString pathWide = ansitowcs(path);
  TgcWString t = PathUtils::Join({modFolder.c_str(), L"assets", pathWide.c_str()});

  if (!PathUtils::IsWithin(t, modFolder))
    return smbiFail(HTError_AccessDenied);

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

  if (!path)
    return smbiFail(HTError_InvalidParam);

  if (!name)
    name = path;

  if (gResourceBarn.Find(name) && !forceUpdate)
    return smbiFail(HTError_AlreadyExists);

  std::wstring modFolder;
  if (!PathUtils::GetModFolder(modFolder, hModule))
    return HT_FAIL;

  std::string bundle;
  if (!getBundlePathFor(modFolder, bundle))
    return HT_FAIL;

  if (!verifyPath(modFolder, path))
    return HT_FAIL;

  if (name)
    gResourceBarn.AddEntry(bundle, path, name);
  else
    gResourceBarn.AddEntry(bundle, path);

  return smbiSuccess();
}
