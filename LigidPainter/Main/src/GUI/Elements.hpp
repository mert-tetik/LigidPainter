
/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef ELEMENTS_HPP
#define ELEMENTS_HPP

//Forward declarations
class ColorPalette;
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "UTIL/Util.hpp"

struct PanelSide{ 
    bool hover = false;
    bool pressed = false;
    bool locked = false;
};

struct NodeConnection{
    int nodeIndex; //Connected to which node
    int inputIndex; //Connected to which input

    NodeConnection(int nodeIndex,int inputIndex);
};

class TextRenderer;
struct Element;

//!------------------------------BUTTON------------------------------

#define BUTTON_STYLE_STYLIZED 0
#define BUTTON_STYLE_SOLID 1
#define BUTTON_STYLE_BASIC 2

class Button
{
private:
    //Private member functions
    void render(glm::vec3 resultPos,glm::vec2 resultScale,float resultRadius,float resultOutlineThickness);
    bool renderTheTexture(glm::vec3 resultPos,glm::vec2 resultScale,float resultScaleText,glm::vec2 videoScale,TextRenderer &textRenderer,float &textureRadius);
    void manageMouseActivity(Mouse &mouse);
    void renderTextAndTexture(TextRenderer &textRenderer, glm::vec2 videoScale, float resultScaleText);

public:
    Shader shader;
    bool clickState1 = false;
    float panelOffset = 0.f;
    std::string text;
    glm::vec4 color;
    glm::vec4 color2;
    glm::vec4 textColor;
    glm::vec4 textColor2;
    float textScale;
    bool outline; 
    bool outlineExtra;
    float outlineThickness;
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
    

    //Constructors 
    Button();
    Button(Shader shader,std::string text, glm::vec2 scale, glm::vec4 color, glm::vec4 color2, bool outline, float radius, 
           int animationStyle,glm::vec4 textColor,glm::vec4 textColor2,Texture texture,float textScale,float panelOffset,
           bool outlineExtra,glm::vec3 outlineColor,glm::vec3 outlineColor2,float outlineThickness,bool keepPressingState);
    Button(int style,glm::vec2 scale,ColorPalette colorPalette,Shader shader,std::string text,Texture texture,float panelOffset,bool keepPressingState);

    //Public member functions
    void render(glm::vec2 videoScale,Mouse& mouse, Timer &timer,TextRenderer &textRenderer,bool doMouseTracking);
};


//!------------------------------TEXTBOX------------------------------


/// @brief UI Element which inputs text from user
class TextBox
{
private:
    /// @brief Renders a box using button shader
    void render(glm::vec3 resultPos,glm::vec2 resultScale,float resultRadius,float resultOutlineThickness);
public:
    Shader shader;
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
    TextBox(Shader shader,std::string text, glm::vec2 scale, glm::vec4 color, glm::vec4 color2, int animationStyle,glm::vec4 textColor,glm::vec4 textColor2,
            float textScale,float panelOffset,glm::vec3 outlineColor,glm::vec3 outlineColor2,int openSelectFolderDialog);
    TextBox(int style,glm::vec2 scale,ColorPalette colorPalette,Shader shader,std::string text,float panelOffset,int openSelectFolderDialog);
    
    /// @brief Public member function to render the text box
    void render(glm::vec2 videoScale,Mouse& mouse, Timer &timer,TextRenderer &textRenderer,bool doMouseTracking,GLFWwindow* window);
};


//!------------------------------COMBO BOX------------------------------


class ComboBox
{
private:
    //Private member functions
    void render(glm::vec3 resultPos,glm::vec2 resultScale,float resultRadius,float resultOutlineThickness,float aClickedMixVal,
                float aHoverMixVal,bool outline,glm::vec4 aColor);
public:
    Shader shader;
    float panelOffset = 0.f; 
    std::vector<std::string> texts; 
    std::string text;
    glm::vec4 color; 
    glm::vec4 color2;
    glm::vec4 textColor; 
    glm::vec4 textColor2;
    glm::vec4 bgColor;
    bool pressed = false;
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
    ComboBox(Shader shader,std::vector<std::string> texts, glm::vec2 scale, glm::vec4 color, glm::vec4 color2,glm::vec4 textColor,glm::vec4 textColor2,
           float textScale,float panelOffset,glm::vec3 outlineColor,glm::vec3 outlineColor2);

    ComboBox(int style,glm::vec2 scale,ColorPalette colorPalette,Shader shader,std::vector<std::string> texts,std::string text,float panelOffset);

    //Public member functions
    void render(glm::vec2 videoScale,Mouse& mouse, Timer &timer,TextRenderer &textRenderer,bool doMouseTracking,GLFWwindow* window);
};


//!------------------------------TEXT RENDERER------------------------------

