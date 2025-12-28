# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/cody/RAZ-RE/Firmware/keil-mdk/MDK-ARM/tmp/ReFlasher+RAZ")
  file(MAKE_DIRECTORY "/home/cody/RAZ-RE/Firmware/keil-mdk/MDK-ARM/tmp/ReFlasher+RAZ")
endif()
file(MAKE_DIRECTORY
  "/home/cody/RAZ-RE/Firmware/keil-mdk/MDK-ARM/tmp/1"
  "/home/cody/RAZ-RE/Firmware/keil-mdk/MDK-ARM/tmp/ReFlasher+RAZ"
  "/home/cody/RAZ-RE/Firmware/keil-mdk/MDK-ARM/tmp/ReFlasher+RAZ/tmp"
  "/home/cody/RAZ-RE/Firmware/keil-mdk/MDK-ARM/tmp/ReFlasher+RAZ/src/ReFlasher+RAZ-stamp"
  "/home/cody/RAZ-RE/Firmware/keil-mdk/MDK-ARM/tmp/ReFlasher+RAZ/src"
  "/home/cody/RAZ-RE/Firmware/keil-mdk/MDK-ARM/tmp/ReFlasher+RAZ/src/ReFlasher+RAZ-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/cody/RAZ-RE/Firmware/keil-mdk/MDK-ARM/tmp/ReFlasher+RAZ/src/ReFlasher+RAZ-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/cody/RAZ-RE/Firmware/keil-mdk/MDK-ARM/tmp/ReFlasher+RAZ/src/ReFlasher+RAZ-stamp${cfgdir}") # cfgdir has leading slash
endif()
