#ifndef __UTILS_STRINGUTILS_HPP__
#define __UTILS_STRINGUTILS_HPP__

#include <string>
#include "utils/TypeAliases.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Modbase/Utils/StringCast
// ----------------------------------------------------------------------------

// Cast from UTF-8 to UTF16-LE.
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

// ----------------------------------------------------------------------------
// [SECTION] Modbase/Utils/StringHash
// ----------------------------------------------------------------------------

static inline u32 StringHash(
  const TgcString &str
) {
  constexpr u32 kFnv1aBase = 2166136261;
  constexpr u32 kFnv1aPrime = 16777619;
  u32 hash = kFnv1aBase;
  for (i08 ch: str) {
    hash ^= (u08)ch;
    hash *= kFnv1aPrime;
  }
  return hash;
}

#endif
