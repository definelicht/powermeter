## Overview

This tool enables the readout of drawn power and supplied power from Corsair RMi-series power supplies through in a stopwatch-like fashion, using the [OpenCorsairLink](https://github.com/audiohacked/OpenCorsairLink) project as a driver for the USB interface.
The sampling rate is configurable, and results can be either stored in memory, or written directly to a file.

## Prerequisties

-  ``libusb-1.0`` (available through all major package managers)

## Building and installing

The project uses a standard CMake setup:
```bash
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=<installation directory>
make install
```

This will install the required header files, along with the `libpowermeter` static library.

To use the tool within an existing project, either use the provided CMake configuration, or simply include `<installation directory>/include` and link to `<installation directory>/lib/libpowermeter.a`.

## Usage

See `examples/Example.cpp` for example usage.

The `PowerMeter` class provides stopwatch-like functionality for measuring power:

```cpp
PowerMeter pm(50);      // Write samples directly to standard output every 50 milliseconds
pm.Start();             // Start sampling
// ...do interesting work...
auto power = pm.Stop(); // Returns a vector<std::pair<double, double>>, where each entry
                        // contains the drawn power and supplied power.
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

## Bugs and suggestions

Please direct bugs and suggestions for improvements to the issue tracker.
