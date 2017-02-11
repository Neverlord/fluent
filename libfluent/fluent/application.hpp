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
#include <string>
#include <type_traits>

#include "fluent/context.hpp"

namespace fluent {

template <class T>
class fun_adder {
public:
  fun_adder(T& x) : x_(x) {
    // nop
  }

  fun_adder(fun_adder&&) = default;

  template <class F>
  void operator=(F&& f) {
    x_.add_fun(std::forward<F>(f));
  }

private:
  T& x_;
};

template <class Derived>
class application {
public:
  application(context& ctx) : ctx_(ctx) {
    // nop
  }

  void run() {
    dptr()->state.first_tick();
    for (;;)
      dptr()->state.tick();
  }

  context& ctx() {
    return ctx_;
  }

  auto declare(const char*) {
    return fun_adder<decltype(dptr()->state)>{dptr()->state};
  }

private:
  Derived* dptr() {
    return static_cast<Derived*>(this);
  }

  context& ctx_;
};

} // namespace fluent

