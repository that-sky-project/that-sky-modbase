#ifndef __UTILS_PATHUTILS_HPP__
#define __UTILS_PATHUTILS_HPP__

#include "includes/htmodloader.h"
#include "utils/TypeAliases.hpp"
#include "utils/StringUtils.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Modbase/Utils/PathUtils
// ----------------------------------------------------------------------------

static constexpr i32 kMaxPathLen = 1024;

// Get the mod's installation directory.
//
// NOTE: This function will set the error code of HTModLoader.
HTStatus smbiGetModFolder(
  TgcWString &modFolder,
  HMODULE hModuleDll);

// Check whether `dest` is inside the `src` folder.
bool smbiIsPathWithin(
  const TgcWString &dest,
  const TgcWString &src);

#endif
