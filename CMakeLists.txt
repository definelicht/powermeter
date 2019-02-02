# Corsair PowerMeter 
# Copyright (C) 2019 Johannes de Fine Licht 
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.0)
project(corsair_power)

# External dependencies 
find_package(Threads REQUIRED)
find_path(LIBUSB_INCLUDE_DIR
  NAMES libusb.h
  PATH_SUFFIXES "include" "libusb" "libusb-1.0")
find_library(LIBUSB_LIBRARY
  NAMES usb-1.0 usb
  PATH_SUFFIXES "lib" "lib32" "lib64")

# Build required OpenCorsairLink files
set(POWERMETER_OPENCORSAIRLINK_FILES
		OpenCorsairLink/lowlevel/rmi.c
		OpenCorsairLink/protocol/rmi/core.c
		OpenCorsairLink/protocol/rmi/time.c
		OpenCorsairLink/protocol/rmi/power.c
		OpenCorsairLink/protocol/rmi/temperature.c
		OpenCorsairLink/lowlevel/commanderpro.c
		OpenCorsairLink/protocol/commanderpro/core.c
		OpenCorsairLink/protocol/commanderpro/power.c
		OpenCorsairLink/protocol/commanderpro/fan.c
		OpenCorsairLink/protocol/commanderpro/temperature.c
		OpenCorsairLink/lowlevel/asetek.c
		OpenCorsairLink/protocol/asetek/core.c
		OpenCorsairLink/protocol/asetek/fan.c
		OpenCorsairLink/protocol/asetek/led.c
		OpenCorsairLink/protocol/asetek/pump.c
		OpenCorsairLink/protocol/asetek/temperature.c
		OpenCorsairLink/protocol/asetekpro/core.c
		OpenCorsairLink/protocol/asetekpro/fan.c
		OpenCorsairLink/protocol/asetekpro/pump.c
		OpenCorsairLink/protocol/asetekpro/led.c
		OpenCorsairLink/protocol/asetekpro/temperature.c
		OpenCorsairLink/lowlevel/coolit.c
		OpenCorsairLink/protocol/coolit/core.c
		OpenCorsairLink/protocol/coolit/fan.c
		OpenCorsairLink/protocol/coolit/led.c
		OpenCorsairLink/protocol/coolit/pump.c
		OpenCorsairLink/protocol/coolit/temperature.c
    OpenCorsairLink/logic/scan.c
		OpenCorsairLink/driver.c
    OpenCorsairLink/device.c
		OpenCorsairLink/print.c)

# Override print.h to avoid annoying prints to stdout
set(POWERMETER_OPENCORSAIRLINK_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/OpenCorsairLink/include)
configure_file(print.h.in print.h)
include_directories(${CMAKE_BINARY_DIR})

# Build OpenCorsairLink as a static library
include_directories(SYSTEM OpenCorsairLink/include ${LIBUSB_INCLUDE_DIR})
add_library(opencorsairlink ${POWERMETER_OPENCORSAIRLINK_FILES})
target_link_libraries(opencorsairlink ${LIBUSB_LIBRARY})

# Build CorsairPowerLib as a static library
include_directories(${CMAKE_SOURCE_DIR})
set(CMAKE_CXX_FLAGS "-std=c++11")
set(POWERMETER_SRC PowerMeter.cpp)
add_library(corsairpower ${POWERMETER_SRC})
target_link_libraries(corsairpower opencorsairlink ${CMAKE_THREAD_LIBS_INIT})

# Test executable 
add_executable(Test Test.cpp)
target_link_libraries(Test corsairpower)
