// ----------------------------------------------------------------------------
// HTSkyModBase
// - A Simple mod developed for exporting in-game funtions of Sky: CotL.
// <https://www.github.com/HTMonkeyG/HTSkyModBase>
//
// MIT License
//
// Copyright (c) 2025 HTMonkeyG
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// ----------------------------------------------------------------------------

#ifndef __HTSKYMODBASE_H__
#define __HTSKYMODBASE_H__

#include <windows.h>
#include "includes/htmodloader.h"

// ----------------------------------------------------------------------------
// [SECTION] PROGRAMMING_GUIDE
// ----------------------------------------------------------------------------
// Comments
//
// - There're lots of comments on functions and struct declarations.
// - See the comments to get the usage of them.
// ----------------------------------------------------------------------------
// Structs
//
// - Since multiple in-game structs, classes and objects have extremely differences
// between International and Chinese editions of the game, HTSkyModBase only
// provides a pointer or a handle to those structs.
//
// - You may call smbSky... functions to access the handle, and pass the handle
// to Sky... or SkyEx... functions to access the real in-game resources.
// HTSkyModbase will do the neccessary conversions to make your operations valid
// between all versions and editions of the game.
// ----------------------------------------------------------------------------
// Functions
//
// - The smb... or smbSky... functions are shortcuts to access HTSkyModBase 
// features or in-game resources.
//
// - The Sky... functions are encapsulations of in-game functions, both virtual
// functions and member functions. HTSkyModBase handles necessary conversions
// internally to ensure consistent behavior across different game versions.
//
// - The SkyEx... functions are extended functions or shortcuts to implement
// features not originally presented by the game.
// ----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------------
// [SECTION] Modbase/Basic
// ----------------------------------------------------------------------------

// API attributes.
#define SMB_API HTMLAPI

#ifndef SMB_API_ATTR
#define SMB_API_ATTR
#endif

// Namespace prefix.
#define smbNamespace "htskymodbase"
#define smbMakeNamespace(literal) smbNamespace ":" literal

// Event names.
// Call HTCommOnEvent() to listen to these events.
#define smbEventBuildScene        smbMakeNamespace("BuildScene")
#define smbEventInitialize        smbMakeNamespace("Initialize")
#define smbEventOnEvent           smbMakeNamespace("OnEvent")
#define smbEventOnLevelLoad       smbMakeNamespace("OnLevelLoad")
#define smbEventOnLevelLoadEarly  smbMakeNamespace("OnLevelLoadEarly")
#define smbEventOnLevelLoadLate   smbMakeNamespace("OnLevelLoadLate")
#define smbEventOnLevelUnload     smbMakeNamespace("OnLevelUnload")
#define smbEventOnLevelUnloadLate smbMakeNamespace("OnLevelUnloadLate")
#define smbEventOnPause           smbMakeNamespace("OnPause")
#define smbEventOnSuspend         smbMakeNamespace("OnSuspend")
#define smbEventOnUnpause         smbMakeNamespace("OnUnpause")
#define smbEventPostRender        smbMakeNamespace("PostRender")
#define smbEventRenderFlush       smbMakeNamespace("RenderFlush")
#define smbEventResize            smbMakeNamespace("Resize")
#define smbEventUpdate            smbMakeNamespace("Update")
#define smbEventUpdateBackground  smbMakeNamespace("UpdateBackground")
#define smbEventUpdateLate        smbMakeNamespace("UpdateLate")

// ----------------------------------------------------------------------------
// [SECTION] Sky/Game
// ----------------------------------------------------------------------------

typedef struct SkyGame_ SkyGame;


// Get the Game object pointer.
SMB_API_ATTR SkyGame *SMB_API smbSkyGame();

typedef SkyGame *(SMB_API PFN_smbSkyGame)();


// ----------------------------------------------------------------------------
// [SECTION] Sky/GlobalShaderUniforms
// ----------------------------------------------------------------------------

// Global shader uniforms, not exposed.
typedef struct SkyGlobalShaderUniforms_ SkyGlobalShaderUniforms;

// -> enum SkyShaderUniformType_;
// Shader uniform types of Sky.
typedef int SkyShaderUniformType;

// -> struct SkyShaderUniform_;
// Shader uniform data of Sky.
// Make sure you know what are you doing before modifying this.
typedef struct SkyShaderUniform_ SkyShaderUniform;

// -> SkyShaderUniformType;
enum SkyShaderUniformType_ {
  SkyShaderUniformType_Float = 0,
  SkyShaderUniformType_Uint = 3,
  SkyShaderUniformType_Texture = 4
};

// -> SkyShaderUniform;
struct SkyShaderUniform_ {
  // The name of the uniform. Starts with "u_", e.g. "u_zNear".
  char name[32];
  union {
    // The pointer to the value.
    void *pValue;
    // Unused.
    float valueFloat[4];
  };
  // The type of the uniform.
  SkyShaderUniformType type;
  // The size in elements.
  int size;
};


// [Smb] Get the global shader uniform singleton.
SMB_API_ATTR SkyGlobalShaderUniforms *SMB_API smbSkyGlobalShaderUniforms();

typedef SkyGlobalShaderUniforms *(SMB_API *PFN_smbSkyGlobalShaderUniforms)();


// [Sky] Get a uniform struct pointer from given name.
// You should know what you're doing before modifying the struct.
SMB_API_ATTR SkyShaderUniform *SMB_API Sky_Gsu_FindUniform(
  SkyGlobalShaderUniforms *self,
  const char *name);

typedef SkyShaderUniform *(SMB_API *PFN_SkyGsu_FindUniform)(
  SkyGlobalShaderUniforms *,
  const char *);


// [Extended] Get a uniform value storaged by a float array, e.g. vectors or
// matrices. Returns number of elements written, or needed array length when
// buffer == NULL.
SMB_API_ATTR UINT32 SMB_API SkyEx_Gsu_FindUniformFloat(
  SkyGlobalShaderUniforms *self,
  const char *name,
  float *buffer);

typedef UINT32 (SMB_API *PFN_SkyExGsu_FindUniformFloat)(
  SkyGlobalShaderUniforms *,
  const char *,
  float *);


// ----------------------------------------------------------------------------
// [SECTION] Sky/Resources
// ----------------------------------------------------------------------------

// - About the Resource API
//
// The resource API proxies the game's resource lookup function. This lookup
// function takes a resource alias and returns the bundle name and relative path
// of the resource (in the vanilla game, the resource alias is the same as the
// resource's relative path).
// 
// After being proxied by Modbase, the function first looks up the resource using
// the passed alias in the game, and only then in the mod. Therefore, it is not
// possible to directly replace game resources by registering a resource with the
// same name.

// [Extended] Register the resource file at the specified path into the game's
// resource manifest under the name `name`.
// 
// The resource must be located in the `<mod>/assets/` directory of the mod
// folder, `path` must be a path relative to `<mod>/assets/`. Any registration
// attempt that goes outside the directory will fail.
//
// During development, it is recommended to use the same resource directory
// structure as the game (see the game's `assets/data/initial` folder for details).
SMB_API_ATTR HTStatus SMB_API SkyEx_Resources_RegisterSingleEx(
  HMODULE hModuleDll,
  const char *name,
  const char *path);

typedef HTStatus (SMB_API *PFN_SkyEx_Resources_RegisterSingleEx)(
  HMODULE,
  const char *,
  const char *);



#ifdef __cplusplus
}
#endif

#endif
