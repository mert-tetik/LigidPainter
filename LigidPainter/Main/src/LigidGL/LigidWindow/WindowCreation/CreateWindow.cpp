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
#include "GUI/GUI.hpp"
#include <utility>

#include <gl/GL.h>
#include <stdbool.h>


#if defined(_WIN32) || defined(_WIN64)
    
    //User has Windows
    #include <Windows.h>

    // Forward declarations for the window creation utilities from CreateWindowWin.cpp
    HGLRC init_opengl(HDC real_dc);
    HWND create_window(int width, int height, const wchar_t* title);

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
void emptyFunc2int(LigidWindow,int, int){}
void emptyFunc3int(LigidWindow,int, int, int){}
void emptyFunc2double(LigidWindow,double, double){}

int LigidWindow::createWindow(
                                int w, 
                                int h, 
                                const wchar_t* title
                            )
{
#if defined(_WIN32) || defined(_WIN64)

    //* User using Windows environment

    // Create the window
    this->window = create_window(w, h, title);
    
    // If window creation is failed
    if (!this->window) {
        
        // Window creation failed
        LGDLOG::start<< "Can't create HWND window" << LGDLOG::end;
        return 0;
    
    }

    // Get the device handle
    HDC gldc = GetDC(this->window);
    
    // Get the OpenGl context
    this->openGLContext = init_opengl(gldc);

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
        LGDLOG::start<< "Can't create cocoa window" << LGDLOG::end;
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
        LGDLOG::start<< "Can't create Linux window" << LGDLOG::end;
        return 0;
    
    }


#endif
    
    //-- Init the callback functions of the window --

    this->mousePosCallback = emptyFunc2double;
    this->mouseButtonCallback = emptyFunc3int;
    this->keyCallback = emptyFunc3int;
    this->windowSizeCallback = emptyFunc2int;
    this->scrollCallback = emptyFunc2double;

    return 1;
}






#if defined(__APPLE__)

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