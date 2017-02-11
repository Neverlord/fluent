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

#ifndef FLUENT_CHANNEL_HPP
#define FLUENT_CHANNEL_HPP

#include <tuple>
#include <string>
#include <type_traits>

#include "caf/message.hpp"

#include "caf/detail/int_list.hpp"
#include "caf/detail/apply_args.hpp"

#include "fluent/scratch.hpp"
#include "fluent/type_list.hpp"

namespace fluent {

class channel_send_helper {
public:
  inline channel_send_helper(caf::actor& d, const node& n) : d_(d), n_(n) {
    // nop
  }

  template <class... Ts>
  void operator()(const Ts&... xs) {
    anon_send(d_, n_, caf::make_message(xs...));
  }

private:
  caf::actor& d_;
  const node& n_;
};

template <class... Ts>
class channel : public scratch<Ts...> {
public:
  using super = scratch<Ts...>;

  using super::super;

  void tick_begin() override {
    this->xs_.clear();
  }

  void tick_end() override {
    if (!this->xs_.empty())
      std::cout << "output interface tick end: "
                << caf::deep_to_string(this->xs_) << std::endl;
  }

  bool remote_add(caf::message& x) override {
    bool res = false;
    x.apply([&](Ts&... xs) {
      this->xs_.insert(std::make_tuple(std::move(xs)...));
      res = true;
    });
    return res;
  }

  channel& operator<<(const typename super::container& xs) {
    auto& ctx = this->ctx();
    auto& s = ctx.server();
    for (auto& x : xs) {
      auto& dest = std::get<0>(x);
      if (dest != ctx.self()) {
        channel_send_helper h{s, dest};
        caf::detail::apply_args(h, caf::detail::get_indices(x), x);
      }
    }
    return *this;
  }

  channel& operator<<(const typename super::super& x) {
    return (*this) << x.xs();
  }
};

} // namespace fluent

#endif // FLUENT_CHANNEL_HPP
