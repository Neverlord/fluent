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

#ifndef FLUENT_COLLECTION_BASE_HPP
#define FLUENT_COLLECTION_BASE_HPP

#include <set>
#include <tuple>

#include "fluent/type_list.hpp"
#include "fluent/collection.hpp"

namespace fluent {

template <class... Ts>
class collection_base : public collection {
public:
  using value_type = std::tuple<Ts...>;

  using container = std::set<value_type>;

  collection_base(context& ctx) : collection(ctx) {
    // nop
  }

  const container& xs() const {
    return xs_;
  }

  size_t size() const {
    return xs_.size();
  }

protected:
  container xs_;
};

} // namespace fluent

#endif // FLUENT_COLLECTION_BASE_HPP
