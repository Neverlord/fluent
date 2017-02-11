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

#ifndef FLUENT_STATE_TYPE_HPP
#define FLUENT_STATE_TYPE_HPP

#include <tuple>
#include <vector>
#include <functional>

#include "fluent/collection.hpp"

namespace fluent {

template <class... Ts>
class state_type {
public:
  using tuple_type = std::tuple<Ts...>;

  using collection_ptr = std::unique_ptr<collection>;

  state_type(context& ctx) : ctx_(ctx) {
    xs_.reserve(sizeof...(Ts));
    init(std::make_unique<Ts>(ctx)...);
  }

  // first tick of a program does not trigger tick_begin
  void first_tick() {
    tick_();
  }

  void tick() {
    for (auto& ptr : xs_)
      ptr->tick_begin();
    tick_();
  }

  template <size_t N>
  auto& get() {
    using type = typename std::tuple_element<N, tuple_type>::type;
    return static_cast<type&>(*xs_[N]);
  }

  template <class F>
  void add_fun(F&& f) {
    fs_.emplace_back(std::forward<F>(f));
  }

private:
  void tick_() {
    auto& s = ctx_.collector();
    bool run = true;
    s->do_receive(
      [&](caf::put_atom, caf::message& msg) {
        auto consumed =
           std::any_of(xs_.begin(), xs_.end(),
                       [&](collection_ptr& cp) { return cp->remote_add(msg); });
        if (!consumed)
          std::cerr << "unconsumed remote message: " << caf::to_string(msg)
                    << std::endl;
      },
      caf::after(std::chrono::seconds(0)) >> [&] {
        run = false;
      }
    ).until([&] { return !run; });
    for (auto& f : fs_)
      f();
    for (auto& ptr : xs_)
      ptr->tick_end();
  }

  void init() {
    // end of recursion
  }

  template <class... Us>
  void init(collection_ptr&& ptr, Us&&... xs) {
    xs_.emplace_back(std::move(ptr));
    init(std::forward<Us>(xs)...);
  }

  context& ctx_;
  std::vector<std::unique_ptr<collection>> xs_;
  std::vector<std::function<void()>> fs_;
};

} // namespace fluent

#endif // FLUENT_STATE_TYPE_HPP
