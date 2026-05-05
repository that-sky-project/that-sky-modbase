#include <fstream>
#include <filesystem>
#include <includes/htmod.h>
#include "ModInternal.hpp"
#include "mod/SmbiBinaryDescriptor.hpp"
#include "mod/SmbiModInitializer.hpp"
#include "mod/MetaBinary.hpp"

static SmbiBinaryDescriptorBarn gBinaryDescriptorBarn;
static MetaBinarySystem gMetaBinarySystem;

__declspec(dllexport) HTStatus HTMLAPI HTModOnInit(
  void *reserved
) {
  TgcWString folder;
  if (!PathUtils::GetModFolder(folder, hModuleDll))
    return HT_FAIL;

  TgcWString file = PathUtils::Join({folder, L"assets/Data/Manifests/Binary.json"});
  cJSON *json = cJSON_Parse(FileUtils::ReadFile(file).c_str());

  gBinaryDescriptorBarn.Initialize(json);
  gMetaBinarySystem.Initialize();

  gBinaryDescriptorBarn.ResolveAll();
  gMetaBinarySystem.Enable(&gBinaryDescriptorBarn);

  if (json)
    cJSON_Delete(json);

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
