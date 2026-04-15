#include "ModInternal.hpp"
#include "mod/SmbiModInitializer.hpp"

HTStatus SmbiModInitializer::setupAll(
  HMODULE hModuleDll
) {
  HTStatus s = HT_SUCCESS;

  for (
    const SmbiModInitializer *p = SmbiModInitializer::list();
    p;
    p = p->getPrev()
  ) {
    if (!(*p)(hModuleDll))
      s = HT_FAIL;
  }

  return s;
}
