#ifndef __UTILS_PATHUTILS_HPP__
#define __UTILS_PATHUTILS_HPP__

#include <vector>
#include <filesystem>
#include "includes/htmodloader.h"
#include "utils/TypeAliases.hpp"
#include "utils/StringUtils.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Modbase/Utils/PathUtils
// ----------------------------------------------------------------------------

namespace PathUtils {

static constexpr i32 kMaxPathLen = 1024;

// Get the mod's installation directory.
//
// NOTE: This function will set the error code of HTModLoader.
HTStatus GetModFolder(
  TgcWString &modFolder,
  HMODULE hModuleDll);

// Check whether `dest` is inside the `src` folder.
bool IsWithin(
  const TgcWString &dest,
  const TgcWString &src);
  
// Returns the relative path from src to dest.
TgcWString Relative(
  const TgcWString &dest,
  const TgcWString &src);

// Concat path segments together.
TgcWString Join(
  const std::vector<TgcWString> &segments);

// Return the filename of a path.
static inline TgcWString FileName(
  const TgcWString &fullPath
) {
  std::filesystem::path p(fullPath);
  return p.filename().wstring();
}

}

#endif
