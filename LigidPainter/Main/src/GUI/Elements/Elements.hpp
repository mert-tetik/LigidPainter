
/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

//
#ifndef ELEMENTS_HPP
#define ELEMENTS_HPP

//OpenGL & LigidGL(handles window)
#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

//GLM - Math library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string> //std::string
#include <iostream> //input & outputs file streams
#include <vector> //std::vector

#include "UTIL/Util.hpp"

/// @brief Represent a side of a panel (Left Side, Bottom Side etc.)
struct PanelSide{ 
    //Used in the GUI/Elements/panel.cpp 
    
    //Cursor on the panel side
    bool hover = false;
    //Is the side is pressed (drag the side & scale the panel if pressed)
    bool pressed = false;
    //Set hover false no matter what if locked
    bool locked = false;
};

//Forward declarations
struct Element;

//!------------------------------BUTTON------------------------------

//Use these macros to define the style parameter in a GUI elements constructor (affects the button mostly)
//Most GUI constructors has only 1 style

//Stylized button (outline only & smooth edges)
#define ELEMENT_STYLE_STYLIZED 0

//Solid button (slim outline & sharp edges)
#define ELEMENT_STYLE_SOLID 1

//Basic button (outline & half-smooth edges)
#define ELEMENT_STYLE_BASIC 2

/// @brief To render & manage activites of a button (functions were declared at the GUI/Elements/Button)
class Button
{
private:
    //Private member functions
    
    void render(glm::vec3 resultPos,glm::vec2 resultScale,float resultRadius,float resultOutlineThickness);
    bool renderTheTexture(glm::vec3 resultPos,glm::vec2 resultScale,float resultScaleText,float &textureRadius);
    void manageMouseActivity();
    void renderTextAndTexture(float resultScaleText);
public:
    //Pressed to the button
    //Returns true if a button is pressed
    //Returns back to false once the mouse left key is released (if true) 
    bool clickState1 = false;
    
    //While rendering IN THE PANEL put that much additional space
    float panelOffset = 0.f;
    
    //Text of the button
    std::string text;

    //Main color of the button
    glm::vec4 color;
    
    //Animation color (switch to that color if hover or clicked)
    glm::vec4 color2;
    
    //Main color of the text of the button
    glm::vec4 textColor;

    //Animation color of the text (switch to that color if hover or clicked)
    glm::vec4 textColor2;
    
    //Text's scale value
    float textScale;

    //Render only the outline if set to true
    bool outline; 

    //Put an additional outline to the button if set to true (both outline & outlineExtra can't be true)
    bool outlineExtra;
    
    //Thickness value of the button
    float outlineThickness;

    bool textureSelection = false;
    bool colorSelection = false;
    
    glm::vec3 outlineColor;
    glm::vec3 outlineColor2;
    bool keepPressingState;
    float radius;  
    int animationStyle;
    Texture texture; 
    glm::vec2 scale;  
    glm::vec3 pos;
    glm::vec3 resultPos;
    glm::vec2 resultScale;
    bool doMouseTracking;
    bool hover = false;
    float hoverMixVal = 0.f;
    float clickedMixVal = 0.f;
    float textureSizeScale = 1.f;
    bool textureStickToTop = false;
    bool textAlignLeft = false;
    bool clicked = false;
    

    //Constructors 
    Button();
    Button(std::string text, glm::vec2 scale, glm::vec4 color, glm::vec4 color2, bool outline, float radius, 
           int animationStyle,glm::vec4 textColor,glm::vec4 textColor2,Texture texture,float textScale,float panelOffset,
           bool outlineExtra,glm::vec3 outlineColor,glm::vec3 outlineColor2,float outlineThickness,bool keepPressingState);
    Button(int style, glm::vec2 scale, std::string text, Texture texture, float panelOffset, bool keepPressingState);

    //Public member functions
    void render(Timer &timer,bool doMouseTracking);
};


//!------------------------------TEXTBOX------------------------------


/// @brief UI Element which inputs text from user
class TextBox
{
private:
    /// @brief Renders a box using button shader
    void render(glm::vec3 resultPos,glm::vec2 resultScale,float resultRadius,float resultOutlineThickness);
public:
    bool active = false;
    float panelOffset = 0.f; 
    std::string text; 
    glm::vec4 color;
    glm::vec4 color2; 
    glm::vec4 textColor; 
    glm::vec4 textColor2; 
    float textScale;
    glm::vec3 outlineColor; 
    glm::vec3 outlineColor2;
    int activeChar = 0; 
    int activeChar2 = 0;
    int animationStyle;
    glm::vec2 scale;  
    glm::vec3 pos; 
    bool doMouseTracking;
    bool hover = false; 
    float hoverMixVal = 0.f; 
    float clickedMixVal = 0.f; 
    int openSelectFolderDialog;
    float thickness = 2.f;
    float radius = 0.35f; 
    int textPosCharIndex = 0;

