# -----------------------------------------------------------------------------
# Set the paths in this file if you want to build Inkscape from a shell other than the 
# Windows built-in command line (i.e. MSYS) or IDEs such as CodeLite. These variables
# will be used as default if no environment variables are set.
# -----------------------------------------------------------------------------

# Directory containing the precompiled Inkscape libraries. Usually c:\devlibs or c:\devlibs64
set(ENV_DEVLIBS_PATH C:/devlibs64)

# Directory containing the MinGW instance used for compilation. Usually c:\mingw or c:\mingw64
set(ENV_MINGW_PATH C:/mingw64)

# -----------------------------------------------------------------------------
# MinGW Configuration
# -----------------------------------------------------------------------------
message(STATUS "Configuring MinGW environment:")

if("$ENV{MSYSTEM_CHOST}" STREQUAL "")
  if("$ENV{DEVLIBS_PATH}" STREQUAL "")
    message(STATUS "  Setting path to development libraries from mingwenv.cmake: ${ENV_DEVLIBS_PATH}")
    set(DEVLIBS_PATH ${ENV_DEVLIBS_PATH})
  else()
    message(STATUS "  Setting path to development libraries from environment: $ENV{DEVLIBS_PATH}")
    set(DEVLIBS_PATH $ENV{DEVLIBS_PATH})
  endif()

  if("$ENV{MINGW_PATH}" STREQUAL "")
    message(STATUS "  Setting path to MinGW from mingwenv.cmake: ${ENV_MINGW_PATH}")
    set(MINGW_PATH ${ENV_MINGW_PATH})
  else()
    message(STATUS "  Setting path to MinGW from environment: $ENV{MINGW_PATH}")
    set(MINGW_PATH $ENV{MINGW_PATH})
  endif()

  # Normalize directory separator slashes.
  string(REGEX REPLACE "\\\\" "/" DEVLIBS_PATH ${DEVLIBS_PATH})
  string(REGEX REPLACE "\\\\" "/" MINGW_PATH ${MINGW_PATH})
else()
  set(HAVE_MSYS2 ON)
  message(STATUS "  Detected MinGW environment provided by MSYS2")
  set(MINGW_PATH $ENV{MINGW_PREFIX})
endif()

# -----------------------------------------------------------------------------
# DEVLIBS CHECKS
# -----------------------------------------------------------------------------
if(NOT HAVE_MSYS2)
  # Directory containing the compile time .dll.a and .a files.
  set(DEVLIBS_LIB "${DEVLIBS_PATH}/lib")

  if(NOT EXISTS "${DEVLIBS_LIB}")
    message(FATAL_ERROR "Inkscape development libraries directory does not exist: ${DEVLIBS_LIB}")
  endif()

  # Add devlibs libraries to linker path.
  link_directories(${DEVLIBS_LIB})

  set(DEVLIBS_INCLUDE ${DEVLIBS_PATH}/include)

  if(NOT EXISTS ${DEVLIBS_INCLUDE})
    message(FATAL_ERROR "Inkscape development libraries directory does not exist: ${DEVLIBS_INCLUDE}")
  endif()

  # Add general MinGW headers to compiler include path.
  #include_directories(${DEVLIBS_INCLUDE})

  # Directory containing the precompiled .dll files.
  set(DEVLIBS_BIN ${DEVLIBS_PATH}/bin)

  if(NOT EXISTS ${DEVLIBS_BIN})
    message(FATAL_ERROR "Inkscape development binaries directory does not exist: ${DEVLIBS_BIN}")
  endif()

  # Directory containing the pkgconfig .pc files.
  set(PKG_CONFIG_PATH "${DEVLIBS_PATH}/lib/pkgconfig")

  if(NOT EXISTS "${PKG_CONFIG_PATH}")
    message(FATAL_ERROR "pkgconfig directory does not exist: ${PKG_CONFIG_PATH}")
  endif()

  # Add the devlibs directories to the paths used to find libraries and programs.
  list(APPEND CMAKE_PREFIX_PATH ${DEVLIBS_PATH})

  # Eliminate error messages when not having mingw in the environment path variable.
  list(APPEND CMAKE_PROGRAM_PATH  ${DEVLIBS_BIN})
