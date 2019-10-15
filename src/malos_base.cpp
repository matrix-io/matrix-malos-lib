/*
 * Copyright 2016 <Admobilize>
 */

#include <chrono>
#include <iostream>
#include <thread>

#include "./malos_base.h"

namespace matrix_malos {

// How many threads for a given 0MQ context. Just one.
const int kOneThread = 1;

// Hight water mark. How many samples to queue (outgoing).
const int kSmallHighWaterMark = 20;

// Default delay when inactive.
const int kDefaultDelayWhenInactive = 100;

bool MalosBase::Init(int base_port, const std::string &bind_scope) {
  base_port_ = base_port;

  zmq_pull_config_.reset(new ZmqPuller());
  if (!zmq_pull_config_->Init(base_port, kOneThread, bind_scope)) {
    return false;
  }

  zmq_push_status_.reset(new ZmqPusher());
  if (!zmq_push_status_->Init(base_port + 2, kOneThread, kSmallHighWaterMark,
                             bind_scope)) {
    return false;
  }

  zqm_push_update_.reset(new ZmqPusher());
  if (!zqm_push_update_->Init(base_port + 3, kOneThread, kSmallHighWaterMark,
                              bind_scope)) {
    return false;
  }

  // Receive configurations.
  std::thread config_thread(&MalosBase::ConfigThread, this);
  config_thread.detach();
  // Receive pings.
  std::thread keepalive_thread(&MalosBase::KeepAliveThread, this, bind_scope,
                               base_port + 1);
  keepalive_thread.detach();
  // Send update to clients.
  std::thread update_thread(&MalosBase::UpdateThread, this);
  update_thread.detach();

  std::cerr << "Registered driver " << driver_name_ << " with port "
            << std::to_string(base_port) << "." << std::endl;

  return true;
}

void MalosBase::SendStatus(const pb::driver::Status::MessageType &type,
                const std::string &uuid, const std::string &message) {
  pb::driver::Status status;
  status.set_type(type);
  status.set_uuid(uuid);
  status.set_message(message);

  std::string buffer;
  status.SerializeToString(&buffer);
  zmq_push_status_->Send(buffer);
}


void MalosBase::ConfigThread() {
  // TODO: Fill out key/value pairs and make them readable by
  // derived classes.
  while (true) {
    if (zmq_pull_config_->Poll(ZmqPuller::WAIT_FOREVER)) {
      std::lock_guard<std::mutex> lock(config_mutex_);
      pb::driver::DriverConfig config;
      // Can we parse a configuration?
      if (!config.ParseFromString(zmq_pull_config_->Read())) {
        std::cerr << "Invalid configuration for " << driver_name_ << " driver."
                  << std::endl;
        SendStatus(pb::driver::Status::STATUS_ERROR, "",
                   "Invalid configuration for " + driver_name_ +
                       " driver. Could not parse protobuf.");

        has_been_configured_ = false;
        continue;
      }
      // Derived classes receive the config. For instance, in the case of the
      // Everloop driver there is data for the driver to read. In the case of
      // malos-eye,
      // the camera and the detectors need to be configured.
      if (!ProcessConfig(config)) {
        std::cerr << "Specific config for " << driver_name_ << " failed.";
        SendStatus(pb::driver::Status::STATUS_ERROR, "",
                   "Invalid specific configuration for " + driver_name_ +
                       " driver. Could not parse protobuf.");

        has_been_configured_ = false;
        continue;
      }

      // Shall we set a new delay between updates?
      if (config.delay_between_updates() > 0) {
        delay_between_updates_ =
            config.delay_between_updates() * static_cast<float>(1000);
        std::cerr << "New delay between updates for " << driver_name_ << " is "
                  << delay_between_updates_ << " ms." << std::endl;
      }

      // Shall we set a timeout after the last ping?
      if (config.timeout_after_last_ping() > 0) {
        timeout_after_last_ping_ =
            config.timeout_after_last_ping() * static_cast<float>(1000);
        std::cerr << "New timeout after last ping for " << driver_name_ << " "
                  << timeout_after_last_ping_ << " ms." << std::endl;
      }
      has_been_configured_ = true;
    }
  }
}

void MalosBase::UpdateThread() {
  while (true) {
    config_mutex_.lock();
    // If the device needs mandatory configuration, do not send updates until a
    // valid configuration has been received.
    if ((mandatory_configuration_ && !has_been_configured_) || !is_active_) {
      // We know this is not the best way to do this.
      // The thread should start when the driver is active and end when it's
      // not.
      // This introduces latency for the driver to start once it becomes active.
      config_mutex_.unlock();
      std::this_thread::sleep_for(
          std::chrono::milliseconds(kDefaultDelayWhenInactive));
      continue;
    }
    if (!SendUpdate()) {
      SendStatus(pb::driver::Status::STATUS_ERROR, "",
                 "Could not send update for " + driver_name_ + " driver.");
    }
    config_mutex_.unlock();
    std::this_thread::sleep_for(
        std::chrono::milliseconds(delay_between_updates_));
  }
}

void MalosBase::KeepAliveThread(const std::string &bind_scope, int port) {
  zmq::context_t context(kOneThread);
  zmq::socket_t socket(context, ZMQ_REP);
  socket.bind("tcp://" + bind_scope + ":" + std::to_string(port));

  int last_set_timeout = timeout_after_last_ping_;
  socket.setsockopt(ZMQ_RCVTIMEO, timeout_after_last_ping_);

  while (true) {
    if (timeout_after_last_ping_ != last_set_timeout) {
      socket.setsockopt(ZMQ_RCVTIMEO, timeout_after_last_ping_);
      last_set_timeout = timeout_after_last_ping_;
    }

    zmq::message_t request;
    zmq::message_t reply(0);

    is_active_ = socket.recv(&request);  // ping
    if (is_active_) socket.send(reply);  // pong
  }
}

void MalosBase::FillOutDriverInfo(pb::driver::DriverInfo *driver_info) const {
  driver_info->set_driver_name(driver_name_);
  driver_info->set_base_port(base_port_);
  driver_info->set_provides_updates(provides_updates_);
  driver_info->set_delay_between_updates(delay_between_updates_);
  driver_info->set_needs_pings(needs_pings_);
  driver_info->set_timeout_after_last_ping(timeout_after_last_ping_);
  driver_info->set_notes_for_human(notes_for_human_);
}

}  // namespace matrix_malos
