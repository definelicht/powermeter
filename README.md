## Overview

This tool enables the readout of drawn power and supplied power from [Corsair RMi-series](https://www.corsair.com/us/en/Power/Plug-Type/rmi-series-config/p/CP-9020082-NA) power supplies in a stopwatch-like fashion, using the [OpenCorsairLink](https://github.com/audiohacked/OpenCorsairLink) project as a driver for the USB interface.
The sampling rate is configurable, and results can be either stored in memory, or written directly to a file.

## Prerequisties

-  ``libusb-1.0`` (available through all major package managers)

## Building and installing

_Make sure to clone with `--recursive`, or pull submodules with `git submodule update --init`_ to fetch the [OpenCorsairLink](https://github.com/audiohacked/OpenCorsairLink) dependency.

The project uses a standard CMake setup:
```bash
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=<installation directory>
make install
```

This will install the required header files, along with the `libpowermeter` static library.

To use the tool within an existing project, either use the provided CMake configuration, or simply include `<installation directory>/include/powermeter` and link to `<installation directory>/lib/libpowermeter.a`.

## Usage

See `examples/Example.cpp` for example usage.

**Note that all executables must be run with `sudo` privileges to interact with the USB interface.**

The `PowerMeter` class provides stopwatch-like functionality for measuring power:

```cpp
PowerMeter pm(50);          // Records samples at an interval of 50 milliseconds
pm.Start();                 // Start sampling
// ...do interesting work...
pm.Stop();                  // Stop sampling
auto res = pm.PopSamples(); // Returns a vector<std::tuple<Time_t, double, double>>, where each entry
                            // contains the timestamp, drawn power, and supplied power, and Time_t is
                            // the type std::chrono::high_resolution_clock::time_point
```

As an optional second argument, `PowerMeter` takes an `std::ostream` where input will be written instead of storing it internally. This can be used to write results directly to a file:

```cpp
std::ofstream out_file("power_measurements.csv", "w");
PowerMeter pm(50, out_file);
pm.Start();
// ...do interesting work...
pm.Stop(); // Results will be present in power_measurements.csv
```

Or, directly to standard output:

```cpp
PowerMeter pm(50, std::cout); // Samples are written directly to the terminal/standard out
```

The timestamps are returned as `std::chrono::high_resolution_clock::time_point`, and can be converted to microseconds since epoch by using:

```cpp
sample.time_since_epoch().count()
```

## Running

The included example executable prints power measurements to standard output for a given time, with a given interval:
```bash
sudo ./Example 5 50
```
This will print samples every 50 milliseconds for 5 seconds.

## Bugs and suggestions

Please direct bugs and suggestions for improvements to the issue tracker.
