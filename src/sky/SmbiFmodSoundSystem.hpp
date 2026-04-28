#ifndef __SKY_SMBIFMODPATH_HPP__
#define __SKY_SMBIFMODPATH_HPP__

#include <map>
#include <string>
#include <unordered_map>
#include "fmod_studio.hpp"
#include "utils/TypeAliases.hpp"
#include "utils/SmbiObject.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Sky/FmodGuid
// ----------------------------------------------------------------------------

struct FmodGuid: public FMOD_GUID {
  static constexpr int kGuidStringLen = 36;

  FmodGuid() = default;
  FmodGuid(const TgcString &str);

  TgcString ToString() const;

  bool operator<(const FmodGuid &that) const;
};

// ----------------------------------------------------------------------------
// [SECTION] Sky/SmbiFmodPathBarn
// ----------------------------------------------------------------------------

class SmbiFmodPathBarn: public SmbiMutexObject {
public:
  SmbiFmodPathBarn() = default;
  ~SmbiFmodPathBarn() = default;

  void Initialize();

  void AddFmodPath(
    const FmodGuid &guid,
    const TgcString &path);

  void DeleteFmodPath(
    const FmodGuid &guid);

  bool FindPathByGuid(
    TgcString &path,
    const FmodGuid &guid
  ) const;

private:
  std::map<FmodGuid, TgcString> m_savedPath;
};

// ----------------------------------------------------------------------------
// [SECTION] Sky/SmbiSoundReplacementBarn
// ----------------------------------------------------------------------------

class SmbiSoundReplacementBarn: public SmbiMutexObject {
public:
  SmbiSoundReplacementBarn() = default;
  ~SmbiSoundReplacementBarn() = default;

  void Initialize();

  // Replace `res` with `with`.
  void Replace(
    const TgcString &res,
    const TgcString &with);

  // Restore replaced sound resource `res`.
  void Restore(
    const TgcString &res);

  // Get the actual sound resource name of `res`.
  TgcString GetActualSoundResource(
    const TgcString &res
  ) const;

private:
  std::unordered_map<TgcString, TgcString> m_savedReplacement;
};

#endif
