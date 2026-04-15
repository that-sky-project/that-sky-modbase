#ifndef __MOD_SMBIMODINITIALIZER_H__
#define __MOD_SMBIMODINITIALIZER_H__

#include <Windows.h>
#include "htskymodbase.h"

// ----------------------------------------------------------------------------
// [SECTION] Modbase/Setup
// ----------------------------------------------------------------------------

// Initializer class.
class SmbiModInitializer {
public:
  using PFN_DoInit = HTStatus (*)(
    HMODULE, const SmbiModInitializer *);

  static inline SmbiModInitializer *&list() {
    static SmbiModInitializer *p = nullptr;
    return p;
  }

  static HTStatus SetupAll(
    HMODULE hModuleDll);

  // Register a initializer.
  SmbiModInitializer(
    PFN_DoInit _function,
    const char *_name
  ) 
    : function(_function)
    , name(_name)
  {
    prev = SmbiModInitializer::list();
    SmbiModInitializer::list() = this;
  }

  ~SmbiModInitializer() = default;

  inline const SmbiModInitializer *GetPrev() const {
    return prev;
  }

  inline const char *GetName() const {
    return name;
  }

  HTStatus operator()(
    HMODULE hModuleDll
  ) const {
    if (function)
      return function(hModuleDll, this);
    else
      return HT_FAIL;
  }

private:
  SmbiModInitializer *prev;
  PFN_DoInit function;
  const char *name;

  void *operator new(size_t) = delete;
  static void* operator new[](size_t) = delete;
  void operator delete(void *) = delete;
  void operator delete[](void *) = delete;

  SmbiModInitializer(const SmbiModInitializer &) = delete;
  SmbiModInitializer &operator=(const SmbiModInitializer &) = delete;
};

#endif
