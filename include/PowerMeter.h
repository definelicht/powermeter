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

#pragma once

#include <future>
#include <ostream>
#include <vector>

#include "libusb.h"
extern "C" {
#include "driver.h"
}

// The PowerMeter class provides a stopwatch-like interface to measure power
// consumption from a Corsair RMi-series power supply connected to the host
// machine via a USB interface.
//
// Example usage:
//   std::ofstream out_file("power_measurements.csv", "w");
//   PowerMeter pm(50, out_file);
//   pm.Start();
//   // ...do interesting work...
//   pm.Stop(); // Results will be present in power_measurements.csv
//
// When writing to a file, timestamps are stored according to
// time_since_epoch().count() of std::chrono::high_resolution_clock::time_point,
// which is typically reported in microseconds.

class PowerMeter {

 public:

  // Use the chrono time_point type to sample absolute timestamps. Can be
  // converted to number of microseconds since epoch.
  using Time_t = std::chrono::high_resolution_clock::time_point;

  /// Sample into an std::osteam (e.g., a file on disk, or std::cout), with an
  /// interval given by the first argument.
  PowerMeter(unsigned sampling_interval_ms, std::ostream &output_file);

  /// Sample into memory with an interval given by the argument.
  PowerMeter(unsigned sampling_interval_ms);

  ~PowerMeter();

  /// Start sampling power. If already running, the internal sample buffer is
  /// reset.
  void Start();

  /// Stop sampling power. Unless written to an std::ostream, results can be
  /// retrieved using the GetSamples() member function.
  void Stop();

  /// Retrieve a copy of the sampled power data.
  std::vector<std::tuple<Time_t, double, double>> GetSamples() const;

  /// Retrieve the sampled power data and delete the internal sample buffer. 
  void PopSamples(std::vector<std::tuple<Time_t, double, double>> &samples);

  /// Stop measuring and/or delete the internal sample buffer.
  void Reset();

 private:
  void Init();

  unsigned sampling_interval_ms_;
  std::ostream *output_file_;
  std::vector<std::tuple<Time_t, double, double>> samples_{};
  volatile bool running_{false};
  std::future<void> future_{};

  // Driver-related fields
  libusb_context *context_{nullptr};
  struct corsair_device_info *device_{nullptr};
  struct libusb_device_handle *handle_{nullptr};
};
