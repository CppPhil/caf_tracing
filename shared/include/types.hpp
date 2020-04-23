#pragma once
#include <string>
#include <vector>

#include <caf/all.hpp>

#include "span_context.hpp"

CAF_BEGIN_TYPE_ID_BLOCK(caf_tracing, caf::first_custom_type_id)
  CAF_ADD_ATOM(caf_tracing, shared, local_chat_atom)
  CAF_ADD_ATOM(caf_tracing, shared, ls_query_atom)
  CAF_ADD_ATOM(caf_tracing, shared, chat_atom)
  CAF_ADD_ATOM(caf_tracing, shared, ls_atom)

  CAF_ADD_TYPE_ID(caf_tracing, (std::vector<std::string>) )
CAF_END_TYPE_ID_BLOCK(caf_tracing)

namespace shared {
using client_actor_type
  = caf::typed_actor<caf::reacts_to<caf::join_atom, std::string>,
                     caf::reacts_to<ls_atom>,
                     caf::reacts_to<caf::leave_atom, std::string>,
                     caf::reacts_to<local_chat_atom, std::string>,
                     caf::reacts_to<chat_atom, std::string>,
                     caf::replies_to<ls_query_atom, std::string>::with<bool>>;
} // namespace shared

CAF_BEGIN_TYPE_ID_BLOCK(caf_tracing_ctd, caf::id_block::caf_tracing::end)
  CAF_ADD_TYPE_ID(caf_tracing, (shared::client_actor_type))
CAF_END_TYPE_ID_BLOCK(caf_tracing_ctd)
