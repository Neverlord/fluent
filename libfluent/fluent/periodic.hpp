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

#ifndef FLUENT_PERIODIC_HPP
#define FLUENT_PERIODIC_HPP

#include <cstdint>

#include "caf/atom.hpp"

#include "fluent/collection_base.hpp"

namespace fluent {

using timestamp = int32_t;

template <size_t Seconds>
class periodic : public collection_base<timestamp> {
public:
  using super = collection_base<timestamp>;

  using super::super;

  void tick_begin() override {
    // nop
  }

  void tick_end() override {
    // nop
  }
};

} // namespace fluent

#endif // FLUENT_PERIODIC_HPP

