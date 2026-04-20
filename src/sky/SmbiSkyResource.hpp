#ifndef __SKY_SMBISKYRESOURCE_HPP__
#define __SKY_SMBISKYRESOURCE_HPP__

#include <unordered_map>
#include <string>
#include "utils/SmbiObject.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Sky/ResourceManifestEntry
// ----------------------------------------------------------------------------

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
    const TgcString &path,
    const TgcString &bundle
  )
    : path(path)
    , bundle(bundle)
    , externalData("00000000000000000000")
  {
    e.path = this->path.c_str();
    e.bundle = this->bundle.c_str();
    e.externalData = this->externalData.c_str();
  }

  const ResourceManifestEntry *GetEntry() const {
    return &e;
  }

private:
  ResourceManifestEntry e;
  TgcString path;
  TgcString bundle;
  TgcString externalData;
};

// ----------------------------------------------------------------------------
// [SECTION] Sky/SmbiSkyResourceBarn
// ----------------------------------------------------------------------------

class SmbiSkyResourceBarn: public SmbiMutexObject {
public:
  SmbiSkyResourceBarn() = default;
  ~SmbiSkyResourceBarn() = default;

  // Add an entry with a name. Overwrite if an entry with the same name already
  // exists.
  // The actual resource path is "data/assets/<bundle>/<path>"
  void AddEntry(
    const TgcString &bundle,
    const TgcString &path,
    const TgcString &name);

  // Add an entry with the path as its name.
  void AddEntry(
    const TgcString &bundle,
    const TgcString &path);

  const SmbiResourceManifestEntry *Find(
    const TgcString &name
  ) const;

private:
  std::unordered_map<TgcString, SmbiResourceManifestEntry> m_savedEntry;
};

#endif