    //Constructors
    TextBox();
    TextBox(std::string text, glm::vec2 scale, glm::vec4 color, glm::vec4 color2, int animationStyle,glm::vec4 textColor,glm::vec4 textColor2,
            float textScale,float panelOffset,glm::vec3 outlineColor,glm::vec3 outlineColor2,int openSelectFolderDialog);
    TextBox(int style,glm::vec2 scale,std::string text,float panelOffset,int openSelectFolderDialog);
    
    /// @brief Public member function to render the text box
    void render(Timer &timer,bool doMouseTracking);
};


//!------------------------------COMBO BOX------------------------------


class ComboBox
{
private:
    //Private member functions
    void render(glm::vec3 resultPos,glm::vec2 resultScale,float resultRadius,float resultOutlineThickness,float aClickedMixVal,
                float aHoverMixVal,bool outline,glm::vec4 aColor);
public:
    float panelOffset = 0.f; 
    std::vector<std::string> texts; 
    std::string text;
    glm::vec4 color; 
    glm::vec4 color2;
    glm::vec4 textColor; 
    glm::vec4 textColor2;
    glm::vec4 bgColor;
    bool pressed = false;
    bool selectionDone = false;
    float textScale;
    glm::vec3 outlineColor;
    glm::vec3 outlineColor2;
    glm::vec2 scale;
    glm::vec3 pos;
    bool doMouseTracking;
    std::vector<bool> hover;
    int selectedIndex = 0;
    std::vector<float> hoverMixVal;
    std::vector<float> clickedMixVal;
    
    //Constructors
    ComboBox();
    ComboBox(std::vector<std::string> texts, glm::vec2 scale, glm::vec4 color, glm::vec4 color2,glm::vec4 textColor,glm::vec4 textColor2,
           float textScale,float panelOffset,glm::vec3 outlineColor,glm::vec3 outlineColor2);

    ComboBox(int style,glm::vec2 scale,std::vector<std::string> texts,std::string text,float panelOffset);

    //Public member functions
    void render(Timer &timer,bool doMouseTracking);
};


//!------------------------------TEXT RENDERER------------------------------

#define TEXTRENDERER_ALIGNMENT_LEFT 0
#define TEXTRENDERER_ALIGNMENT_MID 1
#define TEXTRENDERER_ALIGNMENT_RIGHT 2

class TextRenderer
{
private:
    void renderLeftToRight(glm::vec3 pos);
    void renderRightToLeft(glm::vec3 pos);
    void rndrTxt(int textPosCharIndex);
    void renderInsertionPointCursor(int &textPosCharIndex);

    Timer timer; //Timer of the text renderer (used for animation of the insertion point cursor)

public:
    Font font;
	bool keyInput = false;
	bool caps = false;
	int key = 0;
	int mods = 0;
    int action = 0;

    //Text data
    std::string textDataText;
    glm::vec3 textDataPos;
    bool textDataMultipleLines;
    float textDataScale;
    float textDataMinX;
    float textDataMaxX;
    bool textDataAlignment;

    //Aditional text data for the text box
    bool textDataActive;
    int textDataActiveChar;
    int textDataActiveChar2;
    int textDataTextPosCharIndex;

    //Constructors
    TextRenderer(/* args */);
    TextRenderer(Font font);

    //Util public member functions
    float getTextLastCharOffset();
    float getIndexOffset(int charIndex);
    bool doesTheTextOverflow(float x);
    glm::vec3 positionTheText();


    void loadTextData(std::string text, glm::vec3 pos, bool multipleLines, 
                      float scale, float minX,float maxX, int alignment);
    
    void loadTextData(std::string text, glm::vec3 pos, bool multipleLines, 
                      float scale, float minX,float maxX, int alignment,bool active,
                      int activeChar, int activeChar2, int textPosCharIndex);

    void renderText();
    
    void renderText(int &textPosCharIndex,glm::vec4 textColor);

	void processTextInput(std::string &text,int &activeChar,int &activeChar2,int &textPosCharIndex);
};

extern TextRenderer textRenderer;

//!------------------------------CHECKBOX------------------------------


