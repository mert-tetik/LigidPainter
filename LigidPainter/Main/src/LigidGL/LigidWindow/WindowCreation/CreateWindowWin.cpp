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
#include <cstdint>

#include <Windows.h>
#include <GL/wglext.h>

//  --------------------    WGL    --------------------

// Later that function is loaded in the init_opengl_extensions function
typedef HGLRC WINAPI wglCreateContextAttribsARB_type(
                                                        HDC hdc, 
                                                        HGLRC hShareContext, 
                                                        const int *attribList
                                                    );
wglCreateContextAttribsARB_type *wglCreateContextAttribsARB;

// See https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_create_context.txt for all values
#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

// Later that function is loaded in the init_opengl_extensions function
typedef BOOL WINAPI wglChoosePixelFormatARB_type(
                                                    HDC hdc, 
                                                    const int *piAttribIList, 
                                                    const FLOAT *pfAttribFList, 
                                                    UINT nMaxFormats, 
                                                    int *piFormats, 
                                                    UINT *nNumFormats
                                                );

wglChoosePixelFormatARB_type *wglChoosePixelFormatARB;

// See https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_pixel_format.txt for all values
#define WGL_DRAW_TO_WINDOW_ARB                    0x2001
#define WGL_ACCELERATION_ARB                      0x2003
#define WGL_SUPPORT_OPENGL_ARB                    0x2010
#define WGL_DOUBLE_BUFFER_ARB                     0x2011
#define WGL_PIXEL_TYPE_ARB                        0x2013
#define WGL_COLOR_BITS_ARB                        0x2014
#define WGL_DEPTH_BITS_ARB                        0x2022
#define WGL_STENCIL_BITS_ARB                      0x2023

#define WGL_FULL_ACCELERATION_ARB                 0x2027
#define WGL_TYPE_RGBA_ARB                         0x202B



//  --------------------    WINDOW & OPENGL CONTEXT CREATION    --------------------

static void fatal_error(char *uMsg)
{
    LGDLOG::start<< "ERROR : " << uMsg << LGDLOG::end;
    //exit(EXIT_FAILURE);
}

int __global_LGDGL_dummyWindowCounter = 0;

/* 
    Loads the WGL functions 
*/
static void init_opengl_extensions(void)
{
    __global_LGDGL_dummyWindowCounter++;

    // Before we can load extensions, we need a dummy OpenGL context, created using a dummy window.
    // We use a dummy window because you can only set the pixel format for a window once. For the
    // real window, we want to use wglChoosePixelFormatARB (so we can potentially specify options
    // that aren't available in PIXELFORMATDESCRIPTOR), but we can't load and use that before we
    // have a context.
    // Register the window class
    
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASSA wc = {0};
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = hInstance;
    std::string dummyWindowClassName = "DummyWindow" + std::to_string(__global_LGDGL_dummyWindowCounter); 
    wc.lpszClassName = dummyWindowClassName.c_str();

    if (!RegisterClassA(&wc)) {
        fatal_error("Failed to register dummy OpenGL window.");
    }

    HWND dummy_window = CreateWindowExA(
        0,
        wc.lpszClassName,
        "Dummy OpenGL Window",
        0,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        0,
        0,
        wc.hInstance,
        0);

    if (!dummy_window) {
        fatal_error("Failed to create dummy OpenGL window.");
    }

    HDC dummy_dc = GetDC(dummy_window);

    PIXELFORMATDESCRIPTOR pfd;
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.cColorBits = 24;
    pfd.cAlphaBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;
    pfd.cDepthBits = 8;
    pfd.cStencilBits = 8;

    int pixel_format = ChoosePixelFormat(dummy_dc, &pfd);
    if (!pixel_format) {
        fatal_error("Failed to find a suitable pixel format.");
    }
    if (!SetPixelFormat(dummy_dc, pixel_format, &pfd)) {
        fatal_error("Failed to set the pixel format.");
    }

    HGLRC dummy_context = wglCreateContext(dummy_dc);
    if (!dummy_context) {
        fatal_error("Failed to create a dummy OpenGL rendering context.");
    }

    if (!wglMakeCurrent(dummy_dc, dummy_context)) {
        fatal_error("Failed to activate dummy OpenGL rendering context.");
    }

    wglCreateContextAttribsARB = (wglCreateContextAttribsARB_type*)wglGetProcAddress(
        "wglCreateContextAttribsARB");
    wglChoosePixelFormatARB = (wglChoosePixelFormatARB_type*)wglGetProcAddress(
        "wglChoosePixelFormatARB");

    wglMakeCurrent(dummy_dc, 0);
    wglDeleteContext(dummy_context);
    ReleaseDC(dummy_window, dummy_dc);
    DestroyWindow(dummy_window);
}

