#ifndef __SMB_INTERNAL_H__
#define __SMB_INTERNAL_H__

#include <windows.h>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <algorithm>

#include "htskymodbase.h"

// ----------------------------------------------------------------------------
// [SECTION] Modbase/TypeAliases
// ----------------------------------------------------------------------------

#define i08 char
#define u08 unsigned char
#define i16 short
#define u16 unsigned short
#define i32 int
#define u32 unsigned int
#define i64 long long
#define u64 unsigned long long
#define f32 float
#define f64 double
#define nil void

// ----------------------------------------------------------------------------
// [SECTION] Modbase/Setup
// ----------------------------------------------------------------------------

// Module handle of SkyModBase.
extern HMODULE hModuleDll;

// Initializer class.
class SmbiModInitializer {
public:
  using PFN_DoInit = HTStatus (*)(
    HMODULE, const SmbiModInitializer *);

  static inline SmbiModInitializer *&list() {
    static SmbiModInitializer *p = nullptr;
    return p;
  }

  static HTStatus setupAll(
    HMODULE hModuleDll);

  // Register a initializer.
  SmbiModInitializer(
    PFN_DoInit _function,
    const char *_name
  ) 
    : function(_function)
    , name(_name)
  {
    prev = SmbiModInitializer::list();
    SmbiModInitializer::list() = this;
  }

  ~SmbiModInitializer() = default;

  inline const SmbiModInitializer *getPrev() const {
    return prev;
  }

  inline const char *getName() const {
    return name;
  }

  HTStatus operator()(
    HMODULE hModuleDll
  ) const {
    if (function)
      return function(hModuleDll, this);
    else
      return HT_FAIL;
  }

private:
  SmbiModInitializer *prev;
  PFN_DoInit function;
  const char *name;

  void *operator new(size_t) = delete;
  static void* operator new[](size_t) = delete;
  void operator delete(void *) = delete;
  void operator delete[](void *) = delete;

  SmbiModInitializer(const SmbiModInitializer &) = delete;
  SmbiModInitializer &operator=(const SmbiModInitializer &) = delete;
};

// ----------------------------------------------------------------------------
// [SECTION] Modbase/Helper
// ----------------------------------------------------------------------------

#define smbiSetLastError(r, e) (HTSetLastError(e), (r))
#define smbiFail(e) smbiSetLastError(HT_FAIL, e)
#define smbiSuccess() smbiSetLastError(HT_SUCCESS, HTError_Success)

// Create and enable a hook.
static inline HTStatus smbiCreateAndEnableHook(
  const HTAsmSig *sigXX,
  HTAsmFunction *sfn
) {
  if (sigXX && !HTSigScanFunc(sigXX, sfn))
    return HT_FAIL;

  if (!HTAsmHookCreate(hModuleDll, sfn))
    return HT_FAIL;

  return HTAsmHookEnable(hModuleDll, sfn->fn);
}

static inline void smbiPathAppendSimple(
  std::wstring &path,
  const std::wstring &s
) {
  int c = 0;
  if (path[path.length() - 1] == L'\\' || path[path.length() - 1] == L'/')
    c++;
  if (s[0] == L'\\' || s[0] == L'/')
    c++;

  if (c == 2)
    path += s.substr(1);
  else if (c == 1)
    path += s;
  else
    path += L'\\' + s;
}

// ----------------------------------------------------------------------------
// [SECTION] Modbase/Helper/StringCast
// ----------------------------------------------------------------------------

// String cast functions.
static inline std::wstring utf8towcs(
  const std::string &s
) {
  if (s.empty())
    return L"";

  i32 len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
  if (len == 0)
    return L"";

  std::wstring wstr(len, L'\0');
  i32 written = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, wstr.data(), len);
  if (!written)
    return L"";

  wstr.resize(written - 1);
  return wstr;
}

static inline std::string wcstoutf8(
  const std::wstring &s
) {
  if (s.empty())
    return "";

  int len = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, nullptr, 0, nullptr, nullptr);
  if (len == 0)
    return "";

  std::string str(len, '\0');
  int written = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, &str[0], len, nullptr, nullptr);
  if (written == 0)
    return "";

  str.resize(written - 1);
  return str;
}

static inline std::wstring ansitowcs(
  const std::string &s
) {
  if (s.empty())
    return L"";

  i32 len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, nullptr, 0);
  if (len == 0)
    return L"";

  std::wstring wstr(len, L'\0');
  i32 written = MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, wstr.data(), len);
  if (!written)
    return L"";

  wstr.resize(written - 1);
  return wstr;
}

static inline std::string wcstoansi(
  const std::wstring &s
) {
  if (s.empty())
    return "";

  int len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), -1, nullptr, 0, nullptr, nullptr);
  if (len == 0)
    return "";

  std::string str(len, '\0');
  int written = WideCharToMultiByte(CP_ACP, 0, s.c_str(), -1, &str[0], len, nullptr, nullptr);
  if (written == 0)
    return "";

  str.resize(written - 1);
  return str;
}

#endif
