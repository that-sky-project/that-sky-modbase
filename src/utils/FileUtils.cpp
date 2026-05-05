#include "utils/FileUtils.hpp"

TgcString FileUtils::ReadFile(
  const TgcWString &path
) {
  HANDLE hFile = CreateFileW(
    path.c_str(),
    GENERIC_READ,
    FILE_SHARE_READ,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    nullptr);

  if (hFile == INVALID_HANDLE_VALUE)
    goto ErrRet;

  {
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hFile, &fileSize))
      goto ErrRet;

    if (fileSize.QuadPart > MAXDWORD)
      goto ErrRet;

    u32 targetSize = (u32)fileSize.QuadPart;
    TgcString content(targetSize, '\0');

    DWORD bytesRead = 0;
    if (!::ReadFile(hFile, &content[0], targetSize, &bytesRead, NULL))
      goto ErrRet;
    if (bytesRead != targetSize)
      goto ErrRet;

    CloseHandle(hFile);

    return content;
  }

ErrRet:
  if (hFile != INVALID_HANDLE_VALUE)
    CloseHandle(hFile);

  return "";
}
