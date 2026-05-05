#ifndef __UTILS_FILEUTILS_HPP__
#define __UTILS_FILEUTILS_HPP__

#include "utils/TypeAliases.hpp"
#include "utils/PathUtils.hpp"

namespace FileUtils {

TgcString ReadFile(
  const TgcWString &path);

}

#endif
