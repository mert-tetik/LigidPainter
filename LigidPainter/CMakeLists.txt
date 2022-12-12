# CMakeList.txt : CMake project for LigidPainter, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
project(LigidPainter VERSION 1.0)

# Add source to this project's executable.
#add_executable (LigidPainter  "Main/src/Main.cpp")
file(GLOB_RECURSE LIGID_SOURCES
   ${PROJECT_SOURCE_DIR}/*.cpp
   ${PROJECT_SOURCE_DIR}/Main/thirdparty/glad.c
   ${PROJECT_SOURCE_DIR}/Main/thirdparty/tinyfiledialogs.c
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Ligid
   ${LIGID_SOURCES})

target_include_directories(Ligid PRIVATE ${PROJECT_SOURCE_DIR}/Main/thirdparty/include/)
target_include_directories(Ligid PRIVATE ${PROJECT_SOURCE_DIR}/Main/thirdparty)
target_include_directories(Ligid PRIVATE ${PROJECT_SOURCE_DIR}/Main/src)
target_link_directories(Ligid PRIVATE ${PROJECT_SOURCE_DIR}/Main/thirdparty/lib)
target_link_directories(Ligid PRIVATE ${PROJECT_SOURCE_DIR}/Main/thirdparty/lib/freetype)
target_link_libraries(Ligid PRIVATE glfw3 freetype assimp-vc142-mt)

# TODO: Add tests and install targets if needed.