/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    See the ./README.md file for the description

*/

#ifndef LIGIDWINDOW_HPP
#define LIGIDWINDOW_HPP 


/* -- Current OpenGL version -- 
    Change these macros as your OpenGL version
*/
#define LIGIDGL_OPENGL_PROFILE_CORE 0
#define LIGIDGL_OPENGL_PROFILE_COMPATIBILITY 1 

#define LIGIDGL_OPENGL_VERSION_MAJOR 4
#define LIGIDGL_OPENGL_VERSION_MINOR 0
#define LIGIDGL_OPENGL_VERSION_PROFILE LIGIDGL_OPENGL_PROFILE_CORE 

/* -- Keys Are Defined Based On The Standard US Keyboard Layout. -- */
/*              TODO : Use scancode (maybeee)*/

/* Actions */
#define LIGIDGL_RELEASE                0
#define LIGIDGL_PRESS                  1
#define LIGIDGL_REPEAT                 2

/* The unknown key */
#define LIGIDGL_KEY_UNKNOWN            -1

/* Printable keys */
#define LIGIDGL_KEY_SPACE              32
#define LIGIDGL_KEY_APOSTROPHE         39  
#define LIGIDGL_KEY_COMMA              44  
#define LIGIDGL_KEY_MINUS              45  
#define LIGIDGL_KEY_PERIOD             46  
#define LIGIDGL_KEY_SLASH              47  
#define LIGIDGL_KEY_0                  48
#define LIGIDGL_KEY_1                  49
#define LIGIDGL_KEY_2                  50
#define LIGIDGL_KEY_3                  51
#define LIGIDGL_KEY_4                  52
#define LIGIDGL_KEY_5                  53
#define LIGIDGL_KEY_6                  54
#define LIGIDGL_KEY_7                  55
#define LIGIDGL_KEY_8                  56
#define LIGIDGL_KEY_9                  57
#define LIGIDGL_KEY_SEMICOLON          59  
#define LIGIDGL_KEY_EQUAL              61  
#define LIGIDGL_KEY_A                  65
#define LIGIDGL_KEY_B                  66
#define LIGIDGL_KEY_C                  67
#define LIGIDGL_KEY_D                  68
#define LIGIDGL_KEY_E                  69
#define LIGIDGL_KEY_F                  70
#define LIGIDGL_KEY_G                  71
#define LIGIDGL_KEY_H                  72
#define LIGIDGL_KEY_I                  73
#define LIGIDGL_KEY_J                  74
#define LIGIDGL_KEY_K                  75
#define LIGIDGL_KEY_L                  76
#define LIGIDGL_KEY_M                  77
#define LIGIDGL_KEY_N                  78
#define LIGIDGL_KEY_O                  79
#define LIGIDGL_KEY_P                  80
#define LIGIDGL_KEY_Q                  81
#define LIGIDGL_KEY_R                  82
#define LIGIDGL_KEY_S                  83
#define LIGIDGL_KEY_T                  84
#define LIGIDGL_KEY_U                  85
#define LIGIDGL_KEY_V                  86
#define LIGIDGL_KEY_W                  87
#define LIGIDGL_KEY_X                  88
#define LIGIDGL_KEY_Y                  89
#define LIGIDGL_KEY_Z                  90
#define LIGIDGL_KEY_LEFT_BRACKET       91  
#define LIGIDGL_KEY_BACKSLASH          92  
#define LIGIDGL_KEY_RIGHT_BRACKET      93  
#define LIGIDGL_KEY_GRAVE_ACCENT       96  
#define LIGIDGL_KEY_WORLD_1            161 
#define LIGIDGL_KEY_WORLD_2            162 

