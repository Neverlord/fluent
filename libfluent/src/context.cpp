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

#include "fluent/context.hpp"

#include "caf/all.hpp"
#include "caf/io/all.hpp"

namespace fluent {

namespace {

using namespace caf;

struct state {
  // keeps track of connected Bloom instances
  std::map<node, actor> remotes;

  // stores outgoing messages until a connection is available
  std::map<node, std::vector<message>> bufs;

  event_based_actor* self;

  state(event_based_actor* selfptr) : self(selfptr) {
    // nop
  }
};

behavior server_impl(stateful_actor<state>* self, actor collector) {
  return {
    [=](connect_atom, const node& n) {
      auto mm = self->system().middleman().actor_handle();
      self->request(mm, infinite, connect_atom::value, n.host, n.port).then(
        [=](node_id, strong_actor_ptr ptr, std::set<std::string>) {
          std::cout << "connecting to " << caf::to_string(n)
                    << " successful" << std::endl;
          auto hdl = actor_cast<actor>(ptr);
          self->state.remotes[n] = hdl;
          auto& buf = self->state.bufs[n];
          for (auto& msg : buf)
            self->send(hdl, put_atom::value, std::move(msg));
          buf.clear();
        },
        [=](const error&) {
          std::cout << "could not connec to " << caf::to_string(n)
                    << "... retry in 1s " << std::endl;
          // try again in 1s
          self->delayed_send(self, std::chrono::seconds(1),
                             connect_atom::value, n);
        }
      );
    },
    // messages from channels
    [=](const node& n, caf::message& msg) {
      std::cout << caf::to_string(msg) << std::endl;
      auto& st = self->state;
      auto i = st.remotes.find(n);
      if (i == st.remotes.end()) {
        // not in list means we see this node for the first time: connect
        self->send(self, connect_atom::value, n);
        st.remotes.emplace(n, actor{});
        st.bufs[n].emplace_back(std::move(msg));
      } else if (!i->second) {
        // default constructed handle means we have not connected yet, but
        // establishing a connection is pending
        st.bufs[n].emplace_back(std::move(msg));
      } else {
        // send immediately if connection is available
        self->send(i->second, put_atom::value, std::move(msg));
      }
    },
    // messages from remotes
    [=](put_atom atm, caf::message& msg) {
      self->send(collector, atm, std::move(msg));
    }
  };
}

} // namespace <anonymous>

context::context(caf::actor_system& sys, node this_node,
                 std::vector<node> remote_nodes)
    : sys_(sys),
      collector_(sys_),
      this_node_(std::move(this_node)),
      remote_nodes_(std::move(remote_nodes)) {
  server_ = sys_.spawn(server_impl, collector_);
  sys.middleman().publish(server_, this_node.port);
}

} // namespace fluent