endif()

# -----------------------------------------------------------------------------
# MINGW CHECKS
# -----------------------------------------------------------------------------

# We are in a MinGW environment.
set(HAVE_MINGW ON)
  
# Try to determine the MinGW processor architecture.
if(EXISTS "${MINGW_PATH}/mingw32")
  set(HAVE_MINGW64 OFF)
  set(HAVE_MINGW_W64 OFF)
  set(MINGW_ARCH mingw32)
elseif(EXISTS "${MINGW_PATH}/i686-w64-mingw32")
  set(HAVE_MINGW64 OFF)
  set(HAVE_MINGW_W64 ON)
  set(MINGW_ARCH i686-w64-mingw32)
elseif(EXISTS "${MINGW_PATH}/x86_64-w64-mingw32")
  set(HAVE_MINGW64 ON)
  set(HAVE_MINGW_W64 ON)
  set(MINGW_ARCH x86_64-w64-mingw32)
else()
  message(FATAL_ERROR "Unable to determine MinGW processor architecture. Are you using an unsupported MinGW version?")
endif()

# Path to processor architecture specific binaries and libs.
set(MINGW_ARCH_PATH "${MINGW_PATH}/${MINGW_ARCH}")

set(MINGW_BIN "${MINGW_PATH}/bin")

if(NOT EXISTS ${MINGW_BIN})
  message(FATAL_ERROR "MinGW binary directory does not exist: ${MINGW_BIN}")
endif()

# Eliminate error messages when not having mingw in the environment path variable.
list(APPEND CMAKE_PROGRAM_PATH  ${MINGW_BIN})

set(MINGW_LIB "${MINGW_PATH}/lib")

if(NOT EXISTS ${MINGW_LIB})
  message(FATAL_ERROR "MinGW library directory does not exist: ${MINGW_LIB}")
endif()

# Add MinGW libraries to linker path.
link_directories(${MINGW_LIB})

set(MINGW_INCLUDE "${MINGW_PATH}/include")

if(NOT EXISTS ${MINGW_INCLUDE})
  message(FATAL_ERROR "MinGW include directory does not exist: ${MINGW_INCLUDE}")
endif()

# Add general MinGW headers to compiler include path.
include_directories(SYSTEM ${MINGW_INCLUDE})

if(HAVE_MINGW_W64)
  set(MINGW_ARCH_LIB "${MINGW_ARCH_PATH}/lib")
  
  if(NOT EXISTS ${MINGW_ARCH_LIB})
  message(FATAL_ERROR "MinGW-w64 toolchain libraries directory does not exist: ${MINGW_ARCH_LIB}")
  endif()

  # Add 64-Bit libraries to linker path.
  link_directories(${MINGW_ARCH_LIB})
  
  set(MINGW_ARCH_INCLUDE "${MINGW_ARCH_PATH}/include")
  
  if(NOT EXISTS ${MINGW_ARCH_INCLUDE})
  message(FATAL_ERROR "MinGW-w64 toolchain include directory does not exist: ${MINGW_ARCH_INCLUDE}")
  endif()

  # Add 64-Bit MinGW headers to compiler include path.
  include_directories(${MINGW_ARCH_INCLUDE})
endif()

# -----------------------------------------------------------------------------
# LIBRARY AND LINKER SETTINGS
# -----------------------------------------------------------------------------

# Tweak CMake into using Unix-style library names.
set(CMAKE_FIND_LIBRARY_PREFIXES "lib")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".dll.a" ".dll")

if(NOT HAVE_MINGW_W64)
  list(APPEND CMAKE_FIND_LIBRARY_SUFFIXES ".a")
endif()