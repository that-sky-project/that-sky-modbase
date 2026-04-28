#include "ModInternal.hpp"
#include "mod/SmbiModInitializer.hpp"

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
  ModuleBarn *, const char *, ModuleFlags, u32, u08);

static HTStatus fnInit_LifeCycleHook(
  HMODULE,
  const SmbiModInitializer *);

void hook_ModuleBarn_CallFunction(
  ModuleBarn *pThis,
  const char *name,
  ModuleFlags flag,
  u32 direction,
  u08 a5);

// ----------------------------------------------------------------------------
// [SECTION] Api/LifeCycleHook/init
// ----------------------------------------------------------------------------

static const HTAsmSig sigE8_ModuleBarn_CallFunction = {
  "48 89 FA 41 B8 ?  ?  ?  ?  45 31 C9 E8 ?  ?  ?  "
  "?  80 3D",
  HT_SCAN_E8,
  0x0C
};

static HTAsmFunction sfn_ModuleBarn_CallFunction{
  "ModuleBarn::CallFunction()",
  nullptr,
  nullptr,
  nullptr
};

static SmbiModInitializer gInit_LifeCycleHook{
  fnInit_LifeCycleHook,
  "ModuleBarn::CallFunction()"
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

  sfn_ModuleBarn_CallFunction.detour = (void *)hook_ModuleBarn_CallFunction;

  return smbiCreateAndEnableHook(
    &sigE8_ModuleBarn_CallFunction,
    &sfn_ModuleBarn_CallFunction);
}

void hook_ModuleBarn_CallFunction(
  ModuleBarn *pThis,
  const char *name,
  ModuleFlags flag,
  u32 direction,
  u08 a5
) {
  char eventName[64];
  ((PFN_ModuleBarn_CallFunction)sfn_ModuleBarn_CallFunction.origin)(
    pThis,
    name,
    flag,
    direction,
    a5);

  if (flag & ModuleFlags_Game) {
    strncpy(eventName, smbMakeNamespace(""), sizeof(eventName));
    strncat(eventName, name, sizeof(eventName));
    HTCommEmitEvent(eventName, nullptr, nullptr);
  }
}