/*
    Create OpenGL rendering context handle
*/
HGLRC init_opengl(HDC real_dc)
{
    init_opengl_extensions();

    // Now we can choose a pixel format the modern way, using wglChoosePixelFormatARB.
    int pixel_format_attribs[] = {
        WGL_DRAW_TO_WINDOW_ARB,     GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB,     GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB,      GL_TRUE,
        WGL_ACCELERATION_ARB,       WGL_FULL_ACCELERATION_ARB,
        WGL_PIXEL_TYPE_ARB,         WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB,         24,
        WGL_ALPHA_BITS_ARB,         8,   // Request alpha bits
        WGL_DEPTH_BITS_ARB,         8,
        WGL_STENCIL_BITS_ARB,       8,
        //WGL_SAMPLE_BUFFERS_ARB,     1,
        //WGL_SAMPLES_ARB,            1,
        0
    };


    int pixel_format;
    UINT num_formats;
    
    if(!wglChoosePixelFormatARB(real_dc, pixel_format_attribs, 0, 1, &pixel_format, &num_formats)){
        fatal_error("Failed to choose pixel format.");
    }
    
    if (!num_formats) {
        fatal_error("Failed to set the OpenGL 3.3 pixel format.");
    }

    PIXELFORMATDESCRIPTOR pfd;
    DescribePixelFormat(real_dc, pixel_format, sizeof(pfd), &pfd);
    if (!SetPixelFormat(real_dc, pixel_format, &pfd)) {
        fatal_error("Failed to set the OpenGL 3.3 pixel format.");
    }

    int64_t glVersionProfile = 0;
    
    if(LIGIDGL_OPENGL_VERSION_PROFILE == LIGIDGL_OPENGL_PROFILE_CORE)
        glVersionProfile = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
    else if(LIGIDGL_OPENGL_VERSION_PROFILE == LIGIDGL_OPENGL_PROFILE_COMPATIBILITY)
        glVersionProfile = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
    else
        fatal_error("Invalid OpenGL profile! See the LIGIDGL_OPENGL_VERSION_PROFILE macro in the LigidGL header");

    // Specify that we want to create an OpenGL 3.3 core profile context
    int gl33_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, LIGIDGL_OPENGL_VERSION_MAJOR,
        WGL_CONTEXT_MINOR_VERSION_ARB, LIGIDGL_OPENGL_VERSION_MINOR,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
    };

    HGLRC gl33_context = wglCreateContextAttribsARB(real_dc, 0, gl33_attribs);
    if (!gl33_context) {
        LGDLOG::start<< "Failed to create OpenGL 4.0 context. Your GPU might not be compatible with the OpenGL version 4.0" << LGDLOG::end;
        LGDLOG::start<< "Trying to create the OpenGL context without the attributes." << LGDLOG::end;
    
        gl33_context = wglCreateContext(real_dc);

        if(!gl33_context)
            fatal_error(
                        R"(
                        Failed to create the OpenGL context without the attributes. 
                        LigidPainter uses the higher versions of the OpenGL specification. 
                        Upgrading your GPU might solve the problem.
                        )"
                    );
    }

    if (!wglMakeCurrent(real_dc, gl33_context)) {
        fatal_error("Failed to activate OpenGL 4.0 rendering context.");
    }

    return gl33_context;
}

