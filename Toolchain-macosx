# Cross compiling
set(CMAKE_SYSTEM_NAME Darwin)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Set the path to the macOS SDK
set(MACOSX_SYSROOT ${PROJECT_SOURCE_DIR}/Cross/MacOS/MacOSX10.13.sdk)

# Set the path to the cross compilation tools (C)
set(CMAKE_C_COMPILER ${PROJECT_SOURCE_DIR}/Cross/mingw64/bin/gcc.exe)
# Set the path to the cross compilation tools (C++)
set(CMAKE_C_COMPILER ${PROJECT_SOURCE_DIR}/Cross/mingw64/bin/g++.exe)

# Set the MacOS version you want to target
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13 CACHE STRING "Minimum macOS version to target")