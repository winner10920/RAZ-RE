# groups.cmake

# group STARTUP
add_library(Group_STARTUP OBJECT
  "${SOLUTION_ROOT}/../firmware/CMSIS/device/startup/startup_n32g031.s"
)
target_include_directories(Group_STARTUP PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Group_STARTUP PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_STARTUP_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_STARTUP_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_STARTUP PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_STARTUP PUBLIC
  Group_STARTUP_ABSTRACTIONS
)
set(COMPILE_DEFINITIONS
  __MICROLIB
  N32G031 USE_STDPERIPH_DRIVER
)
cbuild_set_defines(AS_ARM COMPILE_DEFINITIONS)
set_source_files_properties("${SOLUTION_ROOT}/../firmware/CMSIS/device/startup/startup_n32g031.s" PROPERTIES
  COMPILE_FLAGS "${COMPILE_DEFINITIONS}"
)
set_source_files_properties("${SOLUTION_ROOT}/../firmware/CMSIS/device/startup/startup_n32g031.s" PROPERTIES
  COMPILE_OPTIONS "-masm=armasm"
)

# group CMSIS
add_library(Group_CMSIS OBJECT
  "${SOLUTION_ROOT}/../firmware/CMSIS/device/system_n32g031.c"
)
target_include_directories(Group_CMSIS PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Group_CMSIS PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_CMSIS_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_CMSIS_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_CMSIS PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_CMSIS PUBLIC
  Group_CMSIS_ABSTRACTIONS
)

# group FWLIB
add_library(Group_FWLIB OBJECT
  "${SOLUTION_ROOT}/../firmware/n32g031_std_periph_driver/src/n32g031_gpio.c"
  "${SOLUTION_ROOT}/../firmware/n32g031_std_periph_driver/src/n32g031_spi.c"
  "${SOLUTION_ROOT}/../firmware/n32g031_std_periph_driver/src/n32g031_rcc.c"
  "${SOLUTION_ROOT}/../firmware/n32g031_std_periph_driver/src/n32g031_dma.c"
  "${SOLUTION_ROOT}/../firmware/n32g031_std_periph_driver/src/n32g031_tim.c"
  "${SOLUTION_ROOT}/../firmware/n32g031_std_periph_driver/src/n32g031_adc.c"
)
target_include_directories(Group_FWLIB PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Group_FWLIB PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_FWLIB_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_FWLIB_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_FWLIB PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_FWLIB PUBLIC
  Group_FWLIB_ABSTRACTIONS
)

# group USER
add_library(Group_USER OBJECT
  "${SOLUTION_ROOT}/../src/main.c"
  "${SOLUTION_ROOT}/../src/n32g031_it.c"
  "${SOLUTION_ROOT}/../src/spi_flash.c"
  "${SOLUTION_ROOT}/../src/nv3029.c"
  "${SOLUTION_ROOT}/../src/dma.c"
  "${SOLUTION_ROOT}/../src/pwm.c"
  "${SOLUTION_ROOT}/../src/sleep_wake.c"
  "${SOLUTION_ROOT}/../src/voltage_monitor.c"
)
target_include_directories(Group_USER PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Group_USER PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_USER_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_USER_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_USER PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_USER PUBLIC
  Group_USER_ABSTRACTIONS
)
