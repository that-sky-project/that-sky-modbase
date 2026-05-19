#include "ModInternal.hpp"
#include "mod/Mod.hpp"
#include "mod/SmbiBinaryDescriptor.hpp"
#include "mod/SmbiModInitializer.hpp"
#include "mod/MetaBinary.hpp"

static SmbiBinaryDescriptorBarn gBinaryDescriptorBarn;
static MetaBinarySystem gMetaBinarySystem;

Mod *gMod = nullptr;

// ----------------------------------------------------------------------------
// [SECTION] Mod/Mod
// ----------------------------------------------------------------------------

Mod::~Mod() {
  delete resourceBarn;
  delete fmodPathBarn;
  delete soundReplacementBarn;
}

void Mod::Alloc() {
  resourceBarn          = new SmbiSkyResourceBarn();
  fmodPathBarn          = new SmbiFmodPathBarn();
  soundReplacementBarn  = new SmbiSoundReplacementBarn();
}

void Mod::Initialize() {
  SmbiModInitializer::SetupAll(hModule);
}

// ----------------------------------------------------------------------------
// [SECTION] Mod/Entry
// ----------------------------------------------------------------------------

void Entry(
  HMODULE hModule
) {
  gMod = new Mod();

  TgcWString folder;
  if (!PathUtils::GetModFolder(folder, hModule))
    return;

  TgcWString file = PathUtils::Join({folder, L"assets/Data/Manifests/Binary.json"});
  cJSON *json = cJSON_Parse(FileUtils::ReadFile(file).c_str());

  gBinaryDescriptorBarn.Initialize(json);
  gMetaBinarySystem.Initialize();

  gBinaryDescriptorBarn.ResolveAll();
  gMetaBinarySystem.Enable(&gBinaryDescriptorBarn);

  if (json)
    cJSON_Delete(json);

  gMod->hModule = hModule;
  gMod->metaBinarySystem = &gMetaBinarySystem;
  gMod->binaryDescriptorBarn = &gBinaryDescriptorBarn;
  gMod->Alloc();

  gMod->Initialize();
}
