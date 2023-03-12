cmake_minimum_required (VERSION 3.8)
project(LigidPainter VERSION 1.2)

file(GLOB_RECURSE LIGID_SOURCES
   ${PROJECT_SOURCE_DIR}/*.cpp
   ${PROJECT_SOURCE_DIR}/Main/thirdparty/glad.c
   ${PROJECT_SOURCE_DIR}/Main/thirdparty/tinyfiledialogs.c
)

#Disable warnings
add_definitions(-w)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


if(UNIX)
   if(APPLE)
      #Appearently fixes numerious glfw3 linker errors
      #Might be deleted if causes any problems
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINKER_FLAGS} -I/usr/local/lib -framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL -lglfw3")
   endif(APPLE)
   find_package(glfw3 REQUIRED)
   find_package(X11 REQUIRED)
   
   find_package(BZip2 REQUIRED) #Worked in ubuntu & macosx (assimp)
   find_package(ZLIB REQUIRED) #Worked in ubuntu & macosx (assimp)
endif(UNIX) 

add_definitions(-DCMAKE_HAS_X)

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
set(NMGFX_FIND_GLFW OFF)


if (WIN32)
   add_executable(Ligid
      ${LIGID_SOURCES} ${PROJECT_SOURCE_DIR}/Resources/Icons/logo-1080x.ico)
      target_compile_definitions(Ligid PRIVATE CRT)
      
else()
   add_executable(Ligid
      ${LIGID_SOURCES})

endif (WIN32)

   
#Installer
install(TARGETS Ligid)
install(FILES
   ${CMAKE_SOURCE_DIR}/freetype.dll
   ${CMAKE_SOURCE_DIR}/assimp-vc142-mt.dll
   DESTINATION bin)
install(DIRECTORY
${CMAKE_SOURCE_DIR}/LigidPainter/Resources DESTINATION bin/LigidPainter)


set(CPACK_PACKAGE_NAME LigidPainter)#App name
set(CPACK_PACKAGE_VENDOR LigidTools)#Publisher name
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE.txt)#The license
set(CPACK_RESOURCE_FILE_WELCOME ${CMAKE_SOURCE_DIR}/Installer/Welcome.txt)#The welcome text
set(CPACK_PACKAGE_ICON,${PROJECT_SOURCE_DIR}/Resources/Icons/logo-1080x.ico)
set(CPACK_WIX_PRODUCT_ICON,${PROJECT_SOURCE_DIR}/Resources/Icons/logo-1080x.ico)
set(CPACK_WIX_UI_BANNER  ${CMAKE_SOURCE_DIR}/Installer/Banner.jpg)#On top of the screen
set(CPACK_WIX_UI_DIALOG  ${CMAKE_SOURCE_DIR}/Installer/Dialog.jpg)#Background image


#set(CPACK_DEBIAN_PACKAGE_MAINTAINER "LigidTools <${CPACK_PACKAGE_CONTACT}>")

#Shortcut
set(CPACK_PACKAGE_EXECUTABLES Ligid;LigidPainter)
set(CPACK_CREATE_DESKTOP_LINKS Ligid)

set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 1)
set(CPACK_PACKAGE_VERSION_PATCH 1)
set(CPACK_GENERATOR WIX)

include(CPack)

target_include_directories(Ligid PRIVATE ${PROJECT_SOURCE_DIR}/Main/thirdparty/include/)
target_include_directories(Ligid PRIVATE ${PROJECT_SOURCE_DIR}/Main/thirdparty)
target_include_directories(Ligid PRIVATE ${PROJECT_SOURCE_DIR}/Main/src)

target_link_directories(Ligid PRIVATE ${PROJECT_SOURCE_DIR}/Main/thirdparty/lib)
target_link_directories(Ligid PRIVATE ${PROJECT_SOURCE_DIR}/Main/thirdparty/lib/freetype)

include_directories(${X11_INCLUDE_DIR})
include_directories(${GLFW_INCLUDE_DIRS})

if(UNIX)
   target_link_libraries(Ligid PRIVATE freetype assimp glfw3 ZLIB::ZLIB BZip2::BZip2 ${X11_LIBRARIES} ${GLFW_LIBRARIES})
elseif(WIN32)
   target_link_libraries(Ligid PRIVATE freetype assimp glfw3)
endif(UNIX)