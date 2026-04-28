#include "ModInternal.hpp"

HTStatus smbiGetModFolder(
  TgcWString &modFolder,
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

  return HT_SUCCESS;
}

bool smbiIsPathWithin(
  const TgcWString &dest,
  const TgcWString &src
) {
  wchar_t buffer[kMaxPathLen];

  HTPathRelative(buffer, src.c_str(), dest.c_str(), kMaxPathLen);
  if (HTPathIsAbsolute(buffer) || !memcmp(buffer, L"..", 2))
    return false;

  return true;
}
