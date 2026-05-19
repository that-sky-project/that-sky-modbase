#include "utils/HookUtils.hpp"
#include "mod/Mod.hpp"

HTStatus HookUtils::CreateAndEnableHook(
  const HTAsmSig *sigXX,
  HTAsmFunction *sfn
) {
  if (sigXX && !HTSigScanFunc(sigXX, sfn))
    return HT_FAIL;

  if (!HTAsmHookCreate(gMod->hModule, sfn))
    return HT_FAIL;

  return HTAsmHookEnable(gMod->hModule, sfn->fn);
}
