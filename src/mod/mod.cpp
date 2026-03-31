#include "smb_internal.hpp"

__declspec(dllexport) HTStatus HTMLAPI HTModOnInit(
  void *reserved
) {
  SmbiModInitializer::setupAll(hModuleDll);

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