#define TEXTRENDERER_ALIGNMENT_LEFT 0
#define TEXTRENDERER_ALIGNMENT_MID 1
#define TEXTRENDERER_ALIGNMENT_RIGHT 2

class TextRenderer
{
private:
    void renderLeftToRight(Shader shader,glm::vec3 pos);
    void renderRightToLeft(Shader shader,glm::vec3 pos);
    void rndrTxt(Shader shader, int textPosCharIndex);
    void renderInsertionPointCursor(Shader shader, int &textPosCharIndex);

    glm::vec2 videoScale;
    Timer timer; //Timer of the text renderer (used for animation of the insertion point cursor)
public:
    Font font;
	bool keyInput = false;
	bool caps = false;
	char key = 0;
	int mods = 0;

    //Text data
    Shader textDataShader;
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
    TextRenderer(Font font, glm::vec2 videoScale);

    //Util public member functions
    float getTextLastCharOffset();
    float getIndexOffset(int charIndex);
    bool doesTheTextOverflow(float x);
    glm::vec3 positionTheText();


    void loadTextData(Shader shader, std::string text, glm::vec3 pos, bool multipleLines, 
                      float scale, float minX,float maxX, int alignment);
    
    void loadTextData(Shader shader, std::string text, glm::vec3 pos, bool multipleLines, 
                      float scale, float minX,float maxX, int alignment,bool active,
                      int activeChar, int activeChar2, int textPosCharIndex);

    void renderText(Shader shader);
    
    void renderText(Shader shader,int &textPosCharIndex,glm::vec4 textColor);

	void processTextInput(std::string &text,int &activeChar,int &activeChar2,int &textPosCharIndex);
};



//!------------------------------CHECKBOX------------------------------


class CheckBox
{
private:
    //Private member functions
    void render(glm::vec3 resultPos,glm::vec2 resultScale,float resultRadius,float resultOutlineThickness);
public:
    Shader shader;
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
    CheckBox(Shader shader,std::string text, glm::vec2 scale, glm::vec4 color, glm::vec4 color2, int animationStyle,glm::vec4 textColor,glm::vec4 textColor2,
             float textScale,float panelOffset,glm::vec3 outlineColor,glm::vec3 outlineColor2);
    CheckBox(int style,glm::vec2 scale,ColorPalette colorPalette,Shader shader,std::string text,float panelOffset);

    //Public member functions
    void render(glm::vec2 videoScale,Mouse& mouse, Timer &timer,TextRenderer &textRenderer,bool doMouseTracking);
};



//!------------------------------RANGEBAR------------------------------


/// @brief UI Element which inputs a float value between a range from user
class RangeBar
{
private:
    void render(glm::vec3 posVal,glm::vec2 scaleVal,float radiusVal,glm::vec4 color1Val, glm::vec4 color2Val,float mixVal,bool outlineExtra,float resultOutlineThickness);
public:
    Shader shader;
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

    //Constructors
    RangeBar();
    RangeBar(Shader shader,std::string text, glm::vec2 scale, glm::vec4 color, glm::vec4 color2,glm::vec4 pointerColor,glm::vec4 pointerColor2,
             bool outlineExtra,glm::vec3 outlineColor,glm::vec3 outlineColor2, float radius,glm::vec4 textColor,glm::vec4 textColor2,Texture texture,
             float textScale,float panelOffset,float outlineThickness,float minValue,float maxValue,float value);
    RangeBar(int style,glm::vec2 scale,ColorPalette colorPalette,Shader shader,std::string text,Texture texture,float panelOffset,
             float minValue,float maxValue,float value);

    /// @brief Public member function to render the text box
    void render(glm::vec2 videoScale,Mouse& mouse, Timer &timer,TextRenderer &textRenderer,bool doMouseTracking);
};



struct Element{
    //The UI Element

    Button button;
    RangeBar rangeBar;
    CheckBox checkBox;
    ComboBox comboBox;
    TextBox textBox;

    GLFWwindow* window;
    
    int state; //Decide which element will be used 0 = button 
    
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
    Element(ComboBox comboBox,GLFWwindow* window);
    Element(TextBox textBox,GLFWwindow* window);

    //Public member function
    void render(glm::vec2 videoScale,Mouse& mouse, Timer &timer,TextRenderer &textRenderer,bool doMouseTracking);
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
    
