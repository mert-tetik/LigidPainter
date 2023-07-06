/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

*/

#include <iostream>

#include "LigidGL/LigidGL.hpp"


#if defined(_WIN32) || defined(_WIN64)
    
    //User has Windows
    #include <Windows.h>


#elif defined(__APPLE__)


    #include <Cocoa/Cocoa.h>


#elif defined(__linux__)


    //User has Linux
    #include <X11/Xlib.h>
    #include <GL/glx.h>
    #include <utility>


#endif

typedef BOOL (WINAPI* PFNWGLSWAPINTERVALEXTPROC)(int interval);

void LigidGL::setSwapInterval(int interval){
    
    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
    
    if (wglSwapIntervalEXT != nullptr) {
        wglSwapIntervalEXT(interval);
    }
}