#include "sky/SmbiSkyResource.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Sky/SmbiSkyResourceBarn
// ----------------------------------------------------------------------------

void SmbiSkyResourceBarn::Add(
  const TgcString &bundle,
  const TgcString &path,
  const TgcString &name
) {
  auto lock = LockRW();
  m_savedEntry.try_emplace(name, SmbiResourceManifestEntry{path, bundle});
}

void SmbiSkyResourceBarn::Add(
  const TgcString &bundle,
  const TgcString &path
) {
  Add(bundle, path, path);
}

const SmbiResourceManifestEntry *SmbiSkyResourceBarn::Find(
  const TgcString &name
) const {
  auto lock = LockW();

  auto it = m_savedEntry.find(name);
  if (it == m_savedEntry.end())
    return nullptr;

  return &it->second;
}


