#include "SmbiLevelList.hpp"

void SmbiLevelList::AddLevel(
  const TgcString &levelName
) {
  auto lock = LockRW();

  m_savedLevels.emplace(levelName, SmbiLevel{levelName});
}

const SmbiLevel *SmbiLevelList::Find(
  const TgcString &levelName
) const {
  auto lock = LockW();

  const auto &it = m_savedLevels.find(levelName);
  if (it == m_savedLevels.end())
    return nullptr;
  return &it->second;
}
