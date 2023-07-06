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
#define LIGIDGL_KEY_ESCAPE             256
#define LIGIDGL_KEY_ENTER              257
#define LIGIDGL_KEY_TAB                258
#define LIGIDGL_KEY_BACKSPACE          259
#define LIGIDGL_KEY_INSERT             260
#define LIGIDGL_KEY_DELETE             261
#define LIGIDGL_KEY_RIGHT              262
#define LIGIDGL_KEY_LEFT               263
#define LIGIDGL_KEY_DOWN               264
#define LIGIDGL_KEY_UP                 265
#define LIGIDGL_KEY_PAGE_UP            266
#define LIGIDGL_KEY_PAGE_DOWN          267
#define LIGIDGL_KEY_HOME               268
#define LIGIDGL_KEY_END                269
#define LIGIDGL_KEY_CAPS_LOCK          280
#define LIGIDGL_KEY_SCROLL_LOCK        281
#define LIGIDGL_KEY_NUM_LOCK           282
#define LIGIDGL_KEY_PRINT_SCREEN       283
#define LIGIDGL_KEY_PAUSE              284
#define LIGIDGL_KEY_F1                 290
#define LIGIDGL_KEY_F2                 291
#define LIGIDGL_KEY_F3                 292
#define LIGIDGL_KEY_F4                 293
#define LIGIDGL_KEY_F5                 294
#define LIGIDGL_KEY_F6                 295
#define LIGIDGL_KEY_F7                 296
#define LIGIDGL_KEY_F8                 297
#define LIGIDGL_KEY_F9                 298
#define LIGIDGL_KEY_F10                299
#define LIGIDGL_KEY_F11                300
#define LIGIDGL_KEY_F12                301
#define LIGIDGL_KEY_F13                302
#define LIGIDGL_KEY_F14                303
#define LIGIDGL_KEY_F15                304
#define LIGIDGL_KEY_F16                305
#define LIGIDGL_KEY_F17                306
#define LIGIDGL_KEY_F18                307
#define LIGIDGL_KEY_F19                308
#define LIGIDGL_KEY_F20                309
#define LIGIDGL_KEY_F21                310
#define LIGIDGL_KEY_F22                311
#define LIGIDGL_KEY_F23                312
#define LIGIDGL_KEY_F24                313
#define LIGIDGL_KEY_F25                314
#define LIGIDGL_KEY_KP_0               320
#define LIGIDGL_KEY_KP_1               321
#define LIGIDGL_KEY_KP_2               322
#define LIGIDGL_KEY_KP_3               323
#define LIGIDGL_KEY_KP_4               324
#define LIGIDGL_KEY_KP_5               325
#define LIGIDGL_KEY_KP_6               326
#define LIGIDGL_KEY_KP_7               327
#define LIGIDGL_KEY_KP_8               328
#define LIGIDGL_KEY_KP_9               329
#define LIGIDGL_KEY_KP_DECIMAL         330
#define LIGIDGL_KEY_KP_DIVIDE          331
#define LIGIDGL_KEY_KP_MULTIPLY        332
#define LIGIDGL_KEY_KP_SUBTRACT        333
#define LIGIDGL_KEY_KP_ADD             334
#define LIGIDGL_KEY_KP_ENTER           335
#define LIGIDGL_KEY_KP_EQUAL           336
#define LIGIDGL_KEY_LEFT_SHIFT         340
#define LIGIDGL_KEY_LEFT_CONTROL       341
#define LIGIDGL_KEY_LEFT_ALT           342
#define LIGIDGL_KEY_LEFT_SUPER         343
#define LIGIDGL_KEY_RIGHT_SHIFT        344
#define LIGIDGL_KEY_RIGHT_CONTROL      345
#define LIGIDGL_KEY_RIGHT_ALT          346
#define LIGIDGL_KEY_RIGHT_SUPER        347
#define LIGIDGL_KEY_MENU               348

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
};


/// @brief Structure representing a custom cursor.
/// Call the create cursor right after initializings
struct LigidCursor
{
public:
    /// @brief Function to create a custom cursor
    /// @param cursorPixelData Size must be cursorWidth * cursorHeight * 4 
    void createCursor(int cursorWidth, int cursorHeight, int cursorHotspotX, int cursorHotspotY, char* cursorPixelData);

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
    *   @brief The mouse position callback function of the window set by the user.
    *   
    *   Set the function using @ref setMousePosCallback function
    * 
    *   First param : cursor pos in the x axis
    * 
    *   Second param : cursor pos in the y axis
    */
    void (*mousePosCallback)(int, int);
    
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
    void (*mouseButtonCallback)(int, int, int);

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
    * 
    *   Also calls the callback functions
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
    void setMousePosCallback( void (*func)(int,int) );

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
    void setMouseButtonCallback( void (*func)(int,int,int) );



    //----------    GET - SET   ----------

    /*!
    *    @brief learn if a key is pressed
    *    @param key is LIGIDGL_KEY_<Desired key>
    *    @return Returns LIGIDGL_PRESS if the given key is pressed LIGIDGL_RELEASE if not  
    *    
    *    TODO : Move that function to a namespace
    */
    bool isKeyPressed(char key);

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
    
};

#endif