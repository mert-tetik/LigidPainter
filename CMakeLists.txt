# LigidPainter - The only CMakeLists.txt file


cmake_minimum_required(VERSION 3.8)

# Define the project data
project(LigidPainter VERSION 2.0)

# Glob all source files in the LigidPainter directory
file(GLOB_RECURSE LIGID_SOURCES
   ${PROJECT_SOURCE_DIR}/LigidPainter/*.cpp
   ${PROJECT_SOURCE_DIR}/LigidPainter/*.c
)


# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# Define CMAKE_HAS_X
add_definitions(-DCMAKE_HAS_X)


if(WIN32)
    # Add Windows-specific configurations
    add_executable(  
                    Ligid
                    ${LIGID_SOURCES}
                    ${CMAKE_SOURCE_DIR}/Installer/AppIcon.rc
                 )
   
    target_compile_definitions(Ligid PRIVATE CRT)

else()
   
# Add non-Windows configurations
   add_executable(
                    Ligid
                    ${LIGID_SOURCES}
                )

endif(WIN32)



# Set the character set to Unicode
target_compile_definitions(Ligid PRIVATE UNICODE _UNICODE)



# Installer
install(TARGETS Ligid)

install(
            FILES
            ${CMAKE_SOURCE_DIR}/freetype.dll
            ${CMAKE_SOURCE_DIR}/OpenAL32.dll
            ${CMAKE_SOURCE_DIR}/zlibd.dll
            DESTINATION bin
        )

install(
            DIRECTORY
            ${CMAKE_SOURCE_DIR}/LigidPainter/Resources
            DESTINATION bin/LigidPainter
        )



# Set the project version
set(PROJECT_VERSION_MAJOR 2)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)


# Set CPack package details
set(CPACK_PACKAGE_NAME LigidPainter)  # App name
set(CPACK_PACKAGE_VENDOR LigidTools)  # Publisher name
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE.txt)  # The license
set(CPACK_RESOURCE_FILE_WELCOME ${CMAKE_SOURCE_DIR}/Installer/Welcome.txt)  # The welcome text
set(CPACK_PACKAGE_ICON ${PROJECT_SOURCE_DIR}/LigidPainter/Resources/Icons/logo-1080x.ico)
set(CPACK_WIX_PRODUCT_ICON ${PROJECT_SOURCE_DIR}/LigidPainter/Resources/Icons/logo-1080x.ico)
set(CPACK_WIX_UI_BANNER ${CMAKE_SOURCE_DIR}/Installer/Banner.jpg)  # On top of the screen
set(CPACK_WIX_UI_DIALOG ${CMAKE_SOURCE_DIR}/Installer/Dialog.jpg)  # Background image

# Shortcut
set(CPACK_PACKAGE_EXECUTABLES Ligid;LigidPainter)
set(CPACK_CREATE_DESKTOP_LINKS Ligid)

# Set CPack package version
set(CPACK_PACKAGE_VERSION_MAJOR 2)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_GENERATOR WIX)

include(CPack)




# Cross compiling
#set(CMAKE_SYSTEM_NAME Darwin)
#set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Set the path to the macOS SDK
#set(MACOSX_SYSROOT ${PROJECT_SOURCE_DIR}/Cross/MacOS/MacOSX10.13.sdk)

# Set the path to the cross compilation tools (C)
# set(CMAKE_C_COMPILER "C:/mingw64/bin/gcc.exe")
# Set the path to the cross compilation tools (C++)
# set(CMAKE_C_COMPILER "C:/mingw64/bin/g++.exe")

# Set the MacOS version you want to target
# set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13 CACHE STRING "Minimum macOS version to target")

find_package(OpenGL REQUIRED)


# Add include directories for Ligid target
target_include_directories(Ligid PRIVATE ${PROJECT_SOURCE_DIR}/LigidPainter/Main/thirdparty/include/)
target_include_directories(Ligid PRIVATE ${PROJECT_SOURCE_DIR}/LigidPainter/Main/thirdparty)
target_include_directories(Ligid PRIVATE ${PROJECT_SOURCE_DIR}/LigidPainter/Main/src)

# Add link directories for Ligid target
target_link_directories(Ligid PRIVATE 
    ${PROJECT_SOURCE_DIR}/LigidPainter/Main/thirdparty/lib
    ${PROJECT_SOURCE_DIR}/LigidPainter/Main/thirdparty/lib/freetype
    ${PROJECT_SOURCE_DIR}
)

# Link libraries for Ligid target
target_link_libraries(Ligid PRIVATE 
    freetype
    zlibd 
    OpenAL32 
    OpenGL::GL
)
