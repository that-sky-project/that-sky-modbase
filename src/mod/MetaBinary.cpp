#include <includes/htmodloader.h>
#include "ModInternal.hpp"
#include "mod/MetaBinary.hpp"
#include "mod/SmbiBinaryDescriptor.hpp"

// ----------------------------------------------------------------------------
// [SECTION] Modbase/MetaBinaryObject
// ----------------------------------------------------------------------------

bool MetaBinaryFunction::Hook() {
  if (!IsValid() || !m_sfn.detour || m_hookEnabled)
    return false;

  if (HTAsmHookCreate(hModuleDll, &m_sfn) && HTAsmHookEnable(hModuleDll, m_sfn.fn))
    m_hookEnabled = true;

  return m_hookEnabled;
}

// ----------------------------------------------------------------------------
// [SECTION] Modbase/MetaBinaryObject
// ----------------------------------------------------------------------------

void MetaBinarySystem::Initialize() {
  for (auto *p = MetaObject<MetaBinaryFunction>::m_List(); p; p = p->m_prev)
    m_functions[p->m_name] = p;

  for (auto *p = MetaObject<MetaBinaryOffset>::m_List(); p; p = p->m_prev)
    m_variables[p->m_name] = p;

  for (auto *p = MetaObject<MetaBinaryVirtualFunction>::m_List(); p; p = p->m_prev)
    m_virtuals[p->m_name] = p;

  for (auto *p = MetaObject<MetaBinaryMemberOffset>::m_List(); p; p = p->m_prev)
    m_members[p->m_name] = p;
}

void MetaBinarySystem::Enable(
  const SmbiBinaryDescriptorBarn *barn
) {
  for (auto &it: m_functions)
    barn->Apply(it.second);

  for (auto &it: m_variables)
    barn->Apply(it.second);

  for (auto &it: m_virtuals)
    barn->Apply(it.second);

  for (auto &it: m_members)
    barn->Apply(it.second);
}
