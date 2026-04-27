#include "ModInternal.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Api/Memory/StlAllocator/exports
// ----------------------------------------------------------------------------

SMB_API_ATTR LPVOID SMB_API Sky_NewImpl(
  SIZE_T size
) {
  return operator new(size, std::nothrow);
}

SMB_API_ATTR VOID SMB_API Sky_DeleteImpl(
  LPVOID size
) {
  operator delete(size, std::nothrow);
}
