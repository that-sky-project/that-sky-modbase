#ifndef __UTILS_SMBIOBJECT_HPP__
#define __UTILS_SMBIOBJECT_HPP__

#include <shared_mutex>
#include <mutex>
#include "utils/TypeAliases.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Modbase/Utils/SmbiObject
// ----------------------------------------------------------------------------

// Object prototype.
class SmbiObject {
public:
  SmbiObject() = default;
  ~SmbiObject() = default;

  // Lifecycle functions.
  void Initialize() { }
  void Terminate() { }

  // Mod status functions.
  void Enable() { }
  void Disable() { }

  // Runtime functions.
  void Update() { }
  void RenderFlush() { }
};

// Object prototype with shared mutex.
class SmbiMutexObject: public SmbiObject {
  using ObjLockW = std::shared_lock<std::shared_mutex>;
  using ObjLockRW = std::unique_lock<std::shared_mutex>;

public:
  SmbiMutexObject() = default;
  ~SmbiMutexObject() = default;

  // Lifecycle functions.
  void Initialize() { }
  void Terminate() { }

  // Mod status functions.
  void Enable() { }
  void Disable() { }

  // Runtime functions.
  void Update() { }
  void RenderFlush() { }

  // Mutex functions.
  //
  // The mutex should be handled by the class itself. It's not recommended to
  // control the mutex by outside.

  // Acquire read access, i.e., prevent external writes.
  ObjLockW LockW() const { return ObjLockW{m_mutex}; }
  // Acquire read-write access, i.e., exclusively lock the object.
  ObjLockRW LockRW() { return ObjLockRW{m_mutex}; }

protected:
  mutable std::shared_mutex m_mutex;
};

#endif
