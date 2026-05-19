#ifndef __SKY_SMBILEVELLIST_HPP__
#define __SKY_SMBILEVELLIST_HPP__

#include <unordered_map>
#include "ModInternal.hpp"
#include "utils/SmbiObject.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Sky/SmbiLevel
// ----------------------------------------------------------------------------

struct SmbiLevel {
  SmbiLevel(
    const TgcString &levelName
  )
    : levelName(levelName)
  {
    levelHash = StringHash(levelName);
  }

  TgcString levelName = "";
  u32 levelHash = 0;
};

// ----------------------------------------------------------------------------
// [SECTION] Sky/SmbiLevelList
// ----------------------------------------------------------------------------

class SmbiLevelList: public SmbiMutexObject {
public:
  SmbiLevelList() = default;
  ~SmbiLevelList() = default;

  void AddLevel(
    const TgcString &levelName);

  const SmbiLevel *Find(
    const TgcString &levelName
  ) const;

private:
  std::unordered_map<TgcString, SmbiLevel> m_savedLevels = {};
};

#endif
