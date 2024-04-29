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

#include <GL/GL.h>
#include <GL/GLU.h>
#include <cstdint>
#include <unordered_map>


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
    return this->_get_WindowProcCloseWindow(); 
}

void LigidWindow::setShouldClose(bool val){
    this->_set_WindowProcCloseWindow(val); 
}

static std::unordered_map<int, byte> keyStates;

#define ASSERT_INTO_KEYSTATES(key)  if(this->isKeyPressed(key)){\
                                        if(keyStates[key] == 0)keyStates[key] = 2;\
                                        else if(keyStates[key] == 2)keyStates[key] = 1;\
                                    }\
                                    else{keyStates[key] = 0;}

void LigidWindow::pollEvents(){

    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_A)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_B)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_C)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_D)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_E)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_F)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_G)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_H)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_I)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_J)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_K)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_L)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_M)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_N)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_O)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_P)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_Q)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_R)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_S)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_T)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_U)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_V)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_W)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_X)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_Y)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_Z)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_F1)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_F2)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_F3)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_F4)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_F5)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_F6)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_F7)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_F8)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_F9)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_F10)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_F11)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_F12)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_ENTER)
    ASSERT_INTO_KEYSTATES(LIGIDGL_KEY_ESCAPE)

#if defined(_WIN32) || defined(_WIN64)
    
    //* User in windows environment

    //The PeekMessage function retrieves a message from the message queue without removing it from the queue. Here's a breakdown of the parameters:
    while(PeekMessage(
        &this->msg,   // [in/out] A pointer to an MSG structure that receives the retrieved message.
        this->window,         // [in] A handle to the window whose messages are to be retrieved.
                      //      NULL indicates that the function should retrieve messages for any window that belongs to the current thread.
        0,            // [in] The minimum value for the message filter.
                      //      Specifies the value of the first message to be retrieved.
                      //      In this case, 0 indicates that all message types should be retrieved.
        0,            // [in] The maximum value for the message filter.
                      //      Specifies the value of the last message to be retrieved.
                      //      In this case, 0 indicates that all message types should be retrieved.
        PM_REMOVE | PM_NOREMOVE     // [in] The removal options.
                      //                Specifies how the message should be handled after retrieval.
                      //                PM_REMOVE indicates that the retrieved message should be removed from the queue.
                  ))
    {

        //UpdateWindow(this->window);

        /*
            Send the callback functions to the where main window callback function belongs (CreateWindow.cpp)
        */
        this->_setProcFunctions(
                                    *this, 
                                    this->mousePosCallback, 
                                    this->mouseButtonCallback, 
                                    this->keyCallback, 
                                    this->windowSizeCallback, 
                                    this->scrollCallback
                                );

        /*
            The TranslateMessage function translates virtual-key messages into character messages.
            It is typically called to process keyboard input before dispatching it to the appropriate window procedure.
            This function is necessary for translating virtual-key messages (such as WM_KEYDOWN and WM_KEYUP)
            into character messages (such as WM_CHAR) for proper handling of keyboard input.
        */
        TranslateMessage(&this->msg);

        /*
         The DispatchMessage function dispatches a message to a window procedure.
         It sends the message to the window procedure that the message's hWnd and message type specify.
         The window procedure handles the message and performs appropriate actions based on the message type.
        */
        DispatchMessage(&this->msg);
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

void LigidWindow::show(){
    ShowWindow(this->window, SW_SHOWDEFAULT);
    UpdateWindow(this->window);
}

void LigidWindow::hide(){
    ShowWindow(this->window, SW_HIDE);
    UpdateWindow(this->window);
}

void LigidWindow::minimize(){
    ShowWindow(this->window, SW_MINIMIZE);
    UpdateWindow(this->window);
}

bool LigidWindow::isMinimized(){
    return IsIconic(this->window) != 0;
}

void LigidWindow::maximize(){
    ShowWindow(this->window, SW_MAXIMIZE);
    UpdateWindow(this->window);
}

bool LigidWindow::isContextCurrent(){
    HGLRC currentContext = wglGetCurrentContext();
    if (currentContext == nullptr) 
    {
        return false;
    } 
    else 
    {
        return currentContext == this->openGLContext; 
    }
}

bool LigidWindow::shareContext(LigidWindow another_context){
    return wglShareLists(this->openGLContext, another_context.openGLContext);
}

bool LigidWindow::makeContextCurrent(){

#if defined(_WIN32) || defined(_WIN64)
    
    //* User in windows environment

    HDC hdc = GetDC(this->window);    // Obtain the device context handle for the window

    // Release the previous context
    wglMakeCurrent(NULL, NULL);

    // Make the created context current for the given device context
    if(!wglMakeCurrent(hdc, this->openGLContext)){
        std::cout << "Window OpenGL context - failed to make current" << std::endl;
        return false;
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
        LGDLOG::start<< "ERROR : Can't make the context current!" << LGDLOG::end;
    }

#endif

    return true;
}
void LigidWindow::releaseContext(){
#if defined(_WIN32) || defined(_WIN64)
    // Release the previous context
    wglMakeCurrent(NULL, NULL);

#elif(__APPLE__)

#elif(__linux__)

#endif

}

bool LigidWindow::isKeyPressed(int key){
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


bool LigidWindow::isKeyClicked(int key) {
    if(!keyStates.count(key)){
        LGDLOG::start << "ERROR : LigidWindow - Invalid key for the click check!" << LGDLOG::end;
        return false;
    }

    return keyStates[key] == 2;
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
        std::cerr << "Failed to change window size." << LGDLOG::end;
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
        std::cerr << "Failed to get window position." << LGDLOG::end;
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
        std::cerr << "Failed to change the window position." << LGDLOG::end;
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

    /* Don't change the cursor if cursor hovers the title bar or the resizing area */
    if(!this->isCursorOnTitleBar() && !this->isCursorOnResizingArea()){

        SetCursor(cursor.getCursorHandle());

        SetClassLongPtr(this->window, GCLP_HCURSOR, reinterpret_cast<LONG_PTR>(cursor.getCursorHandle()));
    }

#elif(__APPLE__)

    //* User in MacOS environment
    
#elif(__linux__)

    //* User in Linux environment

#endif
}

void LigidWindow::setCursorVisibility(bool visible) {
    
    if (visible) {
        while (ShowCursor(TRUE) < 0) {}
    }
    else {
        while (ShowCursor(FALSE) >= 0) {}
    }

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

void* LigidWindow::getWindowUserPointer()
{
    LONG_PTR userPointer = GetWindowLongPtr(this->window, GWLP_USERDATA);
    return reinterpret_cast<void*>(userPointer);
}

void LigidWindow::setWindowUserPointer(void* pointer){
    SetWindowLongPtr(this->window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pointer));
}

bool LigidWindow::isCursorOnTitleBar(){
    POINT cursorPos;
    RECT windowRect;
    RECT clientRect;

    // Get the cursor position in screen coordinates
    GetCursorPos(&cursorPos);

    // Convert the cursor position to client coordinates
    ScreenToClient(this->window, &cursorPos);

    // Get the window and client area rectangles
    GetWindowRect(this->window, &windowRect);
    GetClientRect(this->window, &clientRect);

    // Calculate the height of the non-client area (including the title bar)
    int nonClientHeight = (windowRect.bottom - windowRect.top) - (clientRect.bottom - clientRect.top);

    // Check if the cursor Y-coordinate is within the non-client area height
    return cursorPos.y < nonClientHeight;
}

bool LigidWindow::isCursorOnResizingArea() {
    // Get the screen coordinates of the cursor
    POINT ptCursor;
    GetCursorPos(&ptCursor);

    // Get the window rectangle
    RECT rcWindow;
    GetWindowRect(this->window, &rcWindow);

    // Calculate the resizing areas (you can adjust the values as needed)
    int nResizeAreaSize = 8;
    RECT rcBottomRight = {
        rcWindow.right - nResizeAreaSize,
        rcWindow.bottom - nResizeAreaSize,
        rcWindow.right,
        rcWindow.bottom
    };

    RECT rcBottom = {
        rcWindow.left + nResizeAreaSize,
        rcWindow.bottom - nResizeAreaSize,
        rcWindow.right - nResizeAreaSize,
        rcWindow.bottom
    };

    RECT rcRight = {
        rcWindow.right - nResizeAreaSize,
        rcWindow.top + nResizeAreaSize,
        rcWindow.right,
        rcWindow.bottom - nResizeAreaSize
    };

    RECT rcLeft = {
        rcWindow.left,
        rcWindow.top + nResizeAreaSize,
        rcWindow.left + nResizeAreaSize,
        rcWindow.bottom - nResizeAreaSize
    };

    // Check if the cursor is within any of the resizing areas
    return PtInRect(&rcBottomRight, ptCursor) || PtInRect(&rcBottom, ptCursor) || PtInRect(&rcRight, ptCursor) || PtInRect(&rcLeft, ptCursor);
}

bool LigidWindow::setWindowIcon(const wchar_t* iconPath) {
    HICON hIcon = (HICON)LoadImage(NULL, iconPath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    if (!hIcon) {
        // Failed to load the icon file
        return false;
    }

    // Send the WM_SETICON message to set the icon for both big and small icons
    SendMessage(this->window, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    SendMessage(this->window, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

    // The window procedure will manage the icon's lifetime,
    // so we can release our local reference to the icon.
    //DestroyIcon(hIcon);

    return true;
}

void LigidWindow::deleteContext(){
    // Release the context
    wglMakeCurrent(NULL, NULL);
    // Delete the context
    wglDeleteContext(this->openGLContext);
    // Release the device
    ReleaseDC(this->window, GetDC(this->window));
}

void LigidWindow::getMaximizedScreenSize(int& scaleX, int& scaleY) {
    RECT windowRect, clientRect;
    GetWindowRect(this->window, &windowRect);
    GetClientRect(this->window, &clientRect);

    // Calculate the non-client area (title bar, borders, etc.)
    int titleBarHeight = GetSystemMetrics(SM_CYCAPTION); // Title bar height
    int borderX = (windowRect.right - windowRect.left) - clientRect.right;
    int borderY = (windowRect.bottom - windowRect.top) - clientRect.bottom - titleBarHeight; // Adjust for title bar

    // Get the monitor info
    HMONITOR hMonitor = MonitorFromWindow(this->window, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
    GetMonitorInfo(hMonitor, &monitorInfo);

    // Adjust for taskbar
    RECT maximizedRect = monitorInfo.rcWork;
    maximizedRect.right -= borderX;
    maximizedRect.bottom -= borderY;

    scaleX = maximizedRect.right - maximizedRect.left;
    scaleY = maximizedRect.bottom - maximizedRect.top;
}