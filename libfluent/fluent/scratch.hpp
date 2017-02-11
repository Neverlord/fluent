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

#ifndef FLUENT_SCRATCH_HPP
#define FLUENT_SCRATCH_HPP

#include "fluent/collection_base.hpp"

namespace fluent {

template <class... Ts>
class scratch : public collection_base<Ts...> {
public:
  using super = collection_base<Ts...>;

  using super::super;

  void tick_begin() override {
    this->xs_.clear();
  }

  void tick_end() override {
    // nop
  }

  // add contents of x for current timestep
  scratch& operator+=(const typename super::container& xs) {
    this->xs_.insert(xs.begin(), xs.end());
    return *this;
  }

  // add contents of x for current timestep
  scratch& operator+=(const super& x) {
    return (*this) += x.xs();
  }
};

} // namespace fluent

#endif // FLUENT_SCRATCH_HPP
