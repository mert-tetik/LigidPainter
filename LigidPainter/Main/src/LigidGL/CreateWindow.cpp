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

    // Forward declarations for the windows utilities
    LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    std::pair<HWND, HGLRC> WinCreateOpenGLWindow(int width, int height, const char* title);

#elif defined(__APPLE__)


    #include <Cocoa/Cocoa.h>
    #include <OpenGL/OpenGL.h>
    #include <OpenGL/gl3.h>

    // Forward declarations for the MacOS utilities
    std::pair<NSWindow, NSOpenGLContext*> MacOSCreateOpenGLWindow(int width, int height, const char* title);


#elif defined(__linux__)


    //User has Linux
    #include <X11/Xlib.h>

#endif



int LigidWindow::createWindow(
                                int w, 
                                int h, 
                                std::string title
                            )
{
#if defined(_WIN32) || defined(_WIN64)

    //* User using Windows environment

    std::pair<HWND, HGLRC> windowContextPair;


    windowContextPair = WinCreateOpenGLWindow(
                                                800, // Width 
                                                600, // Height
                                                title.c_str() // Title of the window
                                            );
    this->window = windowContextPair.first;
    
    this->openGLContext = windowContextPair.second;

    if (!this->window) {
        
        // Window creation failed
        std::cout << "Can't create HWND window" << std::endl;
        return 0;
    
    }

#elif defined(__APPLE__)

    //* User using Apple environment


    std::pair<NSWindow, NSOpenGLContext*> windowContextPair;

    windowContextPair = MacOSCreateOpenGLWindow(
                                                800, // Width 
                                                600, // Height
                                                title.c_str() // Title of the window
                                            );
    this->window = windowContextPair.first;
    
    this->openGLContext = windowContextPair.second;

    if (!this->window) {
        
        // Window creation failed
        std::cout << "Can't create cocoa window" << std::endl;
        return 0;
    
    }


#elif defined(__linux__)



#endif
    
    return 1;
}















#if defined(_WIN32) || defined(_WIN64)

//  --------------------    WINDOWS UTIL    --------------------

std::pair<HWND, HGLRC> WinCreateOpenGLWindow(int width, int height, const char* title) {
    // Get the instance handle of the current module
    HINSTANCE hInstance = GetModuleHandle(NULL);

    // Register the window class
    WNDCLASS wc = {0};
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"OpenGLWindowClass";

    // Class registration failed
    if (!RegisterClass(&wc)) {
        return std::make_pair(HWND(NULL), HGLRC(NULL));
    }

    // Adjust the window size to include the window frame
    RECT windowRect = {0, 0, width, height};
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    // Create the window
    HWND hWnd = CreateWindow(L"OpenGLWindowClass", L"OpenGL Window", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
        NULL, NULL, hInstance, NULL);

    // Window creation failed
    if (!hWnd) 
    {
        return std::make_pair(HWND(NULL), HGLRC(NULL));
    }

    // Get the device context
    HDC hdc = GetDC(hWnd);

    // Set the pixel format
    PIXELFORMATDESCRIPTOR pfd = {0};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixelFormat, &pfd);

    // Create and activate the OpenGL rendering context
    HGLRC hRC = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hRC);

    // Show and update the window
    ShowWindow(hWnd, SW_SHOWDEFAULT);
    UpdateWindow(hWnd);

    return std::make_pair(hWnd, hRC);
;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

#elif defined(__APPLE__)

//  --------------------    MACOS UTIL    --------------------

std::pair<NSWindow, NSOpenGLContext*> MacOSCreateOpenGLWindow(int width, int height, const char* title) {
    //TODO : Complete that
}

#elif defined(__linux__)

//  --------------------    LINUX UTIL    --------------------


#endif