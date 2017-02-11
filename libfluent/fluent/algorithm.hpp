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

#include <map>
#include <iterator>
#include <algorithm>
#include <type_traits>

#include "caf/optional.hpp"

#include "fluent/collection_base.hpp"

namespace fluent {

template <class... Ts, class Predicate>
auto map(const collection_base<Ts...>& x, Predicate f) {
  auto& xs = x.xs();
  std::set<decltype(f(std::declval<std::tuple<Ts...>&>()))> res;
  for (auto& x : xs)
    res.insert(f(x));
  return res;
}

template <class... Ts, class Predicate>
auto filter(const collection_base<Ts...>& x, Predicate f) {
  std::set<std::tuple<Ts...>> res;
  for (auto& x : x.xs())
    if (f(x))
      res.emplace(x);
  return res;
}

template <class... Ts>
auto take(size_t num, const collection_base<Ts...>& x) {
  std::set<std::tuple<Ts...>> res;
  auto& xs = x.xs();
  auto i = xs.begin();
  auto e = i;
  std::advance(e, std::min(num, xs.size()));
  for (; i != e; ++i)
    res.emplace(*i);
  return res;
}

} // namespace fluent