    /// Private member functions
    void mouseTracking(Mouse& mouse);
    void resizeThePanel(Mouse &mouse, glm::vec2 videoScale);
    void prepDrawBtnVertically(Element &button,Element &previousButton,float& elementPos,int btnCounter);
    void prepDrawBtnHorizontally(Element &button,Element &previousButton,float& elementPos,int btnCounter);
    void drawPanel(glm::vec2 videoScale,Mouse &mouse, glm::vec3 resultPos,glm::vec2 resultScale,Timer &timer,TextRenderer &textRenderer); //Draws the panel and it's elements

public:
    Shader shader;
    std::vector<Section> sections;
    glm::vec2 scale;  
    glm::vec3 pos; 
    glm::vec4 color;
    glm::vec4 color2;
    bool hover = false; 
    bool vertical = true;
    int rowCount; 
    std::vector<Button> barButtons;
    float outlineThickness;
    bool clearDepthBuffer = true;
    PanelSide leftSide;
    PanelSide rightSide;
    PanelSide bottomSide;
    PanelSide topSide;
    glm::vec2 resultScale;
    glm::vec3 resultPos;
    float slideVal = 0.f;
    float maxScaleVal;    
    bool doMouseTracking;
    bool isLibraryDisplayer = false;
    bool hasSlider = false;
    Button sliderButton;

    //Constructors
    Panel();
    Panel(Shader shader,ColorPalette colorPalette,std::vector<Section> sections,glm::vec2 scale,glm::vec3 pos,glm::vec4 color,glm::vec4 color2,
          bool vertical,bool lockL,bool lockR,bool lockB,bool lockT,float outlineThickness,int rowCount,std::vector<Button> barButtons,float maxScaleVal,
          bool hasSlider);

    /// @brief Public member function to render the panel
    void render(glm::vec2 videoScale,Mouse& mouse,Timer &timer,TextRenderer &textRenderer,bool doMouseTracking);
};



//!------------------------------NODEIO-----------------------------


class NodeIO
{
private:
    /* data */
public:
    std::vector<NodeConnection> connections;
    std::string title;
    Element element; 
    Button IOCircle;
    glm::vec4 color; 
    unsigned int result; 
    int state;

    //Constructor
    NodeIO(std::string title ,Element element,glm::vec4 color,ColorPalette colorPalette,Shader buttonShader,glm::vec2 videoScale,int state);
};

class Node
{
private:
    bool renderBarriers(Panel &nodeEditorPanel,Mouse &mouse);
    void drawLine(glm::vec2 src, glm::vec2 dest,glm::vec2 videoScale,Panel nodeEditorPanel);
    void createConnection(int nodeIParam,int IOIParam,int currentNodeI,int currentIOI,std::vector<Node> &nodeScene);
    void clearConnections(int nodeIParam,int IOIParam,std::vector<Node> &nodeScene);
    bool doHaveConnection(int nodeIParam,int IOIParam,std::vector<Node> nodeScene);
    void addVectors(std::vector<NodeIO>& orgVec, std::vector<NodeIO>& addedVec);
    void createPanelUsingIOs();
    void getHoveredInputs(int &nodeIParam, int &IOIndexParam, std::vector<Node> nodeScene);
    int getStateData(int nodeI, int outI,std::vector<Node> nodeScene);
    void getTheIOConnectedToTheInput(int &nodeIParam, int &IOIParam,int currentNodeI, int currentIOI, std::vector<Node> nodeScene);

public:
    std::vector<NodeIO> IOs;
    Shader buttonShader;  
    Shader singleCurveShader;  
    int materialID;
    int nodeIndex; 
    Panel nodePanel;  
    Button barButton; 
    glm::vec2 scale = glm::vec2(10,20); 
    glm::vec3 pos = glm::vec3(50,50,0.8f); 

    //Constructor
    Node();

    //Public member functions
    void loadIO(std::vector<NodeIO> inputs, std::vector<NodeIO> outputs, Shader buttonShader,Shader singleCurveShader,ColorPalette colorPalette,
                int materialID,int nodeIndex);
    void render(glm::vec2 videoScale,Mouse& mouse,Timer &timer,TextRenderer &textRenderer,Panel nodeEditorPanel,std::vector<Node> &nodeScene,int currentNodeIndex);
};

//!------------------------------COLOR PALETTE-----------------------------

class ColorPalette
{
private:
    /* data */
public:
    ColorPalette(/* args */);
    
    glm::vec4 themeColor = glm::vec4(       0.043f      ,0.635f     ,0.823f     ,1.f    );  //LigidPainter's theme color which is cyan-like light blue 
    glm::vec4 mainColor = glm::vec4(        0.26        ,0.26       ,0.26       ,0.9f   );  //That color will be used oftenly
    glm::vec4 secondColor = glm::vec4(      0.16        ,0.16       ,0.16       ,1.f    );  //That too
    glm::vec4 thirdColor = glm::vec4(       0.46        ,0.46       ,0.46       ,1.f    );  //Will be used few times
    glm::vec4 oppositeColor = glm::vec4(    0.8         ,0.8        ,0.8        ,1.f    );  //Will be used for text / icon color etc.

    void newPalette(glm::vec4 themeColor,glm::vec4 mainColor,glm::vec4 secondColor,glm::vec4 thirdColor,glm::vec4 oppositeColor);
};






#endif //ELEMENTS_HPP