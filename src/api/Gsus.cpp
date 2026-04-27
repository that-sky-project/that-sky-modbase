#include "utils/TypeAliases.hpp"
#include "mod/SmbiModInitializer.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Api/Gsus/init
// ----------------------------------------------------------------------------

// Shader uniform collections.
struct SkyGlobalShaderUniforms_ {
  void *unk_1;
  void *unk_2;
  SkyShaderUniform uniforms[160];
  u32 size;
};

typedef SkyGlobalShaderUniforms *(__fastcall *PFN_GlobalShaderUniforms_Singleton)(
  void);
typedef SkyShaderUniform *(__fastcall *PFN_GlobalShaderUniforms_m_FindUniform)(
  SkyGlobalShaderUniforms *, const char *);

static HTStatus fnInit_GlobalShaderUniforms(
  HMODULE, const SmbiModInitializer *);

static SmbiModInitializer gInit_Gsus{
  fnInit_GlobalShaderUniforms,
  "GlobalShaderUniforms::Singleton()"
};

static const HTAsmSig sigE8_GlobalShaderUniforms_Singleton = {
  "48 8D 15 ?  ?  ?  ?  48 89 C1 41 B9 ?  ?  ?  ?  "
  "E8 ?  ?  ?  ?  E8 ?  ?  ?  ?  41 B8 ",
  HT_SCAN_E8,
  0x15
};
static const HTAsmSig sigE8_GlobalShaderUniforms_m_FindUniform = {
  "EA 4C 89 F1 E8 ?  ?  ?  ?  48 85 C0 ",
  HT_SCAN_E8,
  0x04
};

static PFN_GlobalShaderUniforms_Singleton fn_GlobalShaderUniforms_Singleton = NULL;
static PFN_GlobalShaderUniforms_m_FindUniform fn_GlobalShaderUniforms_m_FindUniform = NULL;

static HTStatus fnInit_GlobalShaderUniforms(
  HMODULE hModuleDll,
  const SmbiModInitializer *self
) {
  (void)hModuleDll;
  (void)self;

  fn_GlobalShaderUniforms_m_FindUniform = (PFN_GlobalShaderUniforms_m_FindUniform)HTSigScan(
    &sigE8_GlobalShaderUniforms_m_FindUniform);
  fn_GlobalShaderUniforms_Singleton = (PFN_GlobalShaderUniforms_Singleton)HTSigScan(
    &sigE8_GlobalShaderUniforms_Singleton);

  if (
    !fn_GlobalShaderUniforms_m_FindUniform
    || !fn_GlobalShaderUniforms_Singleton
  )
    return HT_FAIL;

  return HT_SUCCESS;
}

// ----------------------------------------------------------------------------
// [SECTION] Api/Gsus/exports
// ----------------------------------------------------------------------------

SMB_API_ATTR SkyGlobalShaderUniforms *SMB_API smbSkyGlobalShaderUniforms() {
  return fn_GlobalShaderUniforms_Singleton();
}

SMB_API_ATTR SkyShaderUniform *SMB_API Sky_Gsu_FindUniform(
  SkyGlobalShaderUniforms *self,
  const char *name
) {
  if (!self || !name)
    // Invalid params.
    return nullptr;

  return fn_GlobalShaderUniforms_m_FindUniform(
    self,
    name);
}

SMB_API_ATTR UINT32 SMB_API SkyEx_Gsu_FindUniformFloat(
  SkyGlobalShaderUniforms *self,
  const char *name,
  float *buffer
) {
  if (!self || !name)
    // Invalid params.
    return 0;

  SkyShaderUniform *uniform = fn_GlobalShaderUniforms_m_FindUniform(
    self,
    name);

  if (!uniform || uniform->type != 0)
    // Not found.
    return 0;

  if (buffer)
    memcpy((void *)buffer, (void *)uniform->pValue, sizeof(f32) * uniform->size);

  // Returns array length.
  return uniform->size;
}
