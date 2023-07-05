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

#ifndef LIGIDWINDOW_HPP
#define LIGIDWINDOW_HPP 

#include<string>

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

class LigidWindow
{
private:
    #if defined(_WIN32) || defined(_WIN64)

    //* User has Windows
    
    /// @brief The window object for the Windows
    HWND window;

    /// @brief The OpenGL context for the Windows
    HGLRC openGLContext;

    /// @brief The message object for the windows
    MSG msg;

    #elif defined(__APPLE__)

    //User has MacOS

    /// @brief The window object for the MacOS
    NSWindow window; 
    
    /// @brief The OpenGL context for the MacOS
    NSOpenGLContext openGLContext;


    #elif defined(__linux__)

    //User has Linux
    
    /// @brief The window object for the Linux
    Window window; 
    
    /// @brief The OpenGL context for the Linux
    GLXContext openGLContext;

    #endif
public:
    /*! 
    * @brief Default constructor for the LigidWindows
    * Does nothing
    */
    LigidWindow(/* args */){}
    
    /*!
    * Function: CreateOpenGLWindow
    * ----------------------------
    * Creates a new window with an OpenGL rendering context.
    *
    * @param w The width of the window.
    * @param h The height of the window.
    * @param title The title of the window.
    * @return Returns 1 if success  
    * 
    * !!! Creating a window already makes it's OpenGL rendering context current!
    *     Update the OpenGL rendering context after creating a new window ( @ref this->makeContextCurrent function ) if you don't want to
    *     render to the most recent created window class
    */
    int createWindow(int w, int h, std::string title);

    /*!
    *   @brief @return Returns true if window should close.
    *    
    *   @example while(!myWindow.shouldClose()) 
    */
    bool shouldClose();

    /*!
    *   @brief Translates and dispatches a message to the appropriate window procedure for processing.
    *
    *   this->msg contains the message to be processed (for windows) 
    */
    void pollEvents();

    /*!
        @brief Closes the window when called
    */
    void close();

    /*!
        @brief Makes the OpenGL rendering context of the window current rendering context.
        
        Use like that :

        window1.makeContextCurrent();

        Render stuff
        ...

        window2.makeContextCurrent();

        Render stuff
        ...

        !!! Creating a window already makes it's OpenGL rendering context current
    */
    void makeContextCurrent();
};

#endif