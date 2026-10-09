# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/richfedor/RAZ-RE/Firmware/keil-mdk/MDK-ARM/tmp/ReFlasher+RAZ")
  file(MAKE_DIRECTORY "/home/richfedor/RAZ-RE/Firmware/keil-mdk/MDK-ARM/tmp/ReFlasher+RAZ")
endif()
file(MAKE_DIRECTORY
  "/home/richfedor/RAZ-RE/Firmware/keil-mdk/MDK-ARM/tmp/1"
  "/home/richfedor/RAZ-RE/Firmware/keil-mdk/MDK-ARM/tmp/ReFlasher+RAZ"
  "/home/richfedor/RAZ-RE/Firmware/keil-mdk/MDK-ARM/tmp/ReFlasher+RAZ/tmp"
  "/home/richfedor/RAZ-RE/Firmware/keil-mdk/MDK-ARM/tmp/ReFlasher+RAZ/src/ReFlasher+RAZ-stamp"
  "/home/richfedor/RAZ-RE/Firmware/keil-mdk/MDK-ARM/tmp/ReFlasher+RAZ/src"
  "/home/richfedor/RAZ-RE/Firmware/keil-mdk/MDK-ARM/tmp/ReFlasher+RAZ/src/ReFlasher+RAZ-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/richfedor/RAZ-RE/Firmware/keil-mdk/MDK-ARM/tmp/ReFlasher+RAZ/src/ReFlasher+RAZ-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/richfedor/RAZ-RE/Firmware/keil-mdk/MDK-ARM/tmp/ReFlasher+RAZ/src/ReFlasher+RAZ-stamp${cfgdir}") # cfgdir has leading slash
endif()
