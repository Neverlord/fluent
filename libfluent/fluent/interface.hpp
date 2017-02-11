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

#ifndef FLUENT_INTERFACE_HPP
#define FLUENT_INTERFACE_HPP

#include <vector>
#include <string>
#include <cstdint>

#include "caf/actor_system.hpp"

#include "fluent/scratch.hpp"
#include "fluent/type_list.hpp"

namespace fluent {

template <class... Ts>
class input_interface : public scratch<Ts...> {
public:
  using super = scratch<Ts...>;

  using super::super;

  void tick_end() override {
    if (!this->xs_.empty())
      std::cout << "input interface tick end: "
                << caf::deep_to_string(this->xs_) << std::endl;
  }
};

template <class... Ts>
class output_interface : public scratch<Ts...> {
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
};

} // namespace fluent

#endif // FLUENT_INTERFACE_HPP

