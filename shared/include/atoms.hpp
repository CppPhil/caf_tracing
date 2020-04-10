#pragma once
#include "caf/all.hpp"

namespace shared {
/// Atom to send chat messages stemming from the CLI to the client actor
/// so that it can relay them to the remote server actor.
using local_chat_atom = caf::atom_constant<caf::atom("localchat")>;

/// Atom used to communicate with the local client actor to request
/// it to check whether the local client is in the remote client list
/// of the server.
using ls_query_atom = caf::atom_constant<caf::atom("lsquery")>;

using chat_atom = caf::atom_constant<caf::atom("chat")>;
using ls_atom = caf::atom_constant<caf::atom("ls")>;
} // namespace shared
