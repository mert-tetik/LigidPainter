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

#include <GL/GL.h>
#include <GL/GLU.h>


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


void LigidWindow::swapBuffers(){
    
    HDC hDC = GetDC(this->window);

    if (hDC != nullptr) {
        wglSwapLayerBuffers(hDC, WGL_SWAP_MAIN_PLANE);
        ReleaseDC(this->window, hDC);
    }
}


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

        Returns false if there are no messages which means user is trying to close the window (getMessage function returned -1)
    */
    return GetMessage(
                        &this->msg, 
                        this->window, 
                        0, 
                        0
                    ) == -1;
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


    // If received a key message
    if( msg.message == WM_KEYDOWN || msg.message == WM_KEYFIRST || msg.message == WM_KEYUP)
    {
        
        // The key code
        int keyCode = static_cast<int>(msg.wParam);
        
        // The action
        int action = 0;
        if(msg.message == WM_KEYFIRST)
            action = LIGIDGL_PRESS;
        if(msg.message == WM_KEYDOWN)
            action = LIGIDGL_REPEAT;
        if(msg.message == WM_KEYUP)
            action = LIGIDGL_RELEASE;

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
            

        // Call the key callback function set by the user using message data
        this->keyCallback(
                                keyCode, //Received mouse x pos  
                                action,  //Received mouse y pos
                                mods
                            );

    }

    // Check if the window size has changed
    if (msg.message == WM_SIZE) {
        
        this->windowSizeCallback(LOWORD(msg.lParam), HIWORD(msg.lParam));

    }

    // Check if mouse/touchpad scrolled horizontally
    if (msg.message == WM_MOUSEHWHEEL) {
        double delta = GET_WHEEL_DELTA_WPARAM(msg.wParam);

        this->scrollCallback(delta, 0);
    } 
    
    // Check if mouse scrolled vertically
    else if (msg.message == WM_MOUSEWHEEL) {
        double delta = GET_WHEEL_DELTA_WPARAM(msg.wParam);

        this->scrollCallback(0, delta);
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

    // activate the OpenGL rendering context

    // Make the created context current for the given device context
    if(!wglMakeCurrent(hdc, this->openGLContext)){
        std::cout << "Window OpenGL context - failed to make current" << std::endl;
    }       
    


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
    HWND foregroundWindow = GetForegroundWindow();
    if(foregroundWindow == this->window){
        return GetAsyncKeyState(key) & 0x8000;
    }
    else{
        return false;
    }

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

void LigidWindow::getWindowSize(
                                    int& w, //Width of the window 
                                    int& h  //Height of the window
                                )
{
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

void LigidWindow::setWindowSize(
                                    const int w, //Width of the window 
                                    const int h //Height of the window
                                )
{
#if defined(_WIN32) || defined(_WIN64)
    //* User in Windows environment
    
    // Calculate the new window position
    int originalX = CW_USEDEFAULT; // Use default X position
    int originalY = CW_USEDEFAULT; // Use default Y position

    if (!SetWindowPos(this->window, nullptr, originalX, originalY, w, h, SWP_NOMOVE | SWP_NOZORDER)) {
        std::cerr << "Failed to change window size." << std::endl;
    }

#elif(__APPLE__)

    //* User in MacOS environment
    
#elif(__linux__)

    //* User in Linux environment

#endif
}

void LigidWindow::getFramebufferSize(
                                    int& w, //Width of the framebuffer 
                                    int& h  //Height of the framebuffer
                                )
{
#if defined(_WIN32) || defined(_WIN64)
    
    if (this->window == nullptr || this->openGLContext == nullptr) {
        w = 0;
        w = 0;
        return;
    }

    // Get the device context (DC) associated with the HWND
    HDC hDC = GetDC(this->window);

    // Get the framebuffer size
    w = GetDeviceCaps(hDC, HORZRES);
    h = GetDeviceCaps(hDC, VERTRES);

#elif(__APPLE__)

    //* User in MacOS environment
    
#elif(__linux__)

    //* User in Linux environment

#endif
}

void LigidWindow::getWindowPos(
                                int& x, //Position of the window in the x axis 
                                int& y  //Position of the window in the y axis
                            )
{
#if defined(_WIN32) || defined(_WIN64)
    //* User in Windows environment
    
    // Get the window position
    RECT windowRect;
    if (!GetWindowRect(this->window, &windowRect)) {
        std::cerr << "Failed to get window position." << std::endl;
    }

    // Write the position data to the reference parameters
    x = windowRect.left;
    y = windowRect.top;

#elif(__APPLE__)

    //* User in MacOS environment
    
#elif(__linux__)

    //* User in Linux environment

#endif
}

void LigidWindow::setWindowPos(
                                const int x, //Position of the window in the x axis 
                                const int y  //Position of the window in the y axis
                            )
{
#if defined(_WIN32) || defined(_WIN64)
    //* User in Windows environment
    
    // Calculate the new window size
    int originalW = CW_USEDEFAULT; // Use default width
    int originalH = CW_USEDEFAULT; // Use default height

    if (!SetWindowPos(this->window, nullptr, x, y, originalW, originalH, SWP_NOSIZE | SWP_NOZORDER)) {
        std::cerr << "Failed to change the window position." << std::endl;
    }

#elif(__APPLE__)

    //* User in MacOS environment
    
#elif(__linux__)

    //* User in Linux environment

#endif
}

void LigidWindow::setCursor(LigidCursor cursor){
#if defined(_WIN32) || defined(_WIN64)
    //* User in Windows environment
    
    SetCursor(cursor.getCursorHandle());
    SetClassLongPtr(this->window, GCLP_HCURSOR, reinterpret_cast<LONG_PTR>(cursor.getCursorHandle()));

#elif(__APPLE__)

    //* User in MacOS environment
    
#elif(__linux__)

    //* User in Linux environment

#endif
}

void LigidWindow::style(const int styleKey){
    
    int64_t windowStyle = WS_CAPTION;
    
    //Default
    if(styleKey == 0)
        windowStyle = WS_OVERLAPPEDWINDOW;
    
    //Borderless
    if(styleKey == 1)
        windowStyle = WS_POPUP;
    
    //Caption
    if(styleKey == 2)
        windowStyle = WS_CAPTION;
    
    //Full screen
    if(styleKey == 3)
        windowStyle = WS_POPUP;
    

    SetWindowLongPtr(this->window, GWL_STYLE, windowStyle); //3d argument=style
    
    //Full screen
    if(styleKey == 3)
        SetWindowPos(this->window, nullptr, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);
    else
        SetWindowPos(this->window, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE); 
}