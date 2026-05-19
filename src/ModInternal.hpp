#ifndef __SMB_INTERNAL_H__
#define __SMB_INTERNAL_H__

#include <Windows.h>
#include <string>
#include "htskymodbase.h"
#include "htskymodbase_meta.h"
#include "StlAllocator.hpp"

#include "utils/TypeAliases.hpp"
#include "utils/StringUtils.hpp"
#include "utils/SmbiObject.hpp"
#include "utils/PathUtils.hpp"
#include "utils/FileUtils.hpp"
#include "utils/HookUtils.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Modbase/Helper
// ----------------------------------------------------------------------------

#define smbiSetLastError(r, e) (HTSetLastError(e), (r))
#define smbiFail(e) smbiSetLastError(HT_FAIL, e)
#define smbiSuccess() smbiSetLastError(HT_SUCCESS, HTError_Success)

#define smbiPrefixLogI(prefix, format, ...) HTTellText("§f[SMB]" prefix "[I] " format, ## __VA_ARGS__)
#define smbiPrefixLogW(prefix, format, ...) HTTellText("§e[SMB]" prefix "[W] " format, ## __VA_ARGS__)
#define smbiPrefixLogE(prefix, format, ...) HTTellText("§c[SMB]" prefix "[E] " format, ## __VA_ARGS__)

#define smbiLogI(format, ...) smbiPrefixLogI("", format, ## __VA_ARGS__)
#define smbiLogW(format, ...) smbiPrefixLogW("", format, ## __VA_ARGS__)
#define smbiLogE(format, ...) smbiPrefixLogE("", format, ## __VA_ARGS__)

#endif
