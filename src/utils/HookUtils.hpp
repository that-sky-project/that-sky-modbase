#ifndef __UTILS_HOOKUTILS_HPP__
#define __UTILS_HOOKUTILS_HPP__

#include "includes/htmodloader.h"

// ----------------------------------------------------------------------------
// [SECTION] Modbase/HookUtils
// ----------------------------------------------------------------------------

namespace HookUtils {

HTStatus CreateAndEnableHook(
  const HTAsmSig *sigXX,
  HTAsmFunction *sfn
);

}

#endif