class CheckBox
{
private:
    //Private member functions
    void render(glm::vec3 resultPos,glm::vec2 resultScale,float resultRadius,float resultOutlineThickness);
    void manageMouseActivity(glm::vec2 resultScale, glm::vec3 resultPos);

public:
    bool clickState1 = false;
    float panelOffset = 0.f;
    std::string text;
    glm::vec4 color; 
    glm::vec4 color2;
    glm::vec4 textColor;
    glm::vec4 textColor2;
    float textScale;
    glm::vec3 outlineColor;
    glm::vec3 outlineColor2;
    int animationStyle;
    glm::vec2 scale;
    glm::vec3 pos; 
    bool doMouseTracking;
    bool hover = false;
    float hoverMixVal = 0.f;
    float clickedMixVal = 0.f;

    //Constructors 
    CheckBox();
    CheckBox(std::string text, glm::vec2 scale, glm::vec4 color, glm::vec4 color2, int animationStyle,glm::vec4 textColor,glm::vec4 textColor2,
             float textScale,float panelOffset,glm::vec3 outlineColor,glm::vec3 outlineColor2);
    CheckBox(int style,glm::vec2 scale,std::string text,float panelOffset);

    //Public member functions
    void render(Timer &timer,bool doMouseTracking);
};



//!------------------------------RANGEBAR------------------------------


/// @brief UI Element which inputs a float value between a range from user
class RangeBar
{
private:
    void render(glm::vec3 posVal,glm::vec2 scaleVal,float radiusVal,glm::vec4 color1Val, glm::vec4 color2Val,float mixVal,bool outlineExtra,float resultOutlineThickness);
    
public:
    bool pointerPressed = false;
    float panelOffset = 0.f; 
    std::string text; 
    glm::vec4 color; 
    glm::vec4 color2; 
    glm::vec4 pointerColor; 
    glm::vec4 pointerColor2; 
    glm::vec4 textColor; 
    glm::vec4 textColor2;
    float textScale; 
    bool outlineExtra;  
    glm::vec3 outlineColor;
    glm::vec3 outlineColor2;
    float outlineThickness;
    float radius; 
    Texture texture; 
    glm::vec2 scale;  
    glm::vec3 pos;
    bool hover = false; 
    float hoverMixVal = 0.f; 
    float clickedMixVal = 0.f; 
    float minValue = -50.f; 
    float maxValue = 50.f; 
    float value = (maxValue+minValue)/2.f; 
    bool doMouseTracking; 
    bool isNumeric = false;
    float leftArrowMixVal = 0.f;
    float leftArrowClickedMixVal = 0.f;
    float rightArrowMixVal = 0.f;
    float rightArrowClickedMixVal = 0.f;
    bool leftArrowHover = false;
    bool rightArrowHover = false;
    bool valueDoneChanging = false;

    //Constructors
    RangeBar();
    RangeBar(std::string text, glm::vec2 scale, glm::vec4 color, glm::vec4 color2,glm::vec4 pointerColor,glm::vec4 pointerColor2,
             bool outlineExtra,glm::vec3 outlineColor,glm::vec3 outlineColor2, float radius,glm::vec4 textColor,glm::vec4 textColor2,Texture texture,
             float textScale,float panelOffset,float outlineThickness,float minValue,float maxValue,float value);
    RangeBar(int style,glm::vec2 scale,std::string text,Texture texture,float panelOffset,
             float minValue,float maxValue,float value);

    /// @brief Public member function to render the text box
    void render(Timer &timer,bool doMouseTracking);
};



struct Element{
    //The UI Element

    Button button;
    RangeBar rangeBar;
    CheckBox checkBox;
    ComboBox comboBox;
    TextBox textBox;

    /*!
        @brief Decide which GUI element will be used 
        0 = button 
        1 = rangebar
        2 = checbox 
        3 = combobox 
        4 = textbox
    */
    int state; 
    
    //Those variables will be transported to the element
    //  - Manipulated & used by the panel
    glm::vec3 pos;
    glm::vec2 scale;
    float panelOffset;

    //Constructors
    Element();
    Element(Button button);
    Element(RangeBar rangeBar);
    Element(CheckBox checkBox);
    Element(ComboBox comboBox);
    Element(TextBox textBox);

    bool isInteracted();

    //Public member function
    void render(Timer &timer,bool doMouseTracking);
};
struct Section{ //Sections seperates the elements in the panel
    
    // Header element (After pressing that button you can display other elements in that section)
    //! Is always button
    Element header; 