/* Function keys */
#define LIGIDGL_KEY_ESCAPE             27
#define LIGIDGL_KEY_ENTER              13
#define LIGIDGL_KEY_TAB                9
#define LIGIDGL_KEY_BACKSPACE          8
#define LIGIDGL_KEY_INSERT             45
#define LIGIDGL_KEY_DELETE             46
#define LIGIDGL_KEY_RIGHT              39
#define LIGIDGL_KEY_LEFT               37
#define LIGIDGL_KEY_DOWN               40
#define LIGIDGL_KEY_UP                 38
#define LIGIDGL_KEY_PAGE_UP            33
#define LIGIDGL_KEY_PAGE_DOWN          34
#define LIGIDGL_KEY_HOME               36
#define LIGIDGL_KEY_END                35
#define LIGIDGL_KEY_CAPS_LOCK          20
#define LIGIDGL_KEY_SCROLL_LOCK        145
#define LIGIDGL_KEY_NUM_LOCK           144
#define LIGIDGL_KEY_PRINT_SCREEN       44
#define LIGIDGL_KEY_PAUSE              19
#define LIGIDGL_KEY_F1                 112
#define LIGIDGL_KEY_F2                 113
#define LIGIDGL_KEY_F3                 114
#define LIGIDGL_KEY_F4                 115
#define LIGIDGL_KEY_F5                 116
#define LIGIDGL_KEY_F6                 117
#define LIGIDGL_KEY_F7                 118
#define LIGIDGL_KEY_F8                 119
#define LIGIDGL_KEY_F9                 120
#define LIGIDGL_KEY_F10                121
#define LIGIDGL_KEY_F11                122
#define LIGIDGL_KEY_F12                123
#define LIGIDGL_KEY_F13                124
#define LIGIDGL_KEY_F14                125
#define LIGIDGL_KEY_F15                126
#define LIGIDGL_KEY_F16                127
#define LIGIDGL_KEY_F17                128
#define LIGIDGL_KEY_F18                129
#define LIGIDGL_KEY_F19                130
#define LIGIDGL_KEY_F20                131
#define LIGIDGL_KEY_F21                132
#define LIGIDGL_KEY_F22                133
#define LIGIDGL_KEY_F23                134
#define LIGIDGL_KEY_F24                135
#define LIGIDGL_KEY_F25                136
#define LIGIDGL_KEY_KP_0               96
#define LIGIDGL_KEY_KP_1               97
#define LIGIDGL_KEY_KP_2               98
#define LIGIDGL_KEY_KP_3               99
#define LIGIDGL_KEY_KP_4               100
#define LIGIDGL_KEY_KP_5               101
#define LIGIDGL_KEY_KP_6               102
#define LIGIDGL_KEY_KP_7               103
#define LIGIDGL_KEY_KP_8               104
#define LIGIDGL_KEY_KP_9               105
#define LIGIDGL_KEY_KP_DECIMAL         110
#define LIGIDGL_KEY_KP_DIVIDE          111
#define LIGIDGL_KEY_KP_MULTIPLY        106
#define LIGIDGL_KEY_KP_SUBTRACT        109
#define LIGIDGL_KEY_KP_ADD             107
#define LIGIDGL_KEY_KP_ENTER           13
#define LIGIDGL_KEY_KP_EQUAL           187
#define LIGIDGL_KEY_LEFT_SHIFT         16
#define LIGIDGL_KEY_LEFT_CONTROL       17
#define LIGIDGL_KEY_LEFT_ALT           18
#define LIGIDGL_KEY_LEFT_SUPER         91
#define LIGIDGL_KEY_RIGHT_SHIFT        16
#define LIGIDGL_KEY_RIGHT_CONTROL      17
#define LIGIDGL_KEY_RIGHT_ALT          18
#define LIGIDGL_KEY_RIGHT_SUPER        92
#define LIGIDGL_KEY_MENU               93

/* Mods */
#define LIGIDGL_MOD_DEFAULT             0
#define LIGIDGL_MOD_SHIFT               1
#define LIGIDGL_MOD_CONTROL             2
#define LIGIDGL_MOD_ALT                 3
#define LIGIDGL_MOD_CONTROL_SHIFT       4
#define LIGIDGL_MOD_CONTROL_ALT_SHIFT   5
#define LIGIDGL_MOD_ALT_SHIFT           6
#define LIGIDGL_MOD_CONTROL_ALT         7

