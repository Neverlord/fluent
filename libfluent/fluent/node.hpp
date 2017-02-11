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

#ifndef FLUENT_NODE_HPP
#define FLUENT_NODE_HPP

#include <vector>
#include <string>
#include <cstdint>

#include "caf/meta/type_name.hpp"

namespace fluent {

struct node {
  std::string host;
  uint16_t port;
};

template <class Inspector>
auto inspect(Inspector& f, node& x) {
  return f(caf::meta::type_name("node"), x.host, x.port);
}

inline bool operator==(const node& x, const node& y) {
  return x.host == y.host && x.port == y.port;
}

inline bool operator!=(const node& x, const node& y) {
  return !(x == y);
}

inline bool operator<(const node& x, const node& y) {
  return x.host < y.host && x.port < y.port;
}

} // namespace fluent

#endif // FLUENT_NODE_HPP

