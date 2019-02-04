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

class PowerMeter {

public:

 /// Sample into a file on disk
 PowerMeter(unsigned sampling_interval_ms, std::ostream &output_file);

 /// Sample into memory
 PowerMeter(unsigned sampling_interval_ms);

 ~PowerMeter();

 void Start();

 std::vector<std::pair<double, double>> const &Stop();

 inline std::vector<std::pair<double, double>> const &samples() const {
   return samples_;
 }

private:
 void Init();

 unsigned sampling_interval_ms_;
 std::ostream *output_file_;
 std::vector<std::pair<double, double>> samples_{};
 volatile bool running_{false};
 std::future<void> future_{};

 // Driver-related fields
 libusb_context *context_{nullptr};
 struct corsair_device_info *device_{nullptr};
 struct libusb_device_handle *handle_{nullptr};
};
