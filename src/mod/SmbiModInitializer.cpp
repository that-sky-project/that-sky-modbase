#include "ModInternal.hpp"
#include "mod/SmbiModInitializer.hpp"

HTStatus SmbiModInitializer::SetupAll(
  HMODULE hModuleDll
) {
  using PCInit = const SmbiModInitializer *;
  HTStatus s = HT_SUCCESS;

  for (PCInit p = SmbiModInitializer::list(); p; p = p->GetPrev()) {
    if (!(*p)(hModuleDll))
      s = HT_FAIL;
  }

  return s;
}
