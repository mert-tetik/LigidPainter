/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

*/

#include <iostream>
#include <functional>

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

void LigidGL::getPrimaryMonitorData(int& monitorWidth, int& monitorHeight, int& monitorRefreshRate){
    // --- Get the screen resolution --- 

    // Retrieve the width of the screen (primary monitor) using GetSystemMetrics
    monitorWidth = GetSystemMetrics(SM_CXSCREEN);

    // Retrieve the height of the screen (primary monitor) using GetSystemMetrics
    monitorHeight = GetSystemMetrics(SM_CYSCREEN);


    // --- Get the refresh rate --- 

    // Declare a DEVMODE structure to hold the display settings
    DEVMODE devMode;

    // Set the size of the DEVMODE structure
    devMode.dmSize = sizeof(DEVMODE);

    // Retrieve the current display settings for the primary monitor using EnumDisplaySettings
    EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devMode);

    // Extract the refresh rate from the retrieved display settings and assign it to monitorRefreshRate
    monitorRefreshRate = devMode.dmDisplayFrequency;
}
