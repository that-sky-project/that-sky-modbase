// ----------------------------------------------------------------------------
// HTSkyModBase
// - A Simple mod developed for exporting in-game funtions of Sky: CotL.
// <https://www.github.com/HTMonkeyG/HTSkyModBase>
//
// MIT License
//
// Copyright (c) 2025 HTMonkeyG
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// ----------------------------------------------------------------------------
// - htskymodbase.hpp
// C++ encapsulation of HTSkyModbase APIs.
// ----------------------------------------------------------------------------

#ifndef __HTSKYMODBASE_HPP__
#define __HTSKYMODBASE_HPP__

#include <new>
#include <vector>
#include <string>
#include "htskymodbase.h"

// ----------------------------------------------------------------------------
// [SECTION] Api/cpp/Memory
// ----------------------------------------------------------------------------

// Basic allocator override.
inline void *operator new(
  size_t _Size
) {
  if (!_Size) _Size = 1;

  void *raw = Sky_NewImpl(_Size);
  if (!raw) throw std::bad_alloc();

  return raw;
}

inline void *operator new[](size_t _Size) {
  return operator new(_Size);
}

inline void operator delete(
  void *_Block
) noexcept {
  return Sky_DeleteImpl(_Block);
}

inline void operator delete[](
  void *_Block
) noexcept {
  return operator delete(_Block);
}

// Nothrow allocator override.
inline void *operator new(
  std::size_t _Size,
  const std::nothrow_t &
) noexcept {
  if (!_Size) _Size = 1;
  return Sky_NewImpl(_Size);
}

inline void *operator new[](
  size_t _Size,
  const std::nothrow_t &_Tag
) noexcept {
  return operator new(_Size, _Tag);
}

inline void operator delete(
  void *_Block,
  const std::nothrow_t &
) noexcept {
  return operator delete(_Block);
}

inline void operator delete[](
  void *_Block,
  const std::nothrow_t &
) noexcept {
  return operator delete(_Block);
}

// ----------------------------------------------------------------------------
// [SECTION] Api/cpp/SkyEx
// ----------------------------------------------------------------------------

namespace SkyEx {

namespace Resources {

static HTStatus RegisterSingleEx(
  HMODULE hModule,
  LPCSTR path,
  LPCSTR name,
  BOOL forceUpdate = 0
) {
  return SkyEx_Resources_RegisterSingleEx(hModule, path, name, forceUpdate);
}

static HTStatus RegisterSingleEx(
  HMODULE hModule,
  LPCSTR path,
  BOOL forceUpdate = 0
) {
  return SkyEx_Resources_RegisterSingleEx(hModule, path, nullptr, forceUpdate);
}

// namespace Resources
}

namespace FmodSoundSystem {

static HTStatus RegisterGuids(
  const LPCSTR *pairs
) {
  return SkyEx_FmodSoundSystem_RegisterGuids(pairs);
}

static HTStatus RegisterGuids(
  const std::vector<std::string> &pairs
) {
  std::vector<const char *> cstringPairs;
  for (auto &s: pairs)
    cstringPairs.push_back(s.c_str());
  cstringPairs.push_back(nullptr);
  return SkyEx_FmodSoundSystem_RegisterGuids(cstringPairs.data());
}

static HTStatus RegisterGuids(
  const std::vector<std::pair<std::string, std::string>> &pairs
) {
  std::vector<const char *> cstringPairs;
  for (auto &p: pairs) {
    cstringPairs.push_back(p.first.c_str());
    cstringPairs.push_back(p.second.c_str());
  }
  cstringPairs.push_back(nullptr);
  return SkyEx_FmodSoundSystem_RegisterGuids(cstringPairs.data());
}

}

// namespace SkyEx
}

// ----------------------------------------------------------------------------
// [SECTION] Api/cpp/Sky
// ----------------------------------------------------------------------------

namespace Sky {

}

#endif
