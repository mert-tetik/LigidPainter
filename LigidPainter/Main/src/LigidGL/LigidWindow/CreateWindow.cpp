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
#include <utility>



#if defined(_WIN32) || defined(_WIN64)
    
    //User has Windows
    #include <Windows.h>

    // Forward declarations for the windows utilities
    LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    std::pair<HWND, HGLRC> WinCreateOpenGLWindow(int width, int height, const char* title);

#elif defined(__APPLE__)


    #include <Cocoa/Cocoa.h>

    // Forward declarations for the MacOS utilities
    std::pair<NSWindow, NSOpenGLContext*> MacOSCreateOpenGLWindow(int width, int height, const char* title);


#elif defined(__linux__)


    //User has Linux
    #include <X11/Xlib.h>
    #include <GL/glx.h>

    // Forward declarations for the Linux utilities
    std::pair<Window, GLXContext> LinuxCreateOpenGLWindow(int width, int height, const char* title);


#endif

/* --Empty function used to init the LigidWindow private callback functions-- */ 
void emptyFunc2int(int, int){}
void emptyFunc3int(int, int, int){}
void emptyFunc2double(double, double){}

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

    //* User using Linux environment

    std::pair<Window, GLXContext> windowContextPair;


    windowContextPair = LinuxCreateOpenGLWindow(
                                                800, // Width 
                                                600, // Height
                                                title.c_str() // Title of the window
                                            );

    this->window = windowContextPair.first;
    
    this->openGLContext = windowContextPair.second;

    if (!this->window) {
        
        // Window creation failed
        std::cout << "Can't create Linux window" << std::endl;
        return 0;
    
    }


#endif
    
    //-- Init the callback functions of the window --

    this->mousePosCallback = emptyFunc2int;
    this->mouseButtonCallback = emptyFunc3int;
    this->keyCallback = emptyFunc3int;
    this->windowSizeCallback = emptyFunc2int;
    this->scrollCallback = emptyFunc2double;

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

std::pair<Window, GLXContext> LinuxCreateOpenGLWindow(int width, int height, const char* title) {
    // Open a connection to the X server
    Display* display = XOpenDisplay(NULL);
    if (display == NULL) {
        return std::make_pair(None, nullptr);
    }

    // Get the default screen
    int screen = DefaultScreen(display);

    // Create a window
    Window root = RootWindow(display, screen);
    XSetWindowAttributes windowAttributes;
    windowAttributes.background_pixel = 0;
    Window window = XCreateWindow(display, root, 0, 0, width, height, 0, CopyFromParent, InputOutput,
                                  CopyFromParent, CWBackPixel, &windowAttributes);
    if (window == None) {
        XCloseDisplay(display);
        return std::make_pair(None, nullptr);
    }

    // Create a visual info
    static int visualAttributes[] = {
        GLX_RGBA,
        GLX_DOUBLEBUFFER,
        GLX_DEPTH_SIZE, 24,
        GLX_STENCIL_SIZE, 8,
        None
    };
    XVisualInfo* visualInfo = glXChooseVisual(display, screen, visualAttributes);
    if (visualInfo == nullptr) {
        XDestroyWindow(display, window);
        XCloseDisplay(display);
        return std::make_pair(None, nullptr);
    }

    // Create a GLX context
    GLXContext context = glXCreateContext(display, visualInfo, nullptr, True);
    if (context == nullptr) {
        XFree(visualInfo);
        XDestroyWindow(display, window);
        XCloseDisplay(display);
        return std::make_pair(None, nullptr);
    }

    // Make the context current
    if (!glXMakeCurrent(display, window, context)) {
        glXDestroyContext(display, context);
        XFree(visualInfo);
        XDestroyWindow(display, window);
        XCloseDisplay(display);
        return std::make_pair(None, nullptr);
    }

    // Set the window title
    XStoreName(display, window, title);

    // Map the window to the screen
    XMapWindow(display, window);
    XFlush(display);

    return std::make_pair(window, context);
}

#endif