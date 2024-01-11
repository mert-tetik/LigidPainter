/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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

void LigidCursor::createCursor(int cursorWidth, int cursorHeight, int cursorHotspotX, int cursorHotspotY, unsigned char* cursorPixelData, int cursorType) {
    
    //Set the cursor type
    this->cursorType = cursorType;

#if defined(_WIN32) || defined(_WIN64)
    // Calculate the number of bytes per row in the cursorPixelData
    int bytesPerRow = cursorWidth * 4;

    // Create a BITMAPINFO structure to describe the cursor bitmap
    BITMAPINFO bitmapInfo = {0};
    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth = cursorWidth;
    bitmapInfo.bmiHeader.biHeight = -cursorHeight;  // Negative height to indicate a top-down bitmap
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

    // Create a device-independent bitmap (DIB) section for the cursor bitmap
    void* pBitmapBits;
    HBITMAP hBitmap = CreateDIBSection(NULL, &bitmapInfo, DIB_RGB_COLORS, &pBitmapBits, NULL, 0);
    if (!hBitmap)
    {
        // Failed to create the DIB section
        return ;
    }

    // Copy the pixel data into the DIB section
    memcpy(pBitmapBits, cursorPixelData, cursorHeight * bytesPerRow);

    // Create the mask bitmap
    BYTE* pMaskBits = new BYTE[cursorWidth * cursorHeight / 8];
    memset(pMaskBits, 0xFF, cursorWidth * cursorHeight / 8);

    // Create the cursor
    ICONINFO iconInfo;
    iconInfo.fIcon = FALSE;
    iconInfo.xHotspot = cursorHotspotX;
    iconInfo.yHotspot = cursorHotspotY;
    iconInfo.hbmMask = CreateBitmap(cursorWidth, cursorHeight, 1, 1, pMaskBits);
    iconInfo.hbmColor = hBitmap;

    HCURSOR hCursor = CreateIconIndirect(&iconInfo);

    // Clean up resources
    delete[] pMaskBits;
    DeleteObject(hBitmap);
    DeleteObject(iconInfo.hbmMask);

    this->cursorHandle = hCursor;
    // Clean up allocated memory

#elif defined(__APPLE__)
    // User has MacOS

#elif defined(__linux__)
    // User has Linux

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