    std::vector<Element> elements; //Elements of that section
    
    //Constructors
    Section();
    Section(Element header,std::vector<Element> elements);
};






//!------------------------------PANEL------------------------------


class Panel
{
private:
    float slideRatio = 1.f;
    
    void mouseTracking();
    void resizeThePanel();
    void prepDrawBtnVertically(Element &button,Element &previousButton,float& elementPos,int btnCounter);
    void prepDrawBtnHorizontally(Element &button,Element &previousButton,float& elementPos,int btnCounter);
    void renderTheHeader(int sectionI,float &elementPos, int &btnCounter, Timer &timer);
    void drawPanel(glm::vec3 resultPos,glm::vec2 resultScale,Timer &timer); //Draws the panel and it's elements

public:
    /*! 
        @brief Sections of the panel 
    
        A section consists of one header and GUI elements

        Panel renders the elements
    */ 
    std::vector<Section> sections;
    
    /*! @brief Scale of the panel in the range of 0 - 100*/
    glm::vec2 scale;  
    
    /*! @brief Position of the panel in the range of 0 - 100*/
    glm::vec3 pos; 
    
    /*! @brief Is added to the @ref pos in the range of 0 - 100 */
    glm::vec3 additionalPos = glm::vec3(0); 
    
    /*! @brief Color of the panel*/
    glm::vec4 color;
    
    /*! @brief Second color of the panel (used for the outline color)*/
    glm::vec4 color2;
    
    /*! @brief True if the cursor hovers the panel*/
    bool hover = false; 
    
    /*! @brief Takes the value from the constructor. Indicates whether the panel is vertical or horizontal*/
    bool vertical = true;
    
    /*! 
        @brief Indicates how many elements can be in a row 
        (Column if panel is horizontal)
        (Is mostly 1)
    */
    int rowCount; 
    
    /*!
        @brief Buttons on top of the panel
        Just like a title bar for the panel
    */
    std::vector<Button> barButtons;

    /*! @brief Outline thickness of the panel*/
    float outlineThickness;
    
    /*! 
        @brief Set to false if you don't want to clear the 
        depth buffer of the framebuffer after rendering the panel
    */
    bool clearDepthBuffer = true;

    bool resizingDone = false;

    /*-- Sides of the panel --*/

    PanelSide leftSide;
    PanelSide rightSide;
    PanelSide bottomSide;
    PanelSide topSide;

    /*!
        @brief Scale value of the panel in the screen coordinates.
        Generated using the @ref scale value after rendering the panel
    */
    glm::vec2 resultScale;
    
    /*!
        @brief Position value of the panel in the screen coordinates.
        Generated using the @ref pos value after rendering the panel
    */
    glm::vec3 resultPos;

    /*!
        @brief If the panel has a slider. Indicates how much got scrolled. 
    */
    float slideVal = 0.f;

    /*! @brief Panel's optional scale value can't be higher than this value (scale.y for horizontal panels)*/
    float maxScaleVal;    
    
    /*! @brief Set to false if the panel is locked or smt*/
    bool doMouseTracking;
    
    /*! @brief Is true (special for the library displayer panel)*/
    bool isLibraryDisplayer = false;
    
    /*! @brief Indicate if the panel will use a slider if elements overflows */
    bool hasSlider = false;
    
    /*! @brief This button is used for the slider */
    Button sliderButton;

    /*! @brief Default constructor */
    Panel();

    /*!
        @param shader ButtonShader used for rendering the panel & it's elements 
        @param colorPalette LigidPainter theme color palette
        @param sections Sections of the panel
        @param scale Initial scale of the panel
        @param pos Initial position of the panel
        @param color Color of the panel
        @param color2 Outline color of the panel
        @param vertical Is the panel vertical?
        @param lockL Disable resizing from the left side
        @param lockR Disable resizing from the right side
        @param lockB Disable resizing from the bottom side
        @param lockT Disable resizing from the top side
        @param outlineThickness 
        @param rowCount 
        @param barButtons 
        @param maxScaleVal 
        @param hasSlider 
    */
    Panel(std::vector<Section> sections,glm::vec2 scale,glm::vec3 pos,glm::vec4 color,glm::vec4 color2,
          bool vertical,bool lockL,bool lockR,bool lockB,bool lockT,float outlineThickness,int rowCount,std::vector<Button> barButtons,float maxScaleVal,
          bool hasSlider);

    /// @brief Renders the panel
    void render(Timer &timer,bool doMouseTracking);
};

#endif //ELEMENTS_HPP