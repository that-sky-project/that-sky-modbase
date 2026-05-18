#ifndef __MOD_METABINARY_HPP__
#define __MOD_METABINARY_HPP__

#include <utility>
#include <type_traits>
#include <Meta.hpp>

#include "ModInternal.hpp"
#include "utils/TypeAliases.hpp"
#include "utils/SmbiObject.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Modbase/MetaBinaryObject
// ----------------------------------------------------------------------------

static constexpr intptr_t kInvalidOffset = -1;

// NOTE: All the objects under Modbase/MetaBinaryObject is single-threaded.
// Modifying objects in a multi-threaded environment may cause issues.

// Function address.
class MetaBinaryFunction: public MetaObject<MetaBinaryFunction> {
public:
  // Use "<object_name>.<member_name>()", "<object_name>.<member_name>()" or
  // "<function_name>()", e.g. "Game.Initialize()" or "MetaBinaryOffset::IsValid()".
  MetaBinaryFunction(
    cstring name,
    void *detour = nullptr
  )
    : MetaObject<MetaBinaryFunction>(name)
  {
    m_prev = MetaObject<MetaBinaryFunction>::m_List();
    MetaObject<MetaBinaryFunction>::m_List() = this;
    m_sfn.name = name;
    m_sfn.detour = detour;
  }

  inline bool IsValid() {
    return !!m_sfn.fn;
  }

  inline bool Initialize(void *address) {
    if (IsValid())
      return false;
    m_sfn.fn = address;
    return true;
  }

  // Create hook.
  bool Hook();

  // Calling a function; if the hook is enabled, the call will be automatically
  // forwarded to the trampoline function.
  template<typename R, typename ...Args>
  inline R Call(Args &&...args) {
    // This call only supports pass-by-value.
    using PFN_Call = R (*)(std::remove_reference_t<Args>...);

    HTAssertMsg(IsValid(), "Try to call uninitialized function %s", m_name);

    // Always call the original function.
    // TODO: Maybe we need a mutex.
    PFN_Call fn = m_hookEnabled ? (PFN_Call)m_sfn.origin : (PFN_Call)m_sfn.fn;

    return fn(args...);
  }

private:

  HTAsmFunction m_sfn = { nullptr, nullptr, nullptr, nullptr };
  bool m_hookEnabled = false;
};

// Global variable address.
class MetaBinaryOffset: public MetaObject<MetaBinaryOffset> {
public:
  // Use "<variable_name>".
  MetaBinaryOffset(
    cstring name
  )
    : MetaObject<MetaBinaryOffset>(name)
  {
    m_prev = MetaObject<MetaBinaryOffset>::m_List();
    MetaObject<MetaBinaryOffset>::m_List() = this;
  }

  inline bool Initialize(void *address) {
    if (IsValid())
      return false;
    m_address = address;
    return true;
  }

  inline bool IsValid() {
    return !!m_address;
  }

  template<typename Ty>
  inline Ty *ResolvePtr() {
    return (Ty *)m_address;
  }

  template<typename Ty>
  inline Ty &ResolveRef() {
    return *ResolvePtr<Ty>();
  }

private:
  void *m_address = nullptr;
};

// Virtual function (vftable offset).
class MetaBinaryVirtualFunction: public MetaObject<MetaBinaryVirtualFunction> {
public:
  // Use "<object_name>.<member_name>()" or "<object_name>.<member_name>()",
  // e.g. "Game.Initialize()" or "MetaBinaryOffset::IsValid()".
  MetaBinaryVirtualFunction(
    cstring name,
    void *detour = nullptr
  )
    : MetaObject<MetaBinaryVirtualFunction>(name)
  {
    m_prev = MetaObject<MetaBinaryVirtualFunction>::m_List();
    MetaObject<MetaBinaryVirtualFunction>::m_List() = this;
    m_sfn.detour = detour;
    m_sfn.name = m_name;
  }

  inline bool Initialize(
    uintptr_t slot
  ) {
    if (IsValid())
      return false;
    m_slotIdx = slot;
    return true;
  }

