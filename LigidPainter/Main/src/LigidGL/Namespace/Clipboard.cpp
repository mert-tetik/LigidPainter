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

void LigidGL::setClipboardText(const std::string& text) {
#if defined(_WIN32) || defined(_WIN64)

    // Open the clipboard
    if (!OpenClipboard(nullptr)) {
        std::cerr << "Failed to open the clipboard." << std::endl;
        return;
    }

    // Empty the clipboard
    if (!EmptyClipboard()) {
        std::cerr << "Failed to empty the clipboard." << std::endl;
        CloseClipboard();
        return;
    }

    // Get the required memory size
    const size_t textSize = text.size() + 1;
    const size_t bufferSize = textSize * sizeof(char);

    // Allocate global memory for the text
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, bufferSize);
    if (!hGlobal) {
        std::cerr << "Failed to allocate global memory." << std::endl;
        CloseClipboard();
        return;
    }

    // Lock the memory and get a pointer to it
    char* data = static_cast<char*>(GlobalLock(hGlobal));
    if (!data) {
        std::cerr << "Failed to lock global memory." << std::endl;
        GlobalFree(hGlobal);
        CloseClipboard();
        return;
    }

    // Copy the text to the global memory
    memcpy(data, text.c_str(), bufferSize);

    // Unlock the memory
    GlobalUnlock(hGlobal);

    // Set the data to the clipboard
    if (!SetClipboardData(CF_TEXT, hGlobal)) {
        std::cerr << "Failed to set clipboard data." << std::endl;
        GlobalFree(hGlobal);
        CloseClipboard();
        return;
    }

    // Close the clipboard
    CloseClipboard();

#elif defined(__APPLE__)

    //* User using Apple environment
    // TODO: Implement clipboard functionality for Apple environment

#elif defined(__linux__)

    //* User using Linux environment
    // TODO: Implement clipboard functionality for Linux environment

#endif
}

std::string LigidGL::getClipboardText() {

#if defined(_WIN32) || defined(_WIN64)

    std::string clipboardText;

    // Open the clipboard
    if (!OpenClipboard(nullptr)) {
        std::cerr << "Failed to open the clipboard." << std::endl;
        return clipboardText;
    }

    // Get the clipboard data handle
    HANDLE hClipboardData = GetClipboardData(CF_TEXT);
    if (hClipboardData == nullptr) {
        std::cerr << "Failed to get clipboard data." << std::endl;
        CloseClipboard();
        return clipboardText;
    }

    // Lock the data and get a pointer to it
    char* pData = static_cast<char*>(GlobalLock(hClipboardData));
    if (pData == nullptr) {
        std::cerr << "Failed to lock clipboard data." << std::endl;
        CloseClipboard();
        return clipboardText;
    }

    // Retrieve the text from the clipboard data
    clipboardText = pData;

    // Unlock the data
    GlobalUnlock(hClipboardData);

    // Close the clipboard
    CloseClipboard();

#elif defined(__APPLE__)

    //* User using Apple environment
    // TODO: Implement clipboard functionality for Apple environment

#elif defined(__linux__)

    //* User using Linux environment
    // TODO: Implement clipboard functionality for Linux environment

#endif
}