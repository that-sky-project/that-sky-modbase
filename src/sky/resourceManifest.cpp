#include "smb_internal.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Sky/Resources/declarations
// ----------------------------------------------------------------------------

typedef void *ResourceManifest;

struct ResourceManifestEntry {
  ResourceManifestEntry() = default;

  const char *path;
  const char *bundle;
  const char *externalData;
  int unk[3];
  u32 location;
  u64 hash;
  i08 active;
};

class SmbiResourceManifestEntry {
public:
  SmbiResourceManifestEntry(
    const std::string &path,
    const std::string &bundle
  )
    : path(path)
    , bundle(bundle)
    , externalData("00000000000000000000")
  {
    e.path = this->path.c_str();
    e.bundle = this->bundle.c_str();
    e.externalData = this->externalData.c_str();
  }

  const ResourceManifestEntry *getEntry() const {
    return &e;
  }

private:
  ResourceManifestEntry e;
  std::string path;
  std::string bundle;
  std::string externalData;
};

typedef const ResourceManifestEntry *(__fastcall *PFN_ResourceManifest_LookUp)(
  ResourceManifest *, const char *);

static HTStatus fnInit_ResourceManifest(
  HMODULE,
  const SmbiModInitializer *);
static const ResourceManifestEntry *hook_ResourceManifest_LookUp(
  ResourceManifest *,
  const char *);

// ----------------------------------------------------------------------------
// [SECTION] Sky/Resources/init
// ----------------------------------------------------------------------------

static SmbiModInitializer gInit_ResourceManifest{
  fnInit_ResourceManifest,
  "ResourceManifest::LookUp()"
};

static const HTAsmSig sigE8_ResourceManifest_LookUp = {
  "48 89 CF 48 81 C1 ?  ?  ?  ?  E8 ?  ?  ?  ?  48 "
  "85 C0 0F 84",
  HT_SCAN_E8,
  0x15
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

  sfn_ResourceManifest_LookUp.detour = (void *)hook_ResourceManifest_LookUp;

  return smbiCreateAndEnableHook(
    &sigE8_ResourceManifest_LookUp,
    &sfn_ResourceManifest_LookUp);;
}

// ----------------------------------------------------------------------------
// [SECTION] Sky/Resources/variables
// ----------------------------------------------------------------------------

static std::shared_mutex gMutex;
static std::unordered_map<std::string, SmbiResourceManifestEntry> gSavedEntry;

// ----------------------------------------------------------------------------
// [SECTION] Sky/Resources/functions
// ----------------------------------------------------------------------------

static const ResourceManifestEntry *hook_ResourceManifest_LookUp(
  ResourceManifest *pThis,
  const char *name
) {
  using Tp = const ResourceManifestEntry *;

  Tp p = ((PFN_ResourceManifest_LookUp)sfn_ResourceManifest_LookUp.origin)(
    pThis,
    name);
  if (p)
    return p;

  {
    std::shared_lock lock{gMutex};
    auto it = gSavedEntry.find(name);
    if (it == gSavedEntry.end())
      return nullptr;

    return it->second.getEntry();
  }
}

// Get the mod's folder.
static HTStatus getModFolder(
  std::wstring &modFolder,
  HMODULE hModuleDll
) {
  HTHandle hManifest = HTGetModManifest(hModuleDll);
  if (!hManifest)
    return smbiFail(HTError_ModuleNotFound);

  u32 length = HTGetModInfoFrom(hManifest, HTModInfoFields_Folder, nullptr, 0);
  if (!length)
    return smbiFail(HTError_AccessDenied);

  modFolder.resize(length);
  if (!HTGetModInfoFrom(hManifest, HTModInfoFields_Folder, modFolder.data(), length))
    return smbiFail(HTError_AccessDenied);
}

static HTStatus getBundlePathFor(
  const std::wstring &modFolder,
  std::string &bundlePath
) {
  wchar_t buffer[768];
  constexpr int MAX_LEN = sizeof(buffer) / sizeof(wchar_t);

  // TODO: Maybe we don't need to use HTPathJoin().
  const wchar_t *paths[] = {
    modFolder.c_str(),
    L"assets",
    nullptr
  };
  HTPathJoin(buffer, paths, MAX_LEN);

  // We assume the current working directory is the directory containing the
  // game executable.
  HTPathRelative(buffer, L"data/assets/initial", buffer, MAX_LEN);

  bundlePath = wcstoansi(buffer);
  return HT_SUCCESS;
}

static HTStatus verifyPath(
  std::string &result,
  const std::wstring &modFolder,
  const char *path
) {
  // TODO: Verify the path.
  result = path;
  return HT_SUCCESS;
}

// ----------------------------------------------------------------------------
// [SECTION] Sky/Resources/exports
// ----------------------------------------------------------------------------

SMB_API_ATTR HTStatus SMB_API SkyEx_Resources_RegisterSingleEx(
  HMODULE hModuleDll,
  const char *name,
  const char *path
) {
  if (!hModuleDll || !name || !path)
    return smbiFail(HTError_InvalidHandle);
  
  {
    std::shared_lock lock{gMutex};
    if (gSavedEntry.find(name) != gSavedEntry.end())
      return smbiFail(HTError_AlreadyExists);
  }

  std::wstring modFolder;
  if (!getModFolder(modFolder, hModuleDll))
    return HT_FAIL;

  std::string bundle;
  if (!getBundlePathFor(modFolder, bundle))
    return HT_FAIL;

  std::string realPath;
  if (!verifyPath(realPath, modFolder, path))
    return HT_FAIL;

  {
    std::lock_guard lock{gMutex};
    gSavedEntry.try_emplace(name, SmbiResourceManifestEntry{realPath, bundle});
  }

  return smbiSuccess();
}
