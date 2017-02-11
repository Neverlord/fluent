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

#ifndef FLUENT_COLLECTION_HPP
#define FLUENT_COLLECTION_HPP

#include "fluent/context.hpp"

#include "caf/fwd.hpp"

namespace fluent {

class collection {
public:
  collection(context& ctx);
  virtual ~collection();
  virtual void tick_begin() = 0;
  virtual void tick_end() = 0;
  // returns true if `msg` was consumed, only implemented by channels
  virtual bool remote_add(caf::message& msg);

  context& ctx() {
    return ctx_;
  }

private:
  context& ctx_;
};

} // namespace fluent

#endif // FLUENT_COLLECTION_HPP
