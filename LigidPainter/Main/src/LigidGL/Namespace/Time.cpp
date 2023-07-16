/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

*/

#include <iostream>
#include <functional>
#include <chrono>

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

double LigidGL::getTime(){
    // Store the start time as a static constant
    static const auto startTime = std::chrono::high_resolution_clock::now();

    // Get the current time
    const auto currentTime = std::chrono::high_resolution_clock::now();

    // Calculate the duration between the current time and the start time
    const auto duration = currentTime - startTime;

    // Convert the duration to seconds and return as a double value
    return std::chrono::duration<double>(duration).count();
}