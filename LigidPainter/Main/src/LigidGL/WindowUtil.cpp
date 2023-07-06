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


bool LigidWindow::shouldClose(){

#if defined(_WIN32) || defined(_WIN64)
    
    //* User in windows environment
    
    /* 
        The GetMessage function retrieves a message from the calling thread's message queue.
        If there are no messages, it blocks until a message arrives or the application is terminated.
        Here, we are checking if GetMessage returns 0, which indicates there are no more messages in the queue.
        The message structure (msg) is filled with the message details if GetMessage retrieves a message.
        The hWnd parameter specifies the handle of the window whose messages are to be retrieved.
        The next two parameters (wMsgFilterMin and wMsgFilterMax) filter the messages to be retrieved.
        In this case, they are set to 0, which means all messages are retrieved.

        Returns false if there are no messages which means user is trying to close the window
    */
    return !GetMessage(
                        &this->msg, 
                        this->window, 
                        0, 
                        0
                    );
#endif

}


void LigidWindow::pollEvents(){
#if defined(_WIN32) || defined(_WIN64)
    
    //* User in windows environment

    // The TranslateMessage function translates virtual-key messages into character messages.
    // It is typically called to process keyboard input before dispatching it to the appropriate window procedure.
    // This function is necessary for translating virtual-key messages (such as WM_KEYDOWN and WM_KEYUP)
    // into character messages (such as WM_CHAR) for proper handling of keyboard input.
    TranslateMessage(&this->msg);

    // The DispatchMessage function dispatches a message to a window procedure.
    // It sends the message to the window procedure that the message's hWnd and message type specify.
    // The window procedure handles the message and performs appropriate actions based on the message type.
    DispatchMessage(&this->msg);


    //--- Call the callback functions ---

    // If received a mouse position change message
    if(this->msg.message == WM_MOUSEMOVE){

        // Call the mouse position callback function set by the user using message data
        this->mousePosCallback(
                                LOWORD(msg.lParam), //Received mouse x pos  
                                HIWORD(msg.lParam)  //Received mouse y pos
                            );
    
    }



    // Check if any message is received regarding the left mouse button
    bool LButtonReceived = this->msg.message == WM_LBUTTONDOWN || this->msg.message == WM_LBUTTONDBLCLK || this->msg.message == WM_LBUTTONUP;
    
    // Check if any message is received regarding the right mouse button
    bool RButtonReceived = this->msg.message == WM_RBUTTONDOWN || this->msg.message == WM_RBUTTONDBLCLK || this->msg.message == WM_RBUTTONUP; 
    
    // Check if any message is received regarding the middle mouse button
    bool MButtonReceived = this->msg.message == WM_MBUTTONDOWN || this->msg.message == WM_MBUTTONDBLCLK || this->msg.message == WM_MBUTTONUP; 

    // If received a mouse button message
    if(
            LButtonReceived ||  //If left button action received 
            RButtonReceived ||  //If right button action received
            MButtonReceived     //If middle button action received
        )
    {

        // Set the button
        int button = 0;
        if(LButtonReceived)
            button = 0;
        if(RButtonReceived)
            button = 1;
        if(MButtonReceived)
            button = 2;
        
        // If pressed to the button
        int action = this->msg.message == WM_LBUTTONDOWN || this->msg.message == WM_RBUTTONDOWN || this->msg.message ==WM_MBUTTONDOWN;

        // Mods
        int mods = LIGIDGL_MOD_DEFAULT;
        if(this->isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) == LIGIDGL_PRESS && this->isKeyPressed(LIGIDGL_KEY_LEFT_ALT) == LIGIDGL_RELEASE && this->isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) == LIGIDGL_RELEASE)
            mods = LIGIDGL_MOD_CONTROL;
        else if(this->isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) == LIGIDGL_RELEASE && this->isKeyPressed(LIGIDGL_KEY_LEFT_ALT) == LIGIDGL_PRESS && this->isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) == LIGIDGL_RELEASE)
            mods = LIGIDGL_MOD_ALT;
        else if(this->isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) == LIGIDGL_RELEASE && this->isKeyPressed(LIGIDGL_KEY_LEFT_ALT) == LIGIDGL_RELEASE && this->isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) == LIGIDGL_PRESS)
            mods = LIGIDGL_MOD_SHIFT;
        else if(this->isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) == LIGIDGL_PRESS && this->isKeyPressed(LIGIDGL_KEY_LEFT_ALT) == LIGIDGL_PRESS && this->isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) == LIGIDGL_RELEASE)
            mods = LIGIDGL_MOD_CONTROL_ALT;
        else if(this->isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) == LIGIDGL_PRESS && this->isKeyPressed(LIGIDGL_KEY_LEFT_ALT) == LIGIDGL_RELEASE && this->isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) == LIGIDGL_PRESS)
            mods = LIGIDGL_MOD_CONTROL_SHIFT;
        else if(this->isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) == LIGIDGL_PRESS && this->isKeyPressed(LIGIDGL_KEY_LEFT_ALT) == LIGIDGL_PRESS && this->isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) == LIGIDGL_PRESS)
            mods = LIGIDGL_MOD_CONTROL_ALT_SHIFT;
            

        // Call the mouse button callback function set by the user using message data
        this->mouseButtonCallback(
                                button, //Received mouse x pos  
                                action,  //Received mouse y pos
                                mods
                            );
    
    }

