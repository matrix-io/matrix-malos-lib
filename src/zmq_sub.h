/*
 * Copyright 2016 <Admobilize>
 * MATRIX Labs  [http://creator.matrix.one]
 * This file is part of MATRIX Creator MALOS
 *
 * MATRIX Creator MALOS is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SRC_ZMQ_SUB_H_
#define SRC_ZMQ_SUB_H_

#include <memory>
#include <string>

#include "third_party/zmq.hpp"

namespace matrix_malos {

// This class is used to subscribe to ZMQ messages.
// A small wrapper to make code cleaner in upper layers.
class ZmqSubscriber {
public:
  ZmqSubscriber() {}

  enum { WAIT_FOREVER = -1 };

  // Initialize the subscriber and make it connect to a host/port.
  // The default topic "" means subscribe to all messages.
  bool Init(const std::string &host, int port, int high_water_mark,
            const std::string &topic = "", int n_threads = 1);

  // Wait for messages. Timeout in milliseconds.
  // Returns true if a message arrives.
  bool Poll(int timeout_ms);

  // Read messages when polling is successful.
  std::string Read();

private:
  std::unique_ptr<zmq::context_t> context_;
  std::unique_ptr<zmq::socket_t> socket_;
};

} // namespace matrix_malos

#endif // SRC_ZMQ_PULL_H_
