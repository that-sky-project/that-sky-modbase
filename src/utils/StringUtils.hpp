#ifndef __UTILS_STRINGUTILS_HPP__
#define __UTILS_STRINGUTILS_HPP__

#include <string>
#include "utils/TypeAliases.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Modbase/Utils/StringCast
// ----------------------------------------------------------------------------

// Cast from  to UTF16-LE.
std::wstring utf8towcs(
  const std::string &s);

// Cast from UTF16-LE to UTF-8.
std::string wcstoutf8(
  const std::wstring &s);

// Cast from ANSI to UTF16-LE.
std::wstring ansitowcs(
  const std::string &s);

// Cast from UTF16-LE to ANSI.
std::string wcstoansi(
  const std::wstring &s);

#endif
