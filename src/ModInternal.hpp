#ifndef __SMB_INTERNAL_H__
#define __SMB_INTERNAL_H__

#include <Windows.h>
#include <string>
#include "htskymodbase.h"
#include "StlAllocator.hpp"

#include "utils/TypeAliases.hpp"
#include "utils/StringUtils.hpp"
#include "utils/SmbiObject.hpp"
#include "utils/PathUtils.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Modbase/Setup
// ----------------------------------------------------------------------------

// Module handle of SkyModBase.
extern HMODULE hModuleDll;

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

#define smbiPrefixLogI(prefix, format, ...) HTTellText("§f[SMB]" prefix "[I] " format, ## __VA_ARGS__)
#define smbiPrefixLogW(prefix, format, ...) HTTellText("§e[SMB]" prefix "[W] " format, ## __VA_ARGS__)
#define smbiPrefixLogE(prefix, format, ...) HTTellText("§c[SMB]" prefix "[E] " format, ## __VA_ARGS__)

#define smbiLogI(format, ...) smbiPrefixLogI("", format, ## __VA_ARGS__)
#define smbiLogW(format, ...) smbiPrefixLogW("", format, ## __VA_ARGS__)
#define smbiLogE(format, ...) smbiPrefixLogE("", format, ## __VA_ARGS__)

#endif
