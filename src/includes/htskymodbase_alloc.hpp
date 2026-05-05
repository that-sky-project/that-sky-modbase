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
// - htskymodbase_alloc.hpp
// C++ STL allocator of HTSkyModbase .
// ----------------------------------------------------------------------------

#ifndef __HTSKYMODBASE_ALLOC_HPP__
#define __HTSKYMODBASE_ALLOC_HPP__

#include <new>
#include "htskymodbase.h"

// ----------------------------------------------------------------------------
// [SECTION] Api/cpp/Memory
// ----------------------------------------------------------------------------

#ifdef _MSC_VER
#pragma warning(disable: 4595)
#pragma warning(disable: 28251)
#endif

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

#endif