#endif
}


void LigidWindow::close(){
#if defined(_WIN32) || defined(_WIN64)

    //* User in windows environment
    
    // Closes / destroys the hwnd window
    DestroyWindow(this->window);
    
#endif
} 


void LigidWindow::makeContextCurrent(){

#if defined(_WIN32) || defined(_WIN64)
    
    //* User in windows environment

    HDC hdc = GetDC(this->window);    // Obtain the device context handle for the window

    // Create and activate the OpenGL rendering context
    this->openGLContext = wglCreateContext(hdc);    // Create an OpenGL rendering context
    wglMakeCurrent(hdc, this->openGLContext);       // Make the created context current for the given device context


#elif(__APPLE__)


    //* User in MacOS environment
    
    // Make the OpenGL context current
    this->openGLContext->makeCurrentContext();


#elif(__linux__)


    //TODO Ask for a existing display 

    // Open the X11 display connection
    Display* display = XOpenDisplay(NULL);
    if (display == NULL) {
        // Failed to open X11 display
        // Handle the error here
    }

    // Get the default screen
    int screen = DefaultScreen(display);

    // Make the GLX context current
    if (!glXMakeCurrent(display, this->window, this->glxContext)) {
        // Failed to make context current
        std::cout << "ERROR : Can't make the context current!" << std::endl;
    }

#endif

}

bool LigidWindow::isKeyPressed(char key){
#if defined(_WIN32) || defined(_WIN64)
    
    //* User in windows environment
    
    // Returns true if the key is pressed
    return GetAsyncKeyState(key) & 0x8000;

#elif(__APPLE__)

    //* User in MacOS environment
    
#elif(__linux__)

    //* User in Linux environment

#endif

}

bool LigidWindow::isMouseButtonPressed(int button){
#if defined(_WIN32) || defined(_WIN64)
    
    //* User in windows environment
    
    // Returns true if the button is pressed

    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 && button == LIGIDGL_MOUSE_BUTTON_LEFT) {
        return true;
    }

    if (GetAsyncKeyState(VK_RBUTTON) & 0x8000 && button == LIGIDGL_MOUSE_BUTTON_RIGHT) {
        return true;
    }

    if (GetAsyncKeyState(VK_MBUTTON) & 0x8000 && button == LIGIDGL_MOUSE_BUTTON_MIDDLE) {
        return true;
    }

    return false;

#elif(__APPLE__)

    //* User in MacOS environment
    
#elif(__linux__)

    //* User in Linux environment

#endif

}

void LigidWindow::getWindowSize(int& w, int& h){
#if defined(_WIN32) || defined(_WIN64)
    
    RECT windowRect;
    GetWindowRect(this->window, &windowRect);

    w = windowRect.right - windowRect.left;
    h = windowRect.bottom - windowRect.top;

#elif(__APPLE__)

    //* User in MacOS environment
    
#elif(__linux__)

    //* User in Linux environment

#endif
}

void LigidWindow::setWindowSize(const int w, const int h){
#if defined(_WIN32) || defined(_WIN64)
    //* User in Windows environment
    
    // Calculate the new window position
    int newX = CW_USEDEFAULT; // Use default X position
    int newY = CW_USEDEFAULT; // Use default Y position

    if (!SetWindowPos(this->window, nullptr, newX, newY, w, h, SWP_NOMOVE | SWP_NOZORDER)) {
        std::cerr << "Failed to change window size." << std::endl;
    }

#elif(__APPLE__)

    //* User in MacOS environment
    
#elif(__linux__)

    //* User in Linux environment

#endif
}
