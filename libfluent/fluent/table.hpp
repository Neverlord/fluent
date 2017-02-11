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

#ifndef FLUENT_TABLE_HPP
#define FLUENT_TABLE_HPP

#include "fluent/collection_base.hpp"

namespace fluent {

template <class... Ts>
class table : public collection_base<Ts...> {
public:
  using super = collection_base<Ts...>;

  using super::super;

  void tick_begin() override {
    // nop
  }

  void tick_end() override {
    // clear everything that only appears at this timestamp
    for (auto& k : dirty_)
      this->xs_.erase(k);
    dirty_.clear();
  }

  // remove xs before next tick
  table& operator-=(const typename super::container& xs) {
    for (auto& x : xs)
      dirty_.emplace_back(x);
    return *this;
  }

  table& operator-=(const super& x) {
    return (*this) -= x.xs();
  }

  // add contents of x for current timestep for this time step only
  table& operator+=(const typename super::container& xs) {
    this->xs_.insert(xs.begin(), xs.end());
    return *this -= xs;
  }

  table& operator+=(const super& x) {
    return (*this) += x.xs();
  }


private:
  // caches elements marked for deletion, should really use iterators or
  // references instead of copying all dirty elements
  std::vector<std::tuple<Ts...>> dirty_;
};

} // namespace fluent

#endif // FLUENT_TABLE_HPP

