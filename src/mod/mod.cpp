#include "includes/htmod.h"

#include "ModInternal.hpp"
#include "mod/SmbiModInitializer.hpp"

__declspec(dllexport) HTStatus HTMLAPI HTModOnInit(
  void *reserved
) {
  SmbiModInitializer::SetupAll(hModuleDll);

  return HT_SUCCESS;
}

__declspec(dllexport) HTStatus HTMLAPI HTModOnEnable(
  void *reserved
) {
  return HT_SUCCESS;
}

__declspec(dllexport) void HTMLAPI HTModRenderGui(
  float timeElapesed,
  void *reserved
) {
  ;
}
