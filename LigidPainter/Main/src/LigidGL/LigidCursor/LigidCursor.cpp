/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    See the ./README.md file for the description

*/

#include <iostream>

#include "LigidGL/LigidGL.hpp"


#if defined(_WIN32) || defined(_WIN64)
//User has Windows
#include <Windows.h>

#elif defined(__APPLE__)
//User has MacOS
#import <Cocoa/Cocoa.h>


#elif defined(__linux__)
//User has Linux
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <utility>

#endif

void LigidCursor::createCursor(int cursorWidth, int cursorHeight, int cursorHotspotX, int cursorHotspotY, unsigned char* cursorPixelData) {
#if defined(_WIN32) || defined(_WIN64)
    // User has Windows

    // Allocate memory for the AND and XOR masks
    BYTE* andMask = new BYTE[cursorWidth * cursorHeight / 8];
    BYTE* xorMask = new BYTE[cursorWidth * cursorHeight / 8];

    // Convert pixel data to mask data
    for (int i = 0; i < cursorWidth * cursorHeight; ++i) {
        BYTE pixelValue = cursorPixelData[i];
        andMask[i / 8] |= (pixelValue == 0 ? 0x80 : 0);
        xorMask[i / 8] |= (pixelValue == 1 ? 0x80 : 0);

        if (i % 8 < 7) {
            // Shift the bits to the right for the next pixel
            andMask[i / 8] >>= 1;
            xorMask[i / 8] >>= 1;
        }
    }

    // Create the cursor using the AND and XOR masks
    this->cursorHandle = CreateIcon(GetModuleHandle(nullptr), cursorWidth, cursorHeight, 1, 1, andMask, xorMask);

    // Clean up allocated memory
    delete[] andMask;
    delete[] xorMask;

#elif defined(__APPLE__)
//User has MacOS


#elif defined(__linux__)
//User has Linux

#endif
}

#if defined(_WIN32) || defined(_WIN64)
//User has Windows

HCURSOR LigidCursor::getCursorHandle(){
    return this->cursorHandle;
}

#elif defined(__APPLE__)
//User has MacOS

#elif defined(__linux__)
//User has Linux

#endif
