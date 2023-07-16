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

void LigidWindow::setMousePosCallback(void (*mousePosCallback)(LigidWindow,double, double)) {
    this->mousePosCallback = mousePosCallback;
}

void LigidWindow::setMouseButtonCallback(void (*mouseButtonCallback)(LigidWindow,int, int, int)) {
    this->mouseButtonCallback = mouseButtonCallback;
}

void LigidWindow::setKeyCallback(void (*keyCallback)(LigidWindow,int, int, int)) {
    this->keyCallback = keyCallback;
}

void LigidWindow::setWindowSizeCallback(void (*windowSizeCallback)(LigidWindow,int, int)) {
    this->windowSizeCallback = windowSizeCallback;
}

void LigidWindow::setScrollCallback(void (*scrollCallback)(LigidWindow,double, double)) {
    this->scrollCallback = scrollCallback;
}