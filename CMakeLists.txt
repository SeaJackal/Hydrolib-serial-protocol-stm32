cmake_minimum_required(VERSION 3.16)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

set(BUILD_TESTING OFF)

project(CMakeTest VERSION 1.0)

enable_language(C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_program(CMAKE_C_COMPILER NAMES arm-none-eabi-gcc HINTS /bin)
find_program(CMAKE_CXX_COMPILER NAMES arm-none-eabi-g++ HINTS /bin)
find_program(CMAKE_ASM_COMPILER NAMES arm-none-eabi-gcc HINTS /bin)

set(CMAKE_EXECUTABLE_SUFFIX_C .elf)
set(CMAKE_EXECUTABLE_SUFFIX_CXX .elf)
set(CMAKE_EXECUTABLE_SUFFIX_ASM .elf)

# This should be safe to set for a bare-metal cross-compiler
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

add_compile_options(-Wall -Wextra -Wpedantic -Wc++20-compat -Wno-format-security
    -Woverloaded-virtual -Wsuggest-override -fdata-sections -ffunction-sections -g -gdwarf-2)

add_compile_options(-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard)

add_link_options(-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard)
add_link_options(-specs=nosys.specs -T${CMAKE_CURRENT_SOURCE_DIR}/STM32F407VGTx_FLASH.ld -lc -lm -lnosys
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/STMprogramTest.map,--cref -Wl,--gc-sections)

add_subdirectory(libs/Hydrolibs Hydrolibs)

file(GLOB HYDRODRIVERS drvs/Hydrodrivers/*.c)

add_executable(STMprogramTest src/main.cpp ${HYDRODRIVERS} startup_stm32f407xx.s src/system_stm32f4xx.c
    src/stm32f4xx_it.c)

target_link_libraries(STMprogramTest HydrolibSerialProtocol)

target_include_directories(STMprogramTest PUBLIC include)
target_include_directories(STMprogramTest PUBLIC drvs/Hydrodrivers)
target_include_directories(STMprogramTest PUBLIC drvs/CMSIS/Device/ST/STM32F4xx/Include)
target_include_directories(STMprogramTest PUBLIC drvs/CMSIS/Include)

target_compile_definitions(STMprogramTest PUBLIC STM32F407xx)

target_compile_features(STMprogramTest PUBLIC cxx_std_20)
