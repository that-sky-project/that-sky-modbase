#ifndef __MOD_MOD_HPP__
#define __MOD_MOD_HPP__

#include "utils/SmbiObject.hpp"
#include "mod/MetaBinary.hpp"
#include "mod/SmbiBinaryDescriptor.hpp"
#include "sky/SmbiFmodSoundSystem.hpp"
#include "sky/SmbiSkyResource.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Mod/Mod
// ----------------------------------------------------------------------------

class Mod: public SmbiMutexObject {
public:
  Mod() = default;
  ~Mod();

  void Alloc();

  void Initialize();

  // Module handle of SkyModBase.
  HMODULE hModule = nullptr;

  // - Mod objects.
  MetaBinarySystem *metaBinarySystem = nullptr;
  SmbiBinaryDescriptorBarn *binaryDescriptorBarn = nullptr;
  SmbiSkyResourceBarn *resourceBarn = nullptr;
  SmbiFmodPathBarn *fmodPathBarn = nullptr;
  SmbiSoundReplacementBarn *soundReplacementBarn = nullptr;
};

void Entry(
  HMODULE hModule);

// ----------------------------------------------------------------------------
// [SECTION] Mod/variables
// ----------------------------------------------------------------------------

extern Mod *gMod;

#endif
