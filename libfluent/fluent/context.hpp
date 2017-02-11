/******************************************************************************
 *                        __ _                  _                             *
 *                       / _| |                | |                            *
 *                      | |_| |_   _  ___ _ __ | |_                           *
 *                      |  _| | | | |/ _ \ '_ \| __|                          *
 *                      | | | | |_| |  __/ | | | |_                           *
 *                      |_| |_|\__,_|\___|_| |_|\__|                          *
 *                                                                            *
 * Copyright (C) 2017                                                         *
 * Dominik Charousset <dominik.charousset (at) haw-hamburg.de>                *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License.    *
 * See accompanying file LICENSE.                                             *
 *                                                                            *
 * If you did not receive a copy of the license files, see                    *
 * http://opensource.org/licenses/BSD-3-Clause.                               *
 ******************************************************************************/

#ifndef FLUENT_CONTEXT_HPP
#define FLUENT_CONTEXT_HPP

#include <vector>
#include <string>
#include <cstdint>

#include "caf/actor_system.hpp"
#include "caf/scoped_actor.hpp"

#include "fluent/node.hpp"

namespace fluent {

class context {
public:
  context(caf::actor_system &sys, node this_node,
          std::vector<node> remote_nodes);

  caf::actor_system& sys() {
    return sys_;
  }

  const node& self() const {
    return this_node_;
  }

  const std::vector<node>& remotes() const {
    return remote_nodes_;
  }

  caf::actor& server() {
    return server_;
  }

  caf::scoped_actor& collector() {
    return collector_;
  }

private:
  caf::actor_system& sys_;
  caf::scoped_actor collector_;
  caf::actor server_;
  const node this_node_;
  const std::vector<node> remote_nodes_;
};

} // namespace fluent

#endif // FLUENT_CONTEXT_HPP