/* Button */
#define LIGIDGL_MOUSE_BUTTON_LEFT 0
#define LIGIDGL_MOUSE_BUTTON_RIGHT 1
#define LIGIDGL_MOUSE_BUTTON_MIDDLE 2




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

/*!
*   @brief Contains functions might help you related to your window-OpenGL context. 
*
*   See the LigidWindow 
*/  
namespace LigidGL{

    /*! @brief copies the param text to the clipboard*/
    void setClipboardText(const std::string& text);
    
    /*! @brief returns the text from the clipboard*/
    std::string getClipboardText();

    /*! @brief writes resolution & refresh rate of the primary monitor to the reference parameters */
    void getPrimaryMonitorData(int& monitorWidth, int& monitorHeight, int& monitorRefreshRate);

    /*! @brief returns the current time since the program started in seconds */
    double getTime();

    /*! 
    *   @brief Sets the buffer swap interval for the current context. 
    *   @param interval : 1 = VSync & 0 = no VSync 
    */
    void setSwapInterval(int interval);

    /*!
    *   @brief Core OpenGL loader.
    *          Use this function to load your OpenGL functions.
    *          Give that function to the glad or load by yourself
    *            
    *   @param name : Name of the core OpenGL function like "glClear"
    * 
    *   @return Returns the retrieved function
    */
    void* getProcAddress(const char *name);

};


/// @brief Structure representing a custom cursor.
/// Call the create cursor right after initializings
struct LigidCursor
{
public:

    /*! 
        @brief Is set in the create cursor function
        -1 : other cursor
        0 : arrow cursor
        1 : pointer cursor
        2 : vertical arrow cursor
        3 : horizontal arrow cursor
    */
    int cursorType = -1;

    /// @brief Function to create a custom cursor
    /// @param cursorPixelData Size must be cursorWidth * cursorHeight * 4 
    /// @param cursorType -1 : other cursor, 0 : arrow cursor, 1 : pointer cursor, 2 : vertical arrow cursor, 3 : horizontal arrow cursor
    void createCursor(int cursorWidth, int cursorHeight, int cursorHotspotX, int cursorHotspotY, unsigned char* cursorPixelData, int cursorType);

