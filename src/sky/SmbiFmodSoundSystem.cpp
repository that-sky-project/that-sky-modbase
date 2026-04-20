#include "sky/SmbiFmodSoundSystem.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Sky/FmodGuid
// ----------------------------------------------------------------------------

FmodGuid::FmodGuid(
  const TgcString &str
) {
  sscanf(
    str.c_str(),
    "{%8x-%4hx-%4hx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx}",
    &Data1, &Data2, &Data3,
    &Data4[0], &Data4[1], &Data4[2], &Data4[3],
    &Data4[4], &Data4[5], &Data4[6], &Data4[7]);
}

TgcString FmodGuid::ToString() const {
  TgcString result(kGuidStringLen, '\0');
  snprintf(
    result.data(),
    kGuidStringLen,
    "{%8x-%4hx-%4hx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx}",
    Data1, Data2, Data3,
    Data4[0], Data4[1], Data4[2], Data4[3],
    Data4[4], Data4[5], Data4[6], Data4[7]);
  return result;
}

bool FmodGuid::operator<(
  const FmodGuid &that
) const {
  return Data1 < that.Data1
    && Data2 < that.Data2
    && Data3 < that.Data3
    && Data4[0] < that.Data4[0]
    && Data4[1] < that.Data4[1]
    && Data4[2] < that.Data4[2]
    && Data4[3] < that.Data4[3]
    && Data4[4] < that.Data4[4]
    && Data4[5] < that.Data4[5]
    && Data4[6] < that.Data4[6]
    && Data4[7] < that.Data4[7];
}

// ----------------------------------------------------------------------------
// [SECTION] Sky/SmbiFmodPathBarn
// ----------------------------------------------------------------------------

void SmbiFmodPathBarn::AddFmodPath(
  const FmodGuid &guid,
  const TgcString &path
) {
  auto lock = LockRW();
  m_savedPath[guid] = path;
}

void SmbiFmodPathBarn::DeleteFmodPath(
  const FmodGuid &guid
) {
  auto lock = LockRW();
  m_savedPath.erase(guid);
}

bool SmbiFmodPathBarn::FindPathByGuid(
  TgcString &path,
  const FmodGuid &guid
) const {
  auto lock = LockW();

  const auto &entry = m_savedPath.find(guid);
  if (entry == m_savedPath.end())
    return false;

  path = entry->second;

  return true;
}
