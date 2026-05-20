#include "ModInternal.hpp"
#include "mod/SmbiModInitializer.hpp"
#include "mod/MetaBinary.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Api/LifeCycleHook/declarations
// ----------------------------------------------------------------------------

typedef void *ModuleBarn;

typedef int ModuleFlags;
enum ModuleFlags_ {
  ModuleFlags_Core = 1 << 0,
  ModuleFlags_Intro = 1 << 1,
  ModuleFlags_Game = 1 << 2,
  ModuleFlags_Dev = 1 << 3,
  ModuleFlags_Disabled = 1 << 4,
};

typedef void (__fastcall *PFN_ModuleBarn_CallFunction)(
  ModuleBarn *, const char *, ModuleFlags, u32, u08, void *);

static HTStatus fnInit_LifeCycleHook(
  HMODULE,
  const SmbiModInitializer *);

void hook_ModuleBarn_CallFunction(
  ModuleBarn *pThis,
  const char *name,
  ModuleFlags flag,
  u32 direction,
  u08 a5,
  void *a6);

// ----------------------------------------------------------------------------
// [SECTION] Api/LifeCycleHook/init
// ----------------------------------------------------------------------------

static SmbiModInitializer gInit_LifeCycleHook = {
  fnInit_LifeCycleHook,
  "LifeCycleHook"
};

static MetaBinaryFunction bin_ModuleBarn_CallFunction = {
  "ModuleBarn::CallFunction()",
  hook_ModuleBarn_CallFunction,
};

// ----------------------------------------------------------------------------
// [SECTION] Api/LifeCycleHook/functions
// ----------------------------------------------------------------------------

static HTStatus fnInit_LifeCycleHook(
  HMODULE hModuleDll,
  const SmbiModInitializer *self
) {
  (void)hModuleDll;
  (void)self;

  return bin_ModuleBarn_CallFunction.Hook()
    ? HT_SUCCESS
    : HT_FAIL;
}

void hook_ModuleBarn_CallFunction(
  ModuleBarn *pThis,
  const char *name,
  ModuleFlags flag,
  u32 direction,
  u08 a5,
  void *a6
) {
  char eventName[64];
  bin_ModuleBarn_CallFunction.Call<void>(
    pThis,
    name,
    flag,
    direction,
    a5,
    a6);

  if (flag & ModuleFlags_Game) {
    strncpy(eventName, smbMakeNamespace(""), sizeof(eventName));
    strncat(eventName, name, sizeof(eventName));
    HTCommEmitEvent(eventName, nullptr, nullptr);
  }
}