    // Platform-specific 
    #if defined(_WIN32) || defined(_WIN64)
    // User has Windows
    HCURSOR getCursorHandle(); // Returns the cursor handle for the Windows OS
    #elif defined(__APPLE__)
    // User has MacOS
    // Add MacOS-specific cursor handle or implementation here
    #elif defined(__linux__)
    // User has Linux
    // Add Linux-specific cursor handle or implementation here
    #endif

private:
    // Platform-specific cursor handle
    #if defined(_WIN32) || defined(_WIN64)
    // User has Windows
    HCURSOR cursorHandle;
    #elif defined(__APPLE__)
    // User has MacOS
    // Add MacOS-specific cursor handle or implementation here
    #elif defined(__linux__)
    // User has Linux
    // Add Linux-specific cursor handle or implementation here
    #endif
    
};



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

    /*!
        @brief Retrieves window should close data from the window's proc callback function (located in the CreateWindow.cpp (interprets the global variables))
    */
    bool _get_WindowProcCloseWindow();
    

    /*!
    *   @brief The mouse position callback function of the window set by the user.
    *   
    *   Set the function using @ref setMousePosCallback function
    * 
    *   First param : cursor pos in the x axis
    * 
    *   Second param : cursor pos in the y axis
    */
    void (*mousePosCallback)(LigidWindow,double, double);
    
    /*!
    *   @brief The mouse button callback function of the window set by the user.
    *   
    *   Set the function using @ref setMouseButtonCallback function
    * 
    *   First param : button (LIGIDGL_MOUSE_BUTTON_LEFT , LIGIDGL_MOUSE_BUTTON_RIGHT , LIGIDGL_MOUSE_BUTTON_MIDDLE)
    * 
    *   Second param : action (LIGIDGL_PRESS , LIGIDGL_RELEASE)
    *   
    *   Third param : mods (LIGIDGL_MOD_DEFAULT, LIGIDGL_MOD_SHIFT, LIGIDGL_MOD_ALT, LIGIDGL_MOD_CONTROL etc.) 
    */
    void (*mouseButtonCallback)(LigidWindow,int, int, int);

    /*!
    *   @brief The key callback function of the window set by the user.
    *   
    *   Set the function using @ref setKeyCallback function
    * 
    *   First param : key (LIGIDGL_KEY_<desired key>)
    * 
    *   Second param : action (LIGIDGL_PRESS , LIGIDGL_RELEASE)
    *   
    *   Third param : mods (LIGIDGL_MOD_DEFAULT, LIGIDGL_MOD_SHIFT, LIGIDGL_MOD_ALT, LIGIDGL_MOD_CONTROL etc.) 
    */
    void (*keyCallback)(LigidWindow,int, int, int);

    /*!
    *   @brief The window size callback function of the window set by the user.
    *   
    *   Set the function using @ref setWindowSizeCallback function
    * 
    *   First param : windowSizeX
    * 
    *   Second param : windowSizeY
    *   
    */
    void (*windowSizeCallback)(LigidWindow,int, int);
    
    /*!
    *   @brief The mouse scroll callback function of the window set by the user.
    *   
    *   Set the function using @ref setScrollCallback function
    * 
    *   First param : xOffset
    * 
    *   Second param : yOffset
    *   
    */
    void (*scrollCallback)(LigidWindow,double, double);

    void _setProcFunctions(
                            LigidWindow __procLigidWindow,
                            void (*__procMousePosCallback)(LigidWindow,double, double),
                            void (*__procMouseButtonCallback)(LigidWindow,int, int, int),
                            void (*__procKeyCallback)(LigidWindow,int, int, int),
                            void (*__procWindowSizeCallback)(LigidWindow,int, int),
                            void (*__procScrollCallback)(LigidWindow,double, double)
                        );

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
    * !!! Don't forget to call the @ref show function right after creating a window
    * 
    * !!! Creating a window already makes it's OpenGL rendering context current!
    *     Update the OpenGL rendering context after creating a new window ( @ref this->makeContextCurrent function ) if you don't want to
    *     render to the most recent created window class
    */
    int createWindow(int w, int h, const wchar_t* title);

    /*! @brief Swaps the front and back buffers of the window. */
    void swapBuffers();

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
    * 
    */
    void pollEvents();

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

    /*!
    *    @brief changes the style of the window
    *
    *    @param styleKey 0 : Default | 1 : Borderless | 2 : Caption Only | 3 : FullScreen
    */
    void style(const int styleKey);

    /*!
    *   @brief Closes the window when called
    */
    void close();

    /*!
    *   @brief Shows the window (don't forget to call that function after creating a window)
    */
    void show();

    /*!
    *   @brief Hides the window
    */
    void hide();
    
    /*!
    *   @brief Maximize the window
    */
    void maximize();
    
    /*!
    *   @brief Minimize the window
    */
    void minimize();


    //----------    CALLLBACKS  ----------

    /*!
    *  @brief sets the private member variable @ref mousePosCallback function as the func parameter
    *   
    *   Example mouse position callback function :
    * 
    *   void mouse_pos_callback(int mouse_x_pos, int mouse_y_pos){
    *       std::cout << mouse_x_pos << ' ' << mouse_y_pos << std::endl //Print the mouse position data to the terminal
    *   }
    * 
    *   How to use the function :
    * 
    *   setMousePosCallback(mouse_pos_callback);
    * 
    */
    void setMousePosCallback( void (*func)(LigidWindow,double,double) );

    /*!
    *  @brief sets the private member variable @ref mouseButtonCallback function as the func parameter
    *   
    *   Example mouse button callback function :
    * 
    *   void mouse_btn_callback(int button, int action, int mods){
    *       std::cout << button << ' ' << action << ' ' << mods << std::endl //Print the mouse button data to the terminal
    * 
    *       if(button == LIGIDGL_MOUSE_BUTTON_LEFT && action == LIGIDGL_PRESS && mods == LIGIDGL_MOD_DEFAULT){
    *           Pressed to the left mouse button
    *           ...
    *       }
    *   }
    * 
    *   How to use the function :
    * 
    *   setMouseButtonCallback(mouse_btn_callback);
    * 
    */
    void setMouseButtonCallback( void (*func)(LigidWindow,int,int,int) );

    /*!
    *  @brief sets the private member variable @ref keyCallback function as the func parameter
    *   
    *   Example mouse button callback function :
    * 
    *   void key_callback(int key, int action, int mods){
    *       std::cout << button << ' ' << action << ' ' << mods << std::endl //Print the key data to the terminal
    * 
    *       if(key == LIGID_KEY_A && action == LIGIDGL_PRESS && mods == LIGIDGL_MOD_DEFAULT){
    *           Pressed to the 'A' key
    *           ...
    *       }
    *   }
    * 
    *   How to use the function :
    * 
    *   setMouseButtonCallback(key_callback);
    * 
    */
    void setKeyCallback( void (*func)(LigidWindow,int,int,int) );

    /*!
    *  @brief sets the private member variable @ref windowSizeCallback function as the func parameter
    *   
    *   Example window size callback function :
    * 
    *   void window_size_callback(int windowSizeX, int windowSizeY){
    *       std::cout << windowSizeX << ' ' << windowSizeY << std::endl //Print the window size data to the terminal
    *       
    *       Do smt with the window size values
    *       ...  
    *  
    *   }
    * 
    *   How to use the function :
    * 
    *   setWindowSizeCallback(window_size_callback);
    * 
    */
    void setWindowSizeCallback( void (*func)(LigidWindow,int,int) );
    
    /*!
    *  @brief sets the private member variable @ref scrollCallback function as the func parameter
    *   
    *   Example window size callback function :
    * 
    *   void scroll_callback(double xOffset, int yOffset){
    *       
    *       std::cout << xOffset << ' ' << yOffset << std::endl //Print the scroll data to the terminal
    *       
    *       Do smt with the scroll values
    *       ...  
    *  
    *   }
    * 
    *   How to use the function :
    * 
    *   setScrollCallback(scroll_callback);
    * 
    *   (Use the yOffset for the basic mouse scroll)
    * 
    */
    void setScrollCallback( void (*func)(LigidWindow,double,double) );



    //----------    GET - SET   ----------

    /*!
    *    @brief learn if a key is pressed
    *    @param key is LIGIDGL_KEY_<Desired key>
    *    @return Returns LIGIDGL_PRESS if the given key is pressed LIGIDGL_RELEASE if not (returns false if the window is not focused)  
    *    
    */
    bool isKeyPressed(int key);

    /*!
    *    @brief learn if a mouse button is pressed
    *    @param key is LIGIDGL_MOUSE_BUTTON_LEFT, LIGIDGL_MOUSE_BUTTON_RIGHT or LIGIDGL_MOUSE_BUTTON_MIDDLE
    *    @return Returns LIGIDGL_PRESS if the given button is pressed LIGIDGL_RELEASE if not  
    *    
    *    TODO : Move that function to a namespace
    */
    bool isMouseButtonPressed(int button);
    
    /*!
    *   @brief writes the resolution (size) of the window to the parameters
    *   @param w : size of the window in x axis (width) 
    *   @param h : size of the window in y axis (height)
    * 
    *   Use that way :
    * 
    *   int windowWidth = 0;
    *   int windowHeight = 0;
    * 
    *   myLigidWindow.getWindowSize(windowWidth, windowHeight);
    * 
    *   do smt with the width & height values
    *   ...
    */
    void getWindowSize(int& w, int& h);
    

    /*!
    *   @brief sets the resolution of the window
    *   @param w : size of the window in x axis (width) 
    *   @param h : size of the window in y axis (height)
    */
    void setWindowSize(const int w, const int h);

    /*!
    *   @brief writes the resolution (size) of the framebuffer to the parameters
    *   @param w : size of the framebuffer in x axis (width) 
    *   @param h : size of the framebuffer in y axis (height)
    * 
    *   TODO : Is not functional
    * 
    *   Use the framebuffer size as the window size. 
    * 
    *   Use that way :
    * 
    *   int framebufferWidth = 0;
    *   int framebbuferHeight = 0;
    * 
    *   myLigidWindow.getWindowSize(framebufferWidth, framebbuferHeight);
    * 
    *   do smt with the width & height values
    *   ...
    */
    void getFramebufferSize(int& w, int& h);
    
    /*!
    *   @brief writes the position data of the window to the parameters
    *   @param w : position of the window in x axis  
    *   @param h : position of the window in y axis 
    * 
    *   Position of the window : Position of the Left-Top corner 
    * 
    *   Use that way :
    * 
    *   int windowPosX = 0;
    *   int windowPosY = 0;
    * 
    *   myLigidWindow.getWindowPos(windowPosX, windowPosY);
    * 
    *   do smt with the position values
    *   ...
    */
    void getWindowPos(int& x, int& y);

    /*!
    *   @brief sets the position of the window
    *   @param w : position of the window in x axis (can be negative) 
    *   @param h : position of the window in y axis (can be negative)
    * 
    *   Position of the window : Position of the Left-Top corner 
    * 
    */
    void setWindowPos(const int x, const int y);
    
    /*!
    *   @brief Sets the cursor of the window
    *
    *   Use that way : 
    * 
    *   LigidCursor pointerCursor;
    *   LigidCursor arrowCursor;
    * 
    *   int cursorW = 50;
    *   int cursorH = 50;
    * 
    *   char* pointerCursorPixels = new char[cursorW * cursorH * 4];
    *   char* arrowCursorPixels = new char[cursorW * cursorH * 4];
    * 
    *   imageFileLoader.load("path/To/Pointer/Cursor/Image/File.ico", cursorH,cursorW,4, &pointerCursorPixels);
    *   imageFileLoader.load("path/To/Arrow/Cursor/Image/File.ico", cursorH,cursorW,4, &arrowCursorPixels);
    * 
    *   pointerCursor.createCursor(cursorW, cursorH, 0, 0, pointerCursorPixels);
    *   arrowCursor.createCursor(cursorW, cursorH, 0, 0, arrowCursorPixels);
    * 
    *   Then in your rendering loop : 
    * 
    *   while(!ligidWindow.shouldClose()){
    *       if(mouseOnAnyButton)
    *           ligidWindow.setCursor(pointerCursor);
    *       else
    *           ligidWindow.setCursor(arrowCursor);
    *   }
    */
    void setCursor(LigidCursor cursor);

    /*!
        @brief Function to toggle cursor visibility
    */
    void setCursorVisibility(bool visible);
    
    /*!
        @brief Returns true if the cursor hovers the title bar
    */
    bool isCursorOnTitleBar();
    
    /*!
        @brief Returns true if the cursor hovers the resizing area
    */
    bool isCursorOnResizingArea();

    /*!
        @brief Changes to icon displayed in the window title bar 
        @param iconPath path to the icon texture (*.ico)
    */
    bool setWindowIcon(const wchar_t* iconPath);

    void* getWindowUserPointer();

    void setWindowUserPointer(void* pointer);

};

#endif