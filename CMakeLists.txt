cmake_minimum_required(VERSION 3.15)

#! Check every comment after the "#!"

#! CHANGE YOUR PROJECT NAME
#  It is used as your project's main executable name.
set(PROJECT_NAME counter)
project(${PROJECT_NAME} C CXX) # project(${PROJECT_NAME} C CXX ASM)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

##########################################################
# Other variables
##########################################################

set(TEST_EXECUTABLE test_ts_queue)
#set(ONE_THREAD_EXE serial)

##########################################################
# User configurable options of the template
##########################################################
# Note: symbols like WARNINGS_AS_ERRORS in configuration are intentionally variables
# and not CMake options --using options creates too much problem for students.

#! It is a good practice to set "WARNINGS_AS_ERRORS" ON,
#  but sometimes it creates too much trouble, so default is OFF.
set(WARNINGS_AS_ERRORS OFF)

#! Always use PVS Studio while developing.
set(ENABLE_PVS_STUDIO OFF)

#! Select appropriate sanitizers.
#  Definitely enable sanitizers while developing.
#  Disable it for the production builds and before submitting for grading.
#  Only one of Memory (MSAN), Address (ASAN), or Thread (TSan)
#  sanitizers is applicable at the time -- the first defined.

#! UndefinedBehaviorSanitizer (UBSan)
#  Info: https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html
set(ENABLE_UBSan OFF)
#! AddressSanitizer -- detects use after free or after scope exit,
#  memory overflows and leaks.
#  Info: https://github.com/google/sanitizers/wiki/AddressSanitizer
set(ENABLE_ASAN OFF)
#! ThreadSanitizer -- detects data races.
set(ENABLE_TSan OFF)
#! MemorySanitizer detects uninitialized memory reads
#  Info: https://github.com/google/sanitizers/wiki/MemorySanitizer
set(ENABLE_MSAN OFF)


#! Be default -- build release version if not specified otherwise.
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif ()

# Warnings as errors should be imported here -- do not move this line
include(cmake/CompilerWarnings.cmake)

##########################################################
# Project files, packages, libraries and so on
##########################################################

#! Project main executable source compilation
add_executable(${PROJECT_NAME} src/main.cpp
		src/options_parser.cpp src/options_parser.h
		src/thread_safe_queue.cpp src/thread_safe_queue.h
		src/time_measurement.h src/files_methods.h src/files_methods.cpp
		src/write_in_file.h src/write_in_file.cpp
		src/thread_functions.h src/thread_functions.cpp
		src/errors.h)

#add_executable(${ONE_THREAD_EXE} src/serial.cpp
#        src/options_parser.cpp src/options_parser.h
#        src/methods.h src/integration_structures.h)
#
add_executable(${TEST_EXECUTABLE} tests/test_ts_queue.cpp
		src/options_parser.cpp src/options_parser.h
		src/thread_safe_queue.cpp src/thread_safe_queue.h)

#! Put path to your project headers
target_include_directories(${PROJECT_NAME} PRIVATE options_parser)
target_include_directories(${TEST_EXECUTABLE} PRIVATE options_parser)
#target_include_directories(${ONE_THREAD_EXE} PRIVATE options_parser)

#! Add external packages
# options_parser requires boost::program_options library
find_package(Boost 1.71.0 COMPONENTS program_options system REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} Boost::program_options Boost::system)

target_include_directories(${TEST_EXECUTABLE} PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(${TEST_EXECUTABLE} Boost::program_options Boost::system)

#target_include_directories(${ONE_THREAD_EXE} PRIVATE ${Boost_INCLUDE_DIR})
#target_link_libraries(${ONE_THREAD_EXE} Boost::program_options Boost::system)

##########################################################
# Fixed CMakeLists.txt part
##########################################################

INSTALL(PROGRAMS
		$<TARGET_FILE:${PROJECT_NAME}> # ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
		DESTINATION bin)

# Define ALL_TARGETS variable to use in PVS and Sanitizers
set(ALL_TARGETS ${PROJECT_NAME})

# Include CMake setup
include(cmake/main-config.cmake)