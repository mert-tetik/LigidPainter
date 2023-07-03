# LigidPainter - The only CMakeLists.txt file



cmake_minimum_required(VERSION 3.8)

# Define the project data
project(LigidPainter VERSION 1.3)

# Glob all source files in the LigidPainter directory
file(GLOB_RECURSE LIGID_SOURCES
   ${PROJECT_SOURCE_DIR}/LigidPainter/*.cpp
   ${PROJECT_SOURCE_DIR}/LigidPainter/*.c
)

# Disable warnings
add_definitions(-w)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)





# Fixes the linker errors in the unix environment
if(UNIX)
   if(APPLE)
      # Apparently fixes numerous glfw3 linker errors
      # Might be deleted if it causes any problems
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINKER_FLAGS} -I/usr/local/lib -framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL -lglfw3")
   endif(APPLE)

   # Find GLFW3 and X11 packages
   find_package(glfw3 REQUIRED)
   find_package(X11 REQUIRED)
   
   # Find BZip2 and ZLIB packages (used by Assimp)
   find_package(BZip2 REQUIRED)
   find_package(ZLIB REQUIRED)
endif(UNIX)





# Define CMAKE_HAS_X
add_definitions(-DCMAKE_HAS_X)

# Set GLFW build options
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
set(NMGFX_FIND_GLFW OFF)




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





# Installer
install(TARGETS Ligid)

install(
            FILES
            ${CMAKE_SOURCE_DIR}/freetype.dll
            ${CMAKE_SOURCE_DIR}/assimp-vc142-mtd.dll
            ${CMAKE_SOURCE_DIR}/OpenAL32.dll
            DESTINATION bin
        )

install(
            DIRECTORY
            ${CMAKE_SOURCE_DIR}/LigidPainter/Resources
            DESTINATION bin/LigidPainter
        )





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
set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 1)
set(CPACK_PACKAGE_VERSION_PATCH 1)
set(CPACK_GENERATOR WIX)

include(CPack)




# Add include directories for Ligid target
target_include_directories(Ligid PRIVATE ${PROJECT_SOURCE_DIR}/LigidPainter/Main/thirdparty/include/)
target_include_directories(Ligid PRIVATE ${PROJECT_SOURCE_DIR}/LigidPainter/Main/thirdparty)
target_include_directories(Ligid PRIVATE ${PROJECT_SOURCE_DIR}/LigidPainter/Main/src)

# Add link directories for Ligid target
target_link_directories(Ligid PRIVATE ${PROJECT_SOURCE_DIR}/LigidPainter/Main/thirdparty/lib)
target_link_directories(Ligid PRIVATE ${PROJECT_SOURCE_DIR}/LigidPainter/Main/thirdparty/lib/freetype)

# Include directories for X11 and GLFW
include_directories(${X11_INCLUDE_DIR})
include_directories(${GLFW_INCLUDE_DIRS})

if(UNIX)
   # Link libraries for Unix
   target_link_libraries(Ligid PRIVATE freetype assimp OpenAL32 glfw3 ZLIB::ZLIB BZip2::BZip2 ${X11_LIBRARIES} ${GLFW_LIBRARIES})
elseif(WIN32)
   # Link libraries for Windows
   target_link_libraries(Ligid PRIVATE freetype OpenAL32 assimp-vc142-mtd glfw3)
endif(UNIX)
