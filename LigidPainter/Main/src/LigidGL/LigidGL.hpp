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

#endif

class LigidWindow
{
private:
    #if defined(_WIN32) || defined(_WIN64)

    //User has Windows
    HWND hWnd;
    MSG msg;

    #elif defined(__APPLE__)

    //User has MacOS


    #elif defined(__linux__)

    //User has Linux

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
};

#endif