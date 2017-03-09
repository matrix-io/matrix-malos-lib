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

#include "./zmq_sub.h"

namespace matrix_malos {

bool ZmqSubscriber::Init(const std::string &host, int port, int high_water_mark,
                         const std::string &topic, int n_threads) {
  // Get the context first.
  context_.reset(new zmq::context_t(n_threads));

  // Now get the socket.
  socket_.reset(new zmq::socket_t(*context_.get(), ZMQ_SUB));

  // Connect to the remote host.
  socket_->connect("tcp://" + host + ":" + std::to_string(port));

  // Set high water mark.
  socket_->setsockopt(ZMQ_SNDHWM, (void *)&high_water_mark,
                      sizeof(high_water_mark));

  // Topic we're interested in.
  socket_->setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.size());

  // Assume things went well.
  return true;
}

bool ZmqSubscriber::Poll(int timeout_ms) {
  // Polling a single socket.
  zmq::pollitem_t poll_items[] = {
      {static_cast<void *>(*socket_.get()), 0, ZMQ_POLLIN, 0},
  };
  // Wait for message.
  zmq::poll(&poll_items[0], 1, timeout_ms);
  // Returns true if a message arrives.
  return poll_items[0].revents & ZMQ_POLLIN;
}

std::string ZmqSubscriber::Read() {
  zmq::message_t message;
  // Read the message from the socket.
  socket_->recv(&message);
  // Convert to string and return.
  std::string ret;
  ret.assign(static_cast<char *>(message.data()), message.size());
  return ret;
}

} // namespace  matrix_malos
