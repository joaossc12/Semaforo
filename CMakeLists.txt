# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

set(PICO_BOARD pico_w CACHE STRING "Board type")

include(pico_sdk_import.cmake)
set(FREERTOS_KERNEL_PATH "C:/FreeRTOS-Kernel")
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

project(Semaforo C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

include_directories(${CMAKE_SOURCE_DIR}/lib)

# Add executable. Default name is the project name, version 0.1

add_executable(Semaforo Semaforo.c lib/ssd1306.c)

pico_set_program_name(Semaforo "Semaforo")
pico_set_program_version(Semaforo "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Semaforo 1)
pico_enable_stdio_usb(Semaforo 1)

pico_generate_pio_header(Semaforo ${CMAKE_CURRENT_LIST_DIR}/Semaforo.pio)

# Add the standard library to the build
target_link_libraries(Semaforo
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(Semaforo PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(Semaforo
                    FreeRTOS-Kernel 
                    FreeRTOS-Kernel-Heap4
                    hardware_i2c
                    hardware_pwm
                    hardware_clocks
                    hardware_pio 
        
        )

pico_add_extra_outputs(Semaforo)