  // Create and enable hook.
  template<typename Obj>
  inline bool Hook(Obj *obj) {
    if (!IsValid() || !m_sfn.detour || m_hookEnabled)
      return false;

    using PFN_Call = void *;
    HTAssertMsg(obj, "Try to hook %s on a NULL object", m_name);

    PFN_Call *vftable = *((PFN_Call **)obj);
    HTAssertMsg(vftable, "Try to hook %s on a NULL vftable", m_name);

    PFN_Call fn = (PFN_Call)vftable[m_slotIdx];
    HTAssertMsg(fn, "Try to hook %s on a NULL function", m_name);

    m_sfn.fn = fn;

    if (HTAsmHookCreate(hModuleDll, &m_sfn) && HTAsmHookEnable(hModuleDll, m_sfn.fn))
      m_hookEnabled = true;

    return m_hookEnabled;
  }

  inline bool IsValid() {
    return m_slotIdx != kInvalidOffset;
  }

  // Calling a function; if the hook is enabled, the call will be automatically
  // forwarded to the trampoline function.
  template<typename Ret, typename Obj, typename ...Args>
  inline Ret Call(Obj *obj, Args &&...args) {
    // This call only supports pass-by-value.
    using PFN_Type = Ret (*)(Obj *, std::remove_reference_t<Args>...);
    HTAssertMsg(obj, "Try to call %s on a NULL object", m_name);

    PFN_Type *vftable = *((PFN_Type **)obj);
    HTAssertMsg(vftable, "Try to call %s on a NULL vftable", m_name);

    PFN_Type fn = (PFN_Type)vftable[m_slotIdx];
    HTAssertMsg(fn, "Try to call %s on a NULL function", m_name);

    if (fn == (PFN_Type)m_sfn.fn && m_hookEnabled)
      return ((PFN_Type)m_sfn.origin)(obj, args...);
    else
      return fn(obj, args...);
  }

private:
  uintptr_t m_slotIdx = kInvalidOffset;
  HTAsmFunction m_sfn = { nullptr, nullptr, nullptr, nullptr };
  bool m_hookEnabled = false;
};

// Member variable.
class MetaBinaryMemberOffset: public MetaObject<MetaBinaryMemberOffset> {
public:
  // Use "<object_name>.<member_name>" or "<object_name>.<member_name>",
  // e.g. "Game.metaSystem" or "MetaBinaryOffset::m_offset".
  MetaBinaryMemberOffset(
    cstring name
  )
    : MetaObject<MetaBinaryMemberOffset>(name)
  {
    m_prev = MetaObject<MetaBinaryMemberOffset>::m_List();
    MetaObject<MetaBinaryMemberOffset>::m_List() = this;
  }

  inline bool Initialize(
    uintptr_t offset
  ) {
    if (IsValid())
      return false;
    m_offset = offset;
    return true;
  }

  template<typename Tr, typename Ts>
  inline Tr *ResolvePtr(Ts *src) {
    HTAssertMsg(src, "Try to resolve %s by NULL", m_name);

    if (!IsValid())
      return nullptr;

    return (Tr *)((char *)src + m_offset);
  }

  template<typename Tr, typename Ts>
  inline Tr &ResolveRef(Ts *src) {
    return *ResolvePtr<Tr, Ts>(src);
  }

  inline bool IsValid() {
    return m_offset != kInvalidOffset;
  }

private:
  uintptr_t m_offset = kInvalidOffset;
};

// ----------------------------------------------------------------------------
// [SECTION] Modbase/MetaBinarySystem
// ----------------------------------------------------------------------------

class SmbiBinaryDescriptorBarn;

class MetaBinarySystem: public SmbiObject {
public:
  MetaBinarySystem() = default;
  ~MetaBinarySystem() = default;

  void Initialize();

  void Enable(const SmbiBinaryDescriptorBarn *barn);

private:
  MetaStrHashMap<MetaBinaryFunction *> m_functions;
  MetaStrHashMap<MetaBinaryOffset *> m_variables;
  MetaStrHashMap<MetaBinaryVirtualFunction *> m_virtuals;
  MetaStrHashMap<MetaBinaryMemberOffset *> m_members;
};

#endif
