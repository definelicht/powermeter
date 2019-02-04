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

#include "PowerMeter.h"
#include <chrono>
#include <iostream>

// This program outputs the total and supplied power to standard output for a
// given number of seconds, with a sampling interval of a given number of
// milliseconds.

int main(int argc, char **argv) {
  unsigned duration = 5;
  unsigned sampling_interval = 100;
  if (argc > 1) {
    duration = std::stoul(argv[1]);
  }
  if (argc > 2) {
    sampling_interval = std::stoul(argv[2]);
  }
  PowerMeter pm(sampling_interval, std::cout);
  std::cout << "Microseconds since epoch, drawn power, supplied power:\n"
            << std::flush;
  pm.Start();
  std::this_thread::sleep_for(std::chrono::seconds(duration));
  pm.Stop();
  return 0;
}
