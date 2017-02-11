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

#include <tuple>
#include <functional>

#include "caf/exec_main.hpp"
#include "caf/exec_main.hpp"
#include "caf/actor_system.hpp"
#include "caf/actor_system_config.hpp"

#include "caf/io/middleman.hpp"

#include "fluent/node.hpp"
#include "fluent/table.hpp"
#include "fluent/context.hpp"
#include "fluent/channel.hpp"
#include "fluent/periodic.hpp"
#include "fluent/interface.hpp"
#include "fluent/algorithm.hpp"
#include "fluent/collection.hpp"
#include "fluent/state_type.hpp"
#include "fluent/application.hpp"

using namespace fluent;

namespace {

class reliable_delivery : public application<reliable_delivery> {
public:
  using super = application<reliable_delivery>;

  using st = state_type<
    channel<node, node, uint32_t, std::string>,         // 0: data_chan
    channel<node, node, uint32_t>,                      // 1: ack_chan
    table<node, node, uint32_t, std::string>,           // 2: send_buf
    periodic<10>,                                       // 3: timer
    input_interface<node, node, uint32_t, std::string>, // 4: pipe_in
    output_interface<node, node, uint32_t, std::string> // 5: pipe_sent
  >;

  st state;

  reliable_delivery(context& ctx) : super(ctx), state(ctx) {
    // get named references to our state members
    auto& data_chan = state.get<0>();
    auto& ack_chan = state.get<1>();
    auto& send_buf = state.get<2>();
    auto& timer = state.get<3>();
    auto& pipe_in = state.get<4>();
    auto& pipe_sent = state.get<5>();
    // declare application logic
    declare("send_packet") = [&] {
      send_buf += pipe_in;
      data_chan << pipe_in;
    };
    declare("timer_retry") = [&] {
      data_chan << take(timer.size(), send_buf);
    };
    declare("send_ack") = [&] {
      // drop last element from data_cahn
      ack_chan << map(data_chan, [](auto& x) {
        return std::make_tuple(std::get<0>(x), std::get<1>(x), std::get<2>(x));
      });
    };
    declare("recv_ack") = [&] {
      // get all idents from ack_chan
      auto idents = map(ack_chan, [](auto& x) { return std::get<2>(x); });
      // get all entries from send_buf that are matched by idents
      auto got_ack = filter(send_buf, [&](auto& x) {
        return idents.count(std::get<2>(x)) != 0;
      });
      pipe_sent += got_ack;
      send_buf -= got_ack;
    };
  }
};

struct config : caf::actor_system_config {
  std::string local_host;
  uint16_t local_port;
  std::string remote_host;
  uint16_t remote_port;
  config() {
   opt_group{custom_options_, "global"}
    .add(local_port, "port,p", "set port for this Bloom instance")
    .add(local_host, "host,H", "set hostname for this Bloom instance")
    .add(remote_port, "remote-port,r", "set port for remote Bloom instance")
    .add(remote_host, "remote-host,R", "set hostname for remote Bloom instance");
    add_message_type<node>("node");
  }
};

void caf_main(caf::actor_system& sys, const config& cfg) {
  context ctx{sys,
              {cfg.local_host, cfg.local_port},
              {{cfg.remote_host, cfg.remote_port}}};
  reliable_delivery rd{ctx};
  {
    // stuff we want to send
    std::vector<std::string> lines{"hello", "world", "over", "there"};
    if (cfg.local_port == 8080)
      lines = std::vector<std::string>{"UC Berkeley", "RISE", "lab"};
    else
      lines = std::vector<std::string>{"HAW", "Hamburg", "iNET", "group"};
    std::set<std::tuple<node, node, uint32_t, std::string>> out;
    for (uint32_t i = 0; i < lines.size(); ++i)
      out.emplace(std::make_tuple(ctx.remotes().front(),
                                  ctx.self(), i, lines[i]));
    auto& pipe_in = rd.state.get<4>();
    pipe_in += out;
  }
  rd.run();
}

} // namespace <anonymous>

CAF_MAIN(caf::io::middleman)