int __global_LGDGL_windowCounter = 0;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
/*
    HWND Window handle creation
*/
HWND create_window(int width, int height, const wchar_t* title)
{
    __global_LGDGL_windowCounter++;

    // Get the instance handle of the current module
    HINSTANCE hInstance = GetModuleHandle(NULL);

    // Register the window class
    WNDCLASS wc = {0};
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    std::wstring className = L"OpenGLWindowClass" + std::to_wstring(__global_LGDGL_windowCounter); 
    wc.lpszClassName = className.c_str();

    // Class registration failed
    if (!RegisterClass(&wc)) {
        fatal_error("Class registration failed");
        return nullptr;
    }

    // Adjust the window size to include the window frame
    RECT windowRect = {0, 0, width, height};
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    // Create the window
    HWND hWnd = CreateWindow(
                                className.c_str(), 
                                title, 
                                WS_OVERLAPPEDWINDOW,
                                CW_USEDEFAULT, 
                                CW_USEDEFAULT, 
                                windowRect.right - windowRect.left, 
                                windowRect.bottom - windowRect.top,
                                NULL, 
                                NULL, 
                                hInstance, 
                                NULL
                            );

    // Window creation failed
    if (!hWnd) 
    {
        return nullptr;
    }

    return hWnd;
}






//  ---------   CALLBACKS ---------

// Current window calls the pollEvents 
LigidWindow _procLigidWindow;

/* -- Callback functions of the current window -- */
/* 
    currentLigidWindow.pollEvents calls the _setProcFunctions 
    function and sends it's callback functions to there
*/

void (*_procMousePosCallback)(LigidWindow,double, double);
void (*_procMouseButtonCallback)(LigidWindow,int, int, int);
void (*_procKeyCallback)(LigidWindow,int, int, int);
void (*_procWindowSizeCallback)(LigidWindow,int, int);
void (*_procScrollCallback)(LigidWindow,double, double);

//Set to 0 if the callback functions are not initialized
int _procFuncsInitialized = 0; 

void LigidWindow::_setProcFunctions(
                                        LigidWindow __procLigidWindow,
                                        void (*__procMousePosCallback)(LigidWindow,double, double),
                                        void (*__procMouseButtonCallback)(LigidWindow,int, int, int),
                                        void (*__procKeyCallback)(LigidWindow,int, int, int),
                                        void (*__procWindowSizeCallback)(LigidWindow,int, int),
                                        void (*__procScrollCallback)(LigidWindow,double, double)
                                    )
{
    _procFuncsInitialized = 1;
    _procLigidWindow = __procLigidWindow; 
    _procMousePosCallback = __procMousePosCallback;
    _procMouseButtonCallback = __procMouseButtonCallback;
    _procKeyCallback = __procKeyCallback;
    _procWindowSizeCallback = __procWindowSizeCallback;
    _procScrollCallback = __procScrollCallback;
}


//HWND proc callback sets true if the window should close (window.shouldClose will always return this value if the window handles matches)
bool _WindowProcCloseWindow = false;

//Which window should close
HWND _WindowProcHWND = NULL;

/*
    Check if HWND proc callback function message detected WM_CLOSE flag
    Used by the LigidWindow.shouldClose function
*/
bool LigidWindow::_get_WindowProcCloseWindow(){
#if defined(_WIN32) || defined(_WIN64)
    LigidWindow wnd = *this;
    if(_WindowProcHWND == wnd.window)
        return _WindowProcCloseWindow;
    else 
        false;

#endif
}

void LigidWindow::_set_WindowProcCloseWindow(bool val){
    _WindowProcCloseWindow = val;
}



LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    _WindowProcHWND = hWnd;
 
    if(!_procFuncsInitialized)
        return DefWindowProc(hWnd, uMsg, wParam, lParam);

    switch (uMsg)
    {
        case WM_CLOSE:
        {
            // Optionally add custom logic here to confirm the close request or perform other actions
            // To prevent the window from closing, do not call the default window procedure
            _WindowProcCloseWindow = true;
            return 0;
        }
        
        case WM_DESTROY:
        {
            // Handle window destroy message
            PostQuitMessage(0);
            break;
        }
        
        case WM_EXITSIZEMOVE:
        {
            // Handle the end of window resizing here
            // Perform any necessary actions after window resizing has finished
            // ...
            break;
        }
        
        case WM_NCHITTEST:
        {
            // Handle non-client area hit test to customize behavior during resizing or cursor hovering
            // Determine the hit test result based on the mouse position and return the appropriate value
            // ...
            break;
        }
        
        case WM_MOUSEMOVE:
        {
            // Call the mouse position callback function set by the user using message data
            _procMousePosCallback(
                                    _procLigidWindow,
                                    LOWORD(lParam), //Received mouse x pos  
                                    HIWORD(lParam)  //Received mouse y pos
                                );
            break;
        }

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
        {
            // Check if any message is received regarding the left mouse button
            bool LButtonReceived = uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK || uMsg == WM_LBUTTONUP;

            // Check if any message is received regarding the right mouse button
            bool RButtonReceived = uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONDBLCLK || uMsg == WM_RBUTTONUP; 

            // Check if any message is received regarding the middle mouse button
            bool MButtonReceived = uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONDBLCLK || uMsg == WM_MBUTTONUP; 
            
            // Set the button
            int button = 0;
            if(LButtonReceived)
                button = 0;
            if(RButtonReceived)
                button = 1;
            if(MButtonReceived)
                button = 2;

            // If pressed to the button
            int action = uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN || uMsg ==WM_MBUTTONDOWN;

            // Mods
            int mods = LIGIDGL_MOD_DEFAULT;
            if(_procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) == LIGIDGL_PRESS && _procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_ALT) == LIGIDGL_RELEASE && _procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) == LIGIDGL_RELEASE)
                mods = LIGIDGL_MOD_CONTROL;
            else if(_procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) == LIGIDGL_RELEASE && _procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_ALT) == LIGIDGL_PRESS && _procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) == LIGIDGL_RELEASE)
                mods = LIGIDGL_MOD_ALT;
            else if(_procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) == LIGIDGL_RELEASE && _procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_ALT) == LIGIDGL_RELEASE && _procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) == LIGIDGL_PRESS)
                mods = LIGIDGL_MOD_SHIFT;
            else if(_procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) == LIGIDGL_PRESS && _procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_ALT) == LIGIDGL_PRESS && _procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) == LIGIDGL_RELEASE)
                mods = LIGIDGL_MOD_CONTROL_ALT;
            else if(_procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) == LIGIDGL_PRESS && _procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_ALT) == LIGIDGL_RELEASE && _procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) == LIGIDGL_PRESS)
                mods = LIGIDGL_MOD_CONTROL_SHIFT;
            else if(_procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) == LIGIDGL_PRESS && _procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_ALT) == LIGIDGL_PRESS && _procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) == LIGIDGL_PRESS)
                mods = LIGIDGL_MOD_CONTROL_ALT_SHIFT;



            // Call the mouse button callback function set by the user using message data
            _procMouseButtonCallback(
                                    _procLigidWindow,
                                    button, 
                                    action, 
                                    mods
                                );
            break;
        }

        case WM_KEYDOWN:
        case WM_KEYUP:
        {

            int scanCode = (lParam >> 16) & 0xFF; // Extract scan code
            int virtualKey = MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX); // Convert to virtual key

            // Buffer for the character
            WCHAR buffer[10];
            buffer[0] = '\0';
            
            BYTE keyState[256]; // Create a byte array for key state
            GetKeyboardState(keyState); // Get the current key state

            // The action
            const int action = (HIWORD(lParam) & KF_UP) ? LIGIDGL_RELEASE : LIGIDGL_PRESS;

            // Mods
            int mods = LIGIDGL_MOD_DEFAULT;
            if(_procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) == LIGIDGL_PRESS && _procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_ALT) == LIGIDGL_RELEASE && _procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) == LIGIDGL_RELEASE)
                mods = LIGIDGL_MOD_CONTROL;
            else if(_procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) == LIGIDGL_RELEASE && _procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_ALT) == LIGIDGL_PRESS && _procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) == LIGIDGL_RELEASE)
                mods = LIGIDGL_MOD_ALT;
            else if(_procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) == LIGIDGL_RELEASE && _procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_ALT) == LIGIDGL_RELEASE && _procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) == LIGIDGL_PRESS)
                mods = LIGIDGL_MOD_SHIFT;
            else if(_procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) == LIGIDGL_PRESS && _procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_ALT) == LIGIDGL_PRESS && _procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) == LIGIDGL_RELEASE)
                mods = LIGIDGL_MOD_CONTROL_ALT;
            else if(_procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) == LIGIDGL_PRESS && _procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_ALT) == LIGIDGL_RELEASE && _procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) == LIGIDGL_PRESS)
                mods = LIGIDGL_MOD_CONTROL_SHIFT;
            else if(_procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) == LIGIDGL_PRESS && _procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_ALT) == LIGIDGL_PRESS && _procLigidWindow.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) == LIGIDGL_PRESS)
                mods = LIGIDGL_MOD_CONTROL_ALT_SHIFT;
            

            // Check if the key is a numpad key
            bool isNumpadKey = (wParam >= VK_NUMPAD0 && wParam <= VK_NUMPAD9);

            // Check if the key is an arrow key
            bool isArrowKey = (wParam == VK_LEFT || wParam == VK_RIGHT || wParam == VK_UP || wParam == VK_DOWN);

            if (isArrowKey) {
                int key = 0;
                if (wParam == VK_LEFT) {
                    key = LIGIDGL_KEY_LEFT; // Left arrow
                } 
                else if (wParam == VK_RIGHT) {
                    key = LIGIDGL_KEY_RIGHT; // Right arrow
                } 
                else if (wParam == VK_UP) {
                    key = LIGIDGL_KEY_UP; // Up arrow
                } 
                else if (wParam == VK_DOWN) {
                    key = LIGIDGL_KEY_DOWN; // Down arrow
                }
                
                _procKeyCallback(
                        _procLigidWindow, //window
                        key, //key  
                        action,  //action
                        mods //mods
                    );
            }

            else if(isNumpadKey) {
                // Handle numpad keys here
                // For example, map VK_NUMPAD0 to '0', VK_NUMPAD1 to '1', etc.
                wchar_t numpadChar = L'0' + (wParam - VK_NUMPAD0);
                
                _procKeyCallback(
                                        _procLigidWindow, //window
                                        numpadChar, //key  
                                        action,  //action
                                        mods //mods
                                    );
            }
            else if (ToUnicode(virtualKey, scanCode, keyState, reinterpret_cast<LPWSTR>(buffer), 10, 0) > 0) {
                std::wstring str = std::wstring(buffer);
                if(str.size()){
                    if((str[0] >= 0 && str[0] <= 127)){
                        // Call the key callback function set by the user using message data
                        _procKeyCallback(
                                                _procLigidWindow, //window
                                                str[0], //key  
                                                action,  //action
                                                mods //mods
                                            );
                    }
                }
            }

            break;
        }
        
        case WM_SIZE:
        {

            _procWindowSizeCallback(
                                        _procLigidWindow,
                                        LOWORD(lParam), 
                                        HIWORD(lParam)
                                    );
            break;
        }

        // Check if mouse/touchpad scrolled horizontally
        case WM_MOUSEHWHEEL: 
        {
            double delta = GET_WHEEL_DELTA_WPARAM(wParam);

            _procScrollCallback(
                                    _procLigidWindow,
                                    delta, 
                                    0
                                );
            break;
        } 

        case WM_MOUSEWHEEL:
        {
            double delta = GET_WHEEL_DELTA_WPARAM(wParam);

            _procScrollCallback(
                                    _procLigidWindow,
                                    0, 
                                    delta
                                );
            break;
        }
    
    }

    // Call the default window procedure for remaining messages
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}