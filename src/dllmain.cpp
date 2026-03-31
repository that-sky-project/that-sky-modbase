#include <windows.h>
#include "htskymodbase.h"

HMODULE hModuleDll = NULL;

BOOL APIENTRY DllMain(
  HMODULE hModule,
  DWORD dwReason,
  LPVOID lpReserved
) {
  if (dwReason == DLL_PROCESS_ATTACH)
    hModuleDll = hModule;
  return TRUE;
}
