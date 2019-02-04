// Corsair PowerMeter
// Copyright (C) 2019 Johannes de Fine Licht
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <chrono>
#include <iostream>

extern "C" {
#include "logic/scan.h"
}

#include "PowerMeter.h"

PowerMeter::PowerMeter(unsigned sampling_interval_ms, std::ostream& output_file)
    : sampling_interval_ms_(sampling_interval_ms), output_file_(&output_file) {
  Init();
}

PowerMeter::PowerMeter(unsigned sampling_interval_ms)
    : sampling_interval_ms_(sampling_interval_ms), output_file_(nullptr) {
  Init();
}

void PowerMeter::Init() {
  int ret;

  // Initialize libusb
  ret = libusb_init(&context_);
  if (ret != 0) {
    throw std::runtime_error("Failed to initialize libusb.");
  }

  // Get devices
  int device_count = 0;
  ret = corsairlink_device_scanner(context_, &device_count);
  if (ret != 0) {
    throw std::runtime_error("Failed to retrieve Corsair devices.");
  }
  if (device_count < 1) {
    throw std::runtime_error(
        "No Corsair devices found. Did you remember to run with sudo "
        "privileges?");
  }

  // Detect PSU
  int psu_index = -1;
  for (int d = 0; d < device_count; ++d) {
    if (scanlist[d].device->driver == &corsairlink_driver_rmi) {
      if (psu_index != -1) {
        throw std::runtime_error("Multiple Corsair RMi PSUs not supported.");
      }
      psu_index = d;
    }
  }
  if (psu_index == -1) {
    throw std::runtime_error("No Corsair RMi PSU detected.");
  }

  // Get device handle to prepare for measurements
  device_ = scanlist[psu_index].device;
  handle_ = scanlist[psu_index].handle;
  ret = device_->driver->init(handle_, device_->write_endpoint);
  if (ret != 0) {
    throw std::runtime_error("Failed to initialize driver.");
  }
}

PowerMeter::~PowerMeter() {
  Stop();
  device_->driver->power.sensor_select(device_, handle_, 0);
  device_->driver->deinit(handle_, device_->write_endpoint);
  corsairlink_close(context_);
}

void PowerMeter::Start() {

  samples_ = decltype(samples_)(); // Reset sample vector
  running_ = true;

  future_ = std::async(std::launch::async, [this]() {
    std::chrono::high_resolution_clock timer;
    while (running_) {

      auto timestamp = timer.now();

      // Fetch wallpower draw
      double total_watts;
      device_->driver->power.total_wattage(device_, handle_, &total_watts);

      // Get supplied power from 3 power rails
      double supply_watts_sum = 0;
      for (int r = 0; r < 3; ++r) {
        double watts;
        device_->driver->power.sensor_select(device_, handle_, r);
        device_->driver->power.wattage(device_, handle_, r, &watts);
        supply_watts_sum += watts;
        // TODO: Multiplying voltage and current does not yield the same number
        //       as the power reported from the tool?
        // double volts, amperage;
        // device->driver->power.voltage(device, handle, r, &volts);
        // device->driver->power.amperage(device, handle, r, &amperage);
        // supply_watts_sum += volts * amperage;
      }

      // Log the measurement
      if (output_file_ != nullptr) {
        *output_file_ << timestamp.time_since_epoch().count() << ","
                      << total_watts << "," << supply_watts_sum << "\n";
      } else {
        samples_.emplace_back(timestamp, total_watts, supply_watts_sum);
      }

      // Sleep until next sample should be made
      std::this_thread::sleep_for(
          std::chrono::milliseconds(sampling_interval_ms_));

    }  // End sampling loop
  });  // End std::async lambda
}

void PowerMeter::Stop() {
  if (running_) {
    running_ = false;
    future_.wait();
  }
}

void PowerMeter::Reset() {
  Stop();
  samples_ = decltype(samples_)();
}

std::vector<std::tuple<PowerMeter::Time_t, double, double>>
PowerMeter::GetSamples() const {
  return samples_;  // Copy the internal sample buffer
}

void PowerMeter::PopSamples(
    std::vector<std::tuple<Time_t, double, double>>& samples) {
  samples = std::move(samples_);  // Move internal sample buffer to output
  samples_ = decltype(samples_)();
}
