#------------------------------------------------------------------------------
#                                Project Setup
#------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(CALE C CXX)

# Silence warning CMP0042
if (APPLE AND NOT DEFINED CMAKE_MACOSX_RPATH)
  set(CMAKE_MACOSX_RPATH true)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(CTest)

# Prohibit in-source builds
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds are not allowed. Please use "
    "./configure to choose a build directory and "
    "initialize the build configuration.")
endif ()

# Keep make output sane
set(CMAKE_VERBOSE_MAKEFILE false
  CACHE STRING "Show all outputs including compiler lines." FORCE)

#------------------------------------------------------------------------------
#                               Compiler Setup
#------------------------------------------------------------------------------

if (NOT CMAKE_CXX_FLAGS)
  set(EXTRA_FLAGS "-Wall -Wextra -pedantic -std=c++14 -ftemplate-backtrace-limit=0")
  set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
  set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os")
  set(CMAKE_CXX_FLAGS_RELEASE        "-O3")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
endif()

# Performance.
set(EXTRA_FLAGS "${EXTRA_FLAGS} -march=native")

# Requirement checks
try_run(program_result
        compilation_succeeded
        "${CMAKE_BINARY_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/get_compiler_version.cpp"
        RUN_OUTPUT_VARIABLE compiler_version)
if (NOT compilation_succeeded OR NOT program_result EQUAL 0)
  message(FATAL_ERROR "Cannot determine compiler version")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
  # Too many false positives.
  set(EXTRA_FLAGS "${EXTRA_FLAGS} -Wno-maybe-uninitialized")
  if (compiler_version VERSION_GREATER 6.0
      OR compiler_version VERSION_EQUAL 6.0)
    message(STATUS "Found GCC ${compiler_version}")
  else ()
    message(FATAL_ERROR "GCC >= 6.0 required, found ${compiler_version}")
  endif ()
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  if (compiler_version VERSION_GREATER 3.5
      OR compiler_version VERSION_EQUAL 3.5)
    message(STATUS "Found Clang ${compiler_version}")
  else ()
    message(FATAL_ERROR "Clang >= 3.5 required, found ${compiler_version}")
  endif ()
  if (NOT NO_AUTO_LIBCPP)
    # Standard library selection: check if libc++ is available, and if so, use
    # it. Otherwise fall back to libstdc++.
    set(CXXFLAGS_BACKUP "${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS "-std=c++14 -stdlib=libc++")
    try_run(program_result
            compilation_succeeded
            "${CMAKE_BINARY_DIR}"
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/get_compiler_version.cpp")
    set(CMAKE_CXX_FLAGS "${CXXFLAGS_BACKUP}")
    if(NOT compilation_succeeded)
      message(STATUS "Using libstdc++ as standard library")
    else()
      message(STATUS "Automatically adding -stdlib=libc++ flag")
      set(EXTRA_FLAGS "${EXTRA_FLAGS} -stdlib=libc++")
    endif()
  endif()
else ()
  message(FATAL_ERROR "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
endif ()

if (ENABLE_ADDRESS_SANITIZER)
  set(CXXFLAGS_BACKUP "${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
  try_run(program_result
          compilation_succeeded
          "${CMAKE_BINARY_DIR}"
          "${CMAKE_CURRENT_SOURCE_DIR}/cmake/get_compiler_version.cpp")
  set(CMAKE_CXX_FLAGS "${CXXFLAGS_BACKUP}")
  if(NOT compilation_succeeded)
    message(STATUS "Address Sanitizer not available on selected compiler")
  else()
    message(STATUS "Enabling Address Sanitizer")
    set(EXTRA_FLAGS "${EXTRA_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(ASAN_FOUND true)
  endif()
endif(ENABLE_ADDRESS_SANITIZER)

# Only set our flags if the environment variable CXXFLAGS is not present.
if (NOT CMAKE_CXX_FLAGS)
  set(CMAKE_CXX_FLAGS ${EXTRA_FLAGS})
endif ()

# Pick up 3rd-party packages and ports.
if (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
  include_directories(/usr/local/include)
  link_directories(/usr/local/lib)
endif ()

#------------------------------------------------------------------------------
#                          Dependency Configuration
#------------------------------------------------------------------------------

if (CALE_ENABLE_ASSERTIONS)
  find_package(Backtrace)
  if (NOT Backtrace_FOUND)
    include_directories(${Backtrace_INCLUDE_DIRS})
    message(FATAL_ERROR "Could not find backtrace (execinfo) libraries")
  endif ()
endif ()

if (NOT CAF_ROOT_DIR AND CALE_PREFIX)
  set(CAF_ROOT_DIR ${CALE_PREFIX})
endif ()
find_package(CAF COMPONENTS core io test REQIURED)

#------------------------------------------------------------------------------
#                                 Build Setup
#------------------------------------------------------------------------------

# make uninstall
if ("${PROJECT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    @ONLY)
  add_custom_target(uninstall COMMAND
    ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif ()

# make configure
add_custom_target(configure COMMAND ${CMAKE_CURRENT_BINARY_DIR}/config.status)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libfluent)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin
  CACHE PATH "Single directory for all executables")

set (LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib CACHE
  PATH "Single directory for all libraries")

add_subdirectory(libfluent)

set(FLUENT_LIBRARIES libfluent)

add_subdirectory(examples)

#------------------------------------------------------------------------------
#                                Build Summary
#------------------------------------------------------------------------------

macro(display test desc summary)
  if (${test})
    set(${summary} ${desc})
  else ()
    set(${summary} no)
  endif()
endmacro()

set(caf_dir ${CAF_LIBRARY_CORE})
get_filename_component(caf_dir ${caf_dir} PATH)
if (EXISTS "${caf_dir}/../libcaf_core")
  get_filename_component(caf_dir ${caf_dir} PATH)
else ()
  set(caf_dir ${CAF_INCLUDE_DIR_CORE})
endif ()

display(CALE_ENABLE_ASSERTIONS yes assertions_summary)
display(ASAN_FOUND yes asan_summary)
display(CAF_FOUND ${caf_dir} caf_summary)

set(summary
    "====================|  Build Summary  |===================="
    "\n"
    "\nSource directory: ${CMAKE_SOURCE_DIR}"
    "\nBuild directory:  ${CMAKE_BINARY_DIR}"
    "\nInstall prefix:   ${CMAKE_INSTALL_PREFIX}"
    "\n"
    "\nBuild type:       ${CMAKE_BUILD_TYPE}"
    "\nLog level:        ${log_level_summary}"
    "\nAssertions:       ${assertions_summary}"
    "\nAddressSanitizer: ${asan_summary}"
    "\n"
    "\nCC:               ${CMAKE_C_COMPILER}"
    "\nCXX:              ${CMAKE_CXX_COMPILER}"
    "\nCXXFLAGS:         ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${build_type}}"
    "\n"
    "\nCAF:              ${caf_summary}"
    "\n"
    "\n===========================================================")

message("\n" ${summary} "\n")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/config.summary ${summary})
