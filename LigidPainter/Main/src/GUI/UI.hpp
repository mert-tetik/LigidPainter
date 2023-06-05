/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

Rendering UI Elements

Currently all the UI elements are rendered to the default framebuffer.
Before rendering them bind 2D square vertex buffer.

Example :
    Box box();
    box.init();

    while(){
        box.bindBuffers();

        ..Render UI 

        box.unbindBuffers();
    }

    Define UI Elements inside of the UI class
    Render them inside of the render function of the UI class

    Position of the vertex is determined that way:
        monitor size / 100 * pos val

    *A panel is consists of sections
    *Sections consists of elements and a header
    *Elements consists of buttons, checkboxes, slide bars and more

*/

#ifndef LGDUI_HPP
#define LGDUI_HPP

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Shader.hpp"
#include "Brush.hpp"
#include "Box.hpp"
#include "Renderer.h"
#include "Util.hpp"
#include "Timer.hpp"
#include "ColorPalette.hpp"
#include "TextRendering/TextRenderer.hpp"

#include "GUI/Button.hpp"
#include "GUI/RangeBar.hpp"
#include "GUI/CheckBox.hpp"
#include "GUI/ComboBox.hpp"
#include "GUI/TextBox.hpp"

struct Element{
    //The UI Element

    int nodeState; //Used by the elements of nodes (0 : Input , 1 : Input no connection , 2 : Output , 3 : Output no connection)

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


    Element(){}

    Element(Button button){
        //Init as button
        this->button = button;
        pos = button.pos;
        scale = button.scale;
        panelOffset = button.panelOffset;
        state = 0;
    }
    Element(RangeBar rangeBar){
        //Init as range bar
        this->rangeBar = rangeBar;
        pos = rangeBar.pos;
        scale = rangeBar.scale;
        panelOffset = rangeBar.panelOffset;
        state = 1;
    }
    Element(CheckBox checkBox){
        //Init as range bar
        this->checkBox = checkBox;
        pos = checkBox.pos;
        scale = checkBox.scale;
        panelOffset = checkBox.panelOffset;
        state = 2;
    }
    Element(ComboBox comboBox,GLFWwindow* window){
        //Init as combo box
        this->comboBox = comboBox;
        this->window = window;
        pos = comboBox.pos;
        scale = comboBox.scale;
        panelOffset = comboBox.panelOffset;
        state = 3;
    }
    Element(TextBox textBox,GLFWwindow* window){
        //Init as text box
        this->textBox = textBox;
        this->window = window;
        pos = textBox.pos;
        scale = textBox.scale;
        panelOffset = textBox.panelOffset;
        state = 4;
    }


    Element(Button button,int nodeState){
        //Init as button
        this->button = button;
        pos = button.pos;
        scale = button.scale;
        panelOffset = button.panelOffset;
        state = 0;
        this->nodeState = nodeState;
    }
    Element(RangeBar rangeBar,int nodeState){
        //Init as range bar
        this->rangeBar = rangeBar;
        pos = rangeBar.pos;
        scale = rangeBar.scale;
        panelOffset = rangeBar.panelOffset;
        state = 1;
        this->nodeState = nodeState;
    }

    void render(glm::vec2 videoScale,Mouse& mouse, Timer &timer,TextRenderer &textRenderer,bool doMouseTracking){
        if(state == 0){ //Render the button
            button.pos = pos;
            button.scale = scale;
            button.panelOffset = panelOffset;
            button.doMouseTracking = doMouseTracking;
            button.render(videoScale,mouse, timer,textRenderer,doMouseTracking);
        }
        if(state == 1){ //Render the rangeBar
            rangeBar.pos = pos;
            rangeBar.scale = scale;
            rangeBar.panelOffset = panelOffset;
            rangeBar.doMouseTracking = doMouseTracking;
            rangeBar.render(videoScale,mouse, timer,textRenderer,doMouseTracking);
        }
        if(state == 2){ //Render the checkBox
            checkBox.pos = pos;
            checkBox.scale = scale;
            checkBox.panelOffset = panelOffset;
            checkBox.doMouseTracking = doMouseTracking;
            checkBox.render(videoScale,mouse, timer,textRenderer,doMouseTracking);
        }
        if(state == 3){ //Render the comboxBox
            comboBox.pos = pos;
            comboBox.scale = scale;
            comboBox.panelOffset = panelOffset;
            comboBox.doMouseTracking = doMouseTracking;
            comboBox.render(videoScale,mouse, timer,textRenderer,doMouseTracking,window);
        }
        if(state == 4){ //Render the textbox
            textBox.pos = pos;
            textBox.scale = scale;
            textBox.panelOffset = panelOffset;
            textBox.doMouseTracking = doMouseTracking;
            textBox.render(videoScale,mouse, timer,textRenderer,doMouseTracking,window);
        }
    }
};

struct Section{ //Sections seperates the elements in the panel
    
    // Header element (After pressing that button you can display other elements in that section)
    //! Is always button
    Element header; 

    std::vector<Element> elements; //Elements of that section
    
    Section(){}
    Section(Element header,std::vector<Element> elements){
        this->header = header;
        this->elements = elements;
    }
};

struct Dropper{
    glm::vec3 value;
    bool active = false;
};

#include "GUI/Panel.hpp"
#include "GUI/Dialogs/GreetingDialog.hpp"
#include "GUI/Dialogs/NewTextureDialog.hpp"
#include "GUI/Dialogs/NewProjectDialog.hpp"
#include "GUI/Dialogs/ColorPickerDialog.hpp"
#include "GUI/Dialogs/MaterialEditorDialog.hpp"
#include "GUI/Dialogs/TextureSelectionDialog.hpp"
#include "GUI/Dialogs/SettingsDialog.hpp"
#include "GUI/Dialogs/DisplayerDialog.hpp"
#include "Mouse.hpp"
#include "GUI/ContextMenu.hpp"
#include "GUI/Node/Node.hpp"
#include "GUI/Node/NodeIO.hpp"



#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>



class UI{
    Shaders shaders; 
	AppTextures appTextures;
    Model sphereModel;
public:
    //UI Elements
    //(Mostly panels)
    Panel navigationPanel;
    Panel windowPanel;
    Panel paintingPanel; //Main panel (kinda)
    Panel libraryPanelLeft; 
    Panel libraryPanelDisplayer; 
    Panel nodeEditorDisplayer; 
    Panel selectedTextureDisplayer; 
    Panel twoDPaintingPanel; 
    Panel paintingModesPanel; 

    Dropper dropper;

    //Dialogs    
    GreetingDialog greetingDialog;
    NewProjectDialog newProjectDialog;
    //ColorPickerDialog colorPickerDialog;
    MaterialEditorDialog materialEditorDialog;
    TextureSelectionDialog textureSelectionDialog;
    NewTextureDialog newTextureDialog;
    SettingsDialog settingsDialog;
    DisplayerDialog displayerDialog;

    TextBox renamingTextBox; //This textbox is used to rename library elements
    bool renamingTextBoxClosed = false;
    glm::ivec2 renamingIndices; //x for the context menu index, y for the element index
    std::string lastTitleBeforeRenaming;

    int frameCounter = 0; //Reset every 1000 frame

    int selectedLibraryElementIndex = 0; //0 For the textures , 1 for the materials bla bla
    int selectedMaterialIndex = 0; //In the library (to manipulate with )
    
    bool anyContextMenuActive = false;

    //UI Rendering projection
    //Has the screen resolution
    //Example : L = 0, R = 1920, B = 1080, T = 0  
    glm::mat4 projection; 

    ColorPalette colorPalette;

    int materialIDCounter = 0;
    
    UI(){}

    void init(Shaders shaders,Context context,AppTextures appTextures,glm::vec2 videoScale,Model &sphereModel){
        this->shaders = shaders;
        this->appTextures = appTextures;
        this->sphereModel = sphereModel;
        
        //Init the painting panel
        navigationPanel = Panel(
                                shaders.buttonShader,
                                colorPalette,
                                {
                                    Section(
                                        Element(Button()),
                                        {   
                                            Element(Button(1,glm::vec2(2,5),colorPalette,shaders.buttonShader,"File"    ,Texture(), 0.f,false)),
                                            Element(Button(1,glm::vec2(2,5),colorPalette,shaders.buttonShader,"Edit"    ,Texture(), 0.f,false)),
                                            Element(Button(1,glm::vec2(2,5),colorPalette,shaders.buttonShader,"Panels"  ,Texture(), 0.f,false)),
                                            Element(Button(1,glm::vec2(2,5),colorPalette,shaders.buttonShader,"Help"    ,Texture(), 0.f,false)),
                                        }
                                    )
                                },

                                glm::vec2(50,1), //Initial scale value
                                glm::vec3(50,1,0.1f),  //Initial position value
                                colorPalette.mainColor, //Color of the panel
                                colorPalette.thirdColor, //Color of the panel
                                false,
                                true,
                                true,
                                false,
                                true,
                                1.f,
                                1,
                                {},
                                20.f
                            );

        windowPanel  = Panel(
                                shaders.buttonShader,
                                colorPalette,

                                {
                                    Section(
                                        Element(Button()),
                                        {   
                                            Element(Button(1,glm::vec2(2,5),colorPalette,shaders.buttonShader,"3D Painting"         , appTextures.threeDIcon, 0.f,false)),
                                            Element(Button(1,glm::vec2(2,5),colorPalette,shaders.buttonShader,"2D Painting"         , appTextures.twoDIcon, 0.f,false)),
                                            Element(Button(1,glm::vec2(2,5),colorPalette,shaders.buttonShader,"Export"              , appTextures.exportIcon, 0.f,false)),
                                            Element(Button(1,glm::vec2(2,5),colorPalette,shaders.buttonShader,"Displayer"           , appTextures.environmentIcon, 0.f,false)),
                                            Element(Button(1,glm::vec2(2,5),colorPalette,shaders.buttonShader,"Settings"           , appTextures.gearIcon, 0.f,false))
                                        }
                                    )
                                },
                                
                                glm::vec2(1,48), //Initial scale value
                                glm::vec3(99,50,0.1f),  //Initial position value
                                colorPalette.mainColor, //Color of the panel
                                colorPalette.thirdColor, //Color of the panel
                                true,
                                false,
                                true,
                                true,
                                true,
                                1.f,
                                1,
                                {},
                                20.f
                            );

        paintingPanel=Panel(
                                shaders.buttonShader,
                                colorPalette,
                                {
                                    Section(
                                        Element(Button(2,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Color"  , appTextures.colorIcon, 0.f,true)),
                                        {   
                                            Element(Button(2,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Color1"  , Texture(), 1.f, true)),
                                            Element(Button(2,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Color2"  , Texture(), 1.f, true)),
                                            Element(Button(2,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Color3"  , Texture(), 1.f, true)),
                                            Element(Button(2,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Dropper"  , Texture(), 1.f, false))
                                        }
                                    ),
                                    Section(
                                        Element(Button(2,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Brush"  , appTextures.brushIcon, 3.f,true)),
                                        {   
                                            Element(RangeBar(0,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Radius    "  , appTextures.TDModelIcon, 1.f,0.f,100.f,50.f)),
                                            Element(RangeBar(0,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Opacity   "  , appTextures.TDModelIcon, 1.f,0.f,100.f,50.f)),
                                            Element(RangeBar(0,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Hardness  "  , appTextures.TDModelIcon, 1.f,-100.f,100.f,0.f)),
                                            Element(RangeBar(0,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Spacing   "  , appTextures.TDModelIcon, 1.f,0.f,100.f,0.f)),
                                        }
                                    ),
                                    Section(
                                        Element(Button(2,glm::vec2(2,1),colorPalette,shaders.buttonShader, "More"  , appTextures.modifierIcon, 1.f,true)),
                                        {   
                                            Element(RangeBar(0,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Size Jitter"  , Texture(), 1.f,0.f,100.f,0.f)), //0
                                            Element(RangeBar(0,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Fade"  , Texture(), 1.f,0.f,100.f,0.f)),//1
                                            Element(CheckBox(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Sin Wave Pattern"  , 1.f)),//2
                                            Element(RangeBar(0,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Scatter"  , Texture(), 1.f,0.f,100.f,0.f)),//3
                                            Element(Button(1,glm::vec2(2,4),colorPalette,shaders.buttonShader, "Texture"  , appTextures.greetingDialogImage, 1.f,true)),//4
                                            Element(Button(1,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Remove"  , Texture(), 0.f,false)),//5
                                            Element(CheckBox(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Individual Texture"  , 1.f)),//6
                                            Element(RangeBar(0,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Rotation"  , Texture(), 1.f,0.f,360.f,0.f)), //7
                                            Element(RangeBar(0,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Rotation Jitter"  , Texture(), 1.f,0.f,100.f,0.f)), //8
                                            Element(RangeBar(0,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Alpha Jitter"  , Texture(), 1.f,0.f,100.f,0.f)), //9
                                            Element(Button(2,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Create New Brush"  , Texture(), 2.f,false)),//10
                                            Element(Button(2,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Export"  , Texture(), 0.5f,false)),//11
                                        }
                                    ),
                                    Section(
                                        Element(Button(2,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Soften"  , appTextures.softenIcon, 3.f,true)),
                                        {   
                                            Element(Button(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Radius"  , appTextures.TDModelIcon, 1.f,false)),
                                            Element(RangeBar(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Strength"  , appTextures.TDModelIcon, 1.f,0.f,100.f,50.f)),
                                        }
                                    ),
                                    Section(
                                        Element(Button(2,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Smear"  , appTextures.smearIcon, 3.f,true)),
                                        {   
                                            Element(Button(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Radius"  , appTextures.TDModelIcon, 1.f,false)),
                                            Element(RangeBar(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Strength"  , appTextures.TDModelIcon, 1.f,0.f,100.f,50.f)),
                                        }
                                    ),
                                    Section(
                                        Element(Button(2,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Area"  , appTextures.areaPaintingIcon, 3.f,true)),
                                        {   
                                            Element(Button(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Box"  , appTextures.TDModelIcon, 1.f,false)),
                                            Element(CheckBox(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Gradient"  , 1.f)),
                                            Element(CheckBox(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Depth Test"  , 1.f)),
                                            Element(RangeBar(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Lasso"  , appTextures.TDModelIcon, 1.f,0.f,100.f,50.f)),
                                            Element(RangeBar(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Polygonal Lasso"  , appTextures.TDModelIcon, 1.f,0.f,100.f,50.f)),
                                            Element(RangeBar(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Circle"  , appTextures.TDModelIcon, 1.f,0.f,100.f,50.f)),
                                        }
                                    ),
                                },
                                
                                glm::vec2(5,48), //Initial scale value
                                glm::vec3(50,50,0.1f),  //Initial position value
                                colorPalette.mainColor, //Color of the panel
                                colorPalette.thirdColor, //Color of the panel
                                true,
                                false,
                                true,
                                true,
                                true,
                                1.f,
                                1,
                                {},
                                20.f
                            );
        
        libraryPanelLeft  = Panel(
                                shaders.buttonShader,
                                colorPalette,
                                {
                                    Section(
                                        Element(Button()),
                                        {   
                                            Element(Button(1,glm::vec2(2,1.5f),colorPalette,shaders.buttonShader,"Textures"        , Texture(), 0.f,true)),
                                            Element(Button(1,glm::vec2(2,1.5f),colorPalette,shaders.buttonShader,"Materials"       , Texture(), 0.f,true)),
                                            Element(Button(1,glm::vec2(2,1.5f),colorPalette,shaders.buttonShader,"Brushes"         , Texture(), 0.f,true)),
                                            Element(Button(1,glm::vec2(2,1.5f),colorPalette,shaders.buttonShader,"Fonts"           , Texture(), 0.f,true)),
                                            Element(Button(1,glm::vec2(2,1.5f),colorPalette,shaders.buttonShader,"Scripts"         , Texture(), 0.f,true)),
                                            Element(Button(1,glm::vec2(2,1.5f),colorPalette,shaders.buttonShader,"Filters"         , Texture(), 0.f,true)),
                                            Element(Button(1,glm::vec2(2,1.5f),colorPalette,shaders.buttonShader,"Layers"          , Texture(), 0.f,true)),
                                            Element(Button(1,glm::vec2(2,1.5f),colorPalette,shaders.buttonShader,"3D Models"       , Texture(), 0.f,true)),
                                        }
                                    )
                                },
                                
                                glm::vec2(1,48), //Initial scale value
                                glm::vec3(1,50,0.1f),  //Initial position value
                                colorPalette.mainColor, //Color of the panel
                                colorPalette.thirdColor, //Color of the panel
                                true,
                                true,
                                false,
                                true,
                                true,
                                1.f,
                                1,
                                {},
                                20.f
                            );
        libraryPanelDisplayer  = Panel(
                                shaders.buttonShader,
                                colorPalette,
                                {
                                    {}
                                },
                                
                                glm::vec2(1,48), //Initial scale value
                                glm::vec3(1,50,0.1f),  //Initial position value
                                colorPalette.mainColor, //Color of the panel
                                colorPalette.thirdColor, //Color of the panel
                                true,
                                true,
                                false,
                                true,
                                true,
                                1.f,
                                2,
                                {
                                    Button(1,glm::vec2(2,1.5f),colorPalette,shaders.buttonShader,"Add"        , Texture(), 0.f,false),
                                    Button(1,glm::vec2(2,1.5f),colorPalette,shaders.buttonShader,"Del"        , Texture(), 0.f,false)
                                },
                                11.f
                            );
        nodeEditorDisplayer  = Panel(
                                shaders.buttonShader,
                                colorPalette,
                                {
                                    {}
                                },
                                
                                glm::vec2(25 ,5), //Initial scale value
                                glm::vec3(50 ,95 ,0.1f),  //Initial position value
                                colorPalette.mainColor, //Color of the panel
                                colorPalette.thirdColor, //Color of the panel
                                true,
                                false,
                                true,
                                true,
                                false,
                                1.f,
                                1,
                                {},
                                45.f
                            );
        selectedTextureDisplayer  = Panel(
                                shaders.buttonShader,
                                colorPalette,
                                {
                                    Section(
                                        Element(Button()),
                                        {   
                                            Element(Button(1,glm::vec2(2,5.5f),colorPalette,shaders.buttonShader,""        , appTextures.greetingDialogImage, 0.f,false)),
                                        }
                                    )
                                },
                                
                                glm::vec2(50 ,5), //Initial scale value
                                glm::vec3(50 ,95 ,0.1f),  //Initial position value
                                colorPalette.mainColor, //Color of the panel
                                colorPalette.thirdColor, //Color of the panel
                                true,
                                true,
                                false,
                                true,
                                false,
                                1.f,
                                1,
                                {},
                                20.f
                            );
        twoDPaintingPanel  = Panel(
                                shaders.buttonShader,
                                colorPalette,
                                {
                                    Section(
                                        Element(Button()),
                                        {   
                                            Element(Button(1,glm::vec2(2,5.5f),colorPalette,shaders.buttonShader,""        , appTextures.greetingDialogImage, 0.f,false)),
                                        }
                                    )
                                },
                                
                                glm::vec2(50 ,5), //Initial scale value
                                glm::vec3(50 ,95 ,0.1f),  //Initial position value
                                colorPalette.mainColor, //Color of the panel
                                colorPalette.thirdColor, //Color of the panel
                                true,
                                true,
                                false,
                                true,
                                false,
                                1.f,
                                1,
                                {},
                                20.f
                            );
        paintingModesPanel  = Panel(
                                shaders.buttonShader,
                                colorPalette,
                                {
                                    Section(
                                        Element(Button()),
                                        {   
                                            Element(Button(1,glm::vec2(2,2.f),colorPalette,shaders.buttonShader,"Draw"          , appTextures.brushIcon,        0.f,true)),
                                            Element(Button(1,glm::vec2(2,2.f),colorPalette,shaders.buttonShader,"Soften"        , appTextures.softenIcon,       0.f,true)),
                                            Element(Button(1,glm::vec2(2,2.f),colorPalette,shaders.buttonShader,"Smear"         , appTextures.smearIcon,        0.f,true)),
                                            Element(Button(1,glm::vec2(2,2.f),colorPalette,shaders.buttonShader,"Area"          , appTextures.areaPaintingIcon, 0.f,true)),
                                        }
                                    )
                                },
                                
                                glm::vec2(5 ,8), //Initial scale value
                                glm::vec3(50 ,50 ,0.1f),  //Initial position value
                                colorPalette.mainColor, //Color of the panel
                                colorPalette.thirdColor, //Color of the panel
                                true,
                                true,
                                false,
                                true,
                                false,
                                1.f,
                                1,
                                {},
                                20.f
                            );
        

        greetingDialog = GreetingDialog(context,videoScale,colorPalette,shaders.buttonShader,appTextures);
        newTextureDialog = NewTextureDialog(context,videoScale,colorPalette,shaders.buttonShader,appTextures);
        displayerDialog = DisplayerDialog(context,videoScale,colorPalette,shaders.buttonShader,appTextures);
        newProjectDialog = NewProjectDialog(context,videoScale,colorPalette,shaders.buttonShader,appTextures);
        //colorPickerDialog = ColorPickerDialog(context,videoScale,colorPalette,shaders.buttonShader,shaders.colorPicker,appTextures);
        materialEditorDialog = MaterialEditorDialog(shaders.buttonShader,shaders.tdModelShader,colorPalette,appTextures,sphereModel);
        textureSelectionDialog = TextureSelectionDialog(shaders.buttonShader,colorPalette);
        settingsDialog = SettingsDialog(context,videoScale,colorPalette,shaders.buttonShader,appTextures);

        for (size_t i = 0; i < 3; i++)
        {
            paintingPanel.sections[0].elements[i].button.outlineThickness = 5.f;
            paintingPanel.sections[0].elements[i].button.animationStyle = 2;
        }
        
        libraryPanelDisplayer.isLibraryDisplayer = true;

        renamingTextBox = TextBox(0,glm::vec2(4,2),colorPalette,shaders.buttonShader,"",4.f,false),context.window;
        renamingTextBox.thickness = 0.f;
        renamingTextBox.animationStyle = 1;
        renamingTextBox.radius = 10000.f;
    
        frameCounter++;
    }    

    void render(glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer,Context context,Box box,Library &library,std::vector<Node> &appNodes,std::vector<Node> &nodeScene,
                std::vector<ContextMenu> &contextMenus,int &textureRes, Project &project, Painter &painter,bool &VSync,Skybox &skybox){
        
        glDepthFunc(GL_LEQUAL);

        //Give projection to the curve shader        
        shaders.singleCurve.use();
        shaders.singleCurve.setMat4("projection",projection); 
        
        //Give projection to the color picker shader 
        //!Color picker shader is not used rn        
        shaders.colorPicker.use();
        shaders.colorPicker.setMat4("projection",projection); 

        //Use the related shader
        shaders.buttonShader.use();
        
        //Projection is changing in the renderer.render()
        //userInterface.projection = glm::mat4(...)
        shaders.buttonShader.setMat4("projection",projection); 

        float screenGap = videoScale.x - context.windowScale.x; //Use that value to keep panels on the right side
        float screenGapPerc = screenGap / videoScale.x * 100.f; 

        //TODO Don't call that everyframe (maybe)
        library.uniqueNameControl();

        //!Render all the UI elements
        navigationPanel.render(videoScale,mouse,timer,textRenderer,!(textureSelectionDialog.active || materialEditorDialog.active || anyContextMenuActive));
        windowPanel.render(videoScale,mouse,timer,textRenderer,!(textureSelectionDialog.active || materialEditorDialog.active || anyContextMenuActive));
        paintingPanel.render(videoScale,mouse,timer,textRenderer,!(textureSelectionDialog.active || materialEditorDialog.active || anyContextMenuActive));
        libraryPanelLeft.render(videoScale,mouse,timer,textRenderer,!(textureSelectionDialog.active || materialEditorDialog.active || anyContextMenuActive));
        libraryPanelDisplayer.render(videoScale,mouse,timer,textRenderer,!(textureSelectionDialog.active || materialEditorDialog.active || anyContextMenuActive));
        nodeEditorDisplayer.render(videoScale,mouse,timer,textRenderer,!(textureSelectionDialog.active || materialEditorDialog.active || anyContextMenuActive));
        selectedTextureDisplayer.render(videoScale,mouse,timer,textRenderer,!(textureSelectionDialog.active || materialEditorDialog.active || anyContextMenuActive));
        if(!painter.threeDimensionalMode){
            twoDPaintingPanel.render(videoScale,mouse,timer,textRenderer,!(textureSelectionDialog.active || materialEditorDialog.active || anyContextMenuActive));
            
            //Render the painting texture
            shaders.tdModelShader.use();
            shaders.tdModelShader.setInt("render2D",0);
            shaders.tdModelShader.setInt("useTransformUniforms",1);
            shaders.tdModelShader.setMat4("orthoProjection",projection);

            shaders.tdModelShader.setInt("returnSingleTxtr",1);

            //Bind the selected texture as albedo
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D,library.textures[painter.selectedTextureIndex].ID);

            shaders.tdModelShader.setVec2("scale",glm::vec2(min(twoDPaintingPanel.sections[0].elements[0].button.resultScale.x,twoDPaintingPanel.sections[0].elements[0].button.resultScale.y)));
            shaders.tdModelShader.setVec3("pos",twoDPaintingPanel.sections[0].elements[0].button.resultPos);
            glDrawArrays(GL_TRIANGLES,0,6);

            painter.windowProjection = projection;
            painter.scale2D = glm::vec2(min(twoDPaintingPanel.sections[0].elements[0].button.resultScale.x,twoDPaintingPanel.sections[0].elements[0].button.resultScale.y));
            painter.pos2D = twoDPaintingPanel.sections[0].elements[0].button.resultPos;

            shaders.tdModelShader.setInt("returnSingleTxtr",0);
            shaders.tdModelShader.setInt("useTransformUniforms",0);
            shaders.buttonShader.use();
        }
        paintingModesPanel.render(videoScale,mouse,timer,textRenderer,!(textureSelectionDialog.active || materialEditorDialog.active || anyContextMenuActive));

        if(renamingTextBox.active){
            renamingTextBox.render(videoScale,mouse,timer,textRenderer,false,context.window);
            renamingTextBoxClosed = false;
        }
        else{
            //The first frame renamingTextBox is closed 
            if(!renamingTextBoxClosed){
                if(glfwGetKey(context.window,GLFW_KEY_ESCAPE) == GLFW_RELEASE){
                    if(renamingIndices.x == 0)
                        library.textures[renamingIndices.y].title = renamingTextBox.text;
                    else if(renamingIndices.x == 1)
                        library.materials[renamingIndices.y].title = renamingTextBox.text;
                    else if(renamingIndices.x == 2)
                        library.brushes[renamingIndices.y].title = renamingTextBox.text;
                }
                else{
                    if(renamingIndices.x == 0)
                        library.textures[renamingIndices.y].title = lastTitleBeforeRenaming;
                    else if(renamingIndices.x == 1)
                        library.materials[renamingIndices.y].title = lastTitleBeforeRenaming;
                    else if(renamingIndices.x == 2)
                        library.brushes[renamingIndices.y].title = lastTitleBeforeRenaming;
                }
            }

            renamingTextBoxClosed = true;
        }

        Util util;

        if(paintingPanel.sections[2].elements[5].button.hover && mouse.LClick){ //If pressed to remove the brush texture button
            painter.brushTexture = Texture(0);
        }
        if(paintingPanel.sections[2].elements[11].button.hover && mouse.LClick){ //If pressed to export brush file button
            Brush exportBrush
                            (    
                                1.f - paintingPanel.sections[2].elements[0].rangeBar.value/100.f,
                                1.f - paintingPanel.sections[2].elements[3].rangeBar.value/100.f,
                                1.f - paintingPanel.sections[2].elements[1].rangeBar.value/100.f,
                                paintingPanel.sections[2].elements[7].rangeBar.value,
                                1.f - paintingPanel.sections[2].elements[8].rangeBar.value/100.f,
                                1.f - paintingPanel.sections[2].elements[9].rangeBar.value/100.f,
                                paintingPanel.sections[2].elements[6].checkBox.clickState1,
                                paintingPanel.sections[2].elements[2].checkBox.clickState1,
                                "brush_1",
                                painter.brushTexture
                            );

            exportBrush.saveFile();
        }


        //Update the selected texture
        if(selectedLibraryElementIndex == 0){ //Textures selected
            for (size_t i = 0; i < libraryPanelDisplayer.sections[0].elements.size(); i++) //Check all the texture button elements from the library displayer panel
            {
                if(libraryPanelDisplayer.sections[0].elements[i].button.hover && mouse.LClick){
                    if(paintingPanel.sections[2].elements[4].button.clickState1){//If brush texture displayer is pressed
                        painter.brushTexture = libraryPanelDisplayer.sections[0].elements[i].button.texture; //Select a brush texture
                        paintingPanel.sections[2].elements[4].button.clickState1 = false;
                    } 
                    else
                        painter.selectedTextureIndex = i; //Select the texture 
                } //If any texture button element is pressed

                if(i == painter.selectedTextureIndex) //Highlight the selected texture
                    libraryPanelDisplayer.sections[0].elements[i].button.clickState1 = true;
            }
        }
        
        //Check all the library element button is they are pressed
        for (size_t i = 0; i < libraryPanelLeft.sections[0].elements.size(); i++) 
        {
            if(libraryPanelLeft.sections[0].elements[i].button.hover && mouse.LClick){//If any button element is pressed
                if(selectedLibraryElementIndex != i){
                    libraryPanelLeft.sections[0].elements[selectedLibraryElementIndex].button.clickState1 = false;
                    selectedLibraryElementIndex = i;
                    break;
                }
                
            } 
        }


        elementInteraction(painter,mouse,library,contextMenus,appNodes,nodeScene,context,videoScale,textRenderer,timer,textureRes,screenGapPerc);
        

        selectedTextureDisplayer.sections[0].elements[0].scale.y = selectedTextureDisplayer.scale.y;
        selectedTextureDisplayer.sections[0].elements[0].button.texture = library.textures[painter.selectedTextureIndex];
        twoDPaintingPanel.sections[0].elements[0].scale.y = twoDPaintingPanel.scale.y;
        
        //Render the nodes
        for (size_t i = 0; i < nodeScene.size(); i++)
        {
            //Update the display texture of the material node
            if(nodeScene[i].nodeIndex == 1){//Is a material node
                for (size_t matI = 0; matI < library.materials.size(); matI++)
                {
                    if(nodeScene[i].materialID == library.materials[matI].ID)
                        nodeScene[i].nodePanel.sections[0].elements[0].button.texture = Texture(library.materials[matI].displayingTexture);
                }
            }
            nodeScene[i].render(videoScale,mouse,timer,textRenderer,nodeEditorDisplayer,nodeScene,i);
        }

        
        //Dialogs
        
        //greetingDialog.render(context.window,colorPalette,mouse,timer,textRenderer,videoScale);
        //newProjectDialog.render(context.window,colorPalette,mouse,timer,textRenderer,videoScale,project);
        //colorPickerDialog.render(context.window,colorPalette,mouse,timer,textRenderer,videoScale,project);
        
        displayerDialog.render(context.window,colorPalette,mouse,timer,textRenderer,library,videoScale,skybox);
        
        if(newTextureDialog.active)
            newTextureDialog.render(context.window,colorPalette,mouse,timer,textRenderer,library,videoScale,textureRes);
        
        if(settingsDialog.active)
            settingsDialog.render(context.window,colorPalette,mouse,timer,textRenderer,library,videoScale,textureRes,VSync);
        
        if(materialEditorDialog.active && library.materials.size()){
            if(glfwGetKey(context.window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
                materialEditorDialog.deactivate(textureSelectionDialog);
            materialEditorDialog.render(videoScale,mouse,timer,textRenderer,textureSelectionDialog,library,library.materials[selectedMaterialIndex],textureRes,box,context);
        }
        
        if(textureSelectionDialog.active)
            textureSelectionDialog.render(videoScale,mouse,timer,textRenderer,library);

        if(frameCounter > 1000)
            frameCounter = 0;

        //!Make sure the execute these after rendering everything
        if(mouse.LClick && dropper.active){
            //Dropper active pick color
            glm::vec3 cursorHoverPixelRGBData;
            //Read the cursor position from the default frame buffer
            glReadPixels(mouse.cursorPos.x,mouse.cursorPos.y,1,1,GL_RGB,GL_FLOAT,&cursorHoverPixelRGBData);
            
            dropper.value = cursorHoverPixelRGBData; 
        
            dropper.active = false;

            if(painter.selectedColorIndex == 0)
                painter.color1.loadRGB(dropper.value*glm::vec3(255));
            if(painter.selectedColorIndex == 1)
                painter.color2.loadRGB(dropper.value*glm::vec3(255));
            if(painter.selectedColorIndex == 2)
                painter.color3.loadRGB(dropper.value*glm::vec3(255));
        }
        
        //If clicked to the dropper button activate the dropper
        if(paintingPanel.sections[0].elements[3].button.hover && mouse.LClick){
            dropper.active = true;
        }
    }







private: 
    void elementInteraction(Painter &painter,Mouse &mouse, Library &library,std::vector<ContextMenu> &contextMenus,std::vector<Node> &appNodes,std::vector<Node> &nodeScene,
                            Context &context,glm::vec2 &videoScale,TextRenderer &textRenderer, Timer &timer, int &textureRes,float screenGapPerc){

        //!PAINTING PANEL COLOR BUTTONS
        if(paintingPanel.sections[0].elements[0].button.hover && mouse.LDoubleClick){//Pressed to first color button element
            painter.loadColor1();
        }
        if(paintingPanel.sections[0].elements[1].button.hover && mouse.LDoubleClick){//Pressed to second color button element
            painter.loadColor2();
        }
        if(paintingPanel.sections[0].elements[2].button.hover && mouse.LDoubleClick){//Pressed to third color button element
            painter.loadColor3();
        }

        //Prevent multiple selection and update the painter.selectedColorIndex for colors
        for (size_t i = 0; i < paintingPanel.sections[0].elements.size(); i++)
        {
            if(i == 3) 
                break; //Don't bring the dropper button
            if(paintingPanel.sections[0].elements[i].button.clickState1){ //If a color button is clicked
                if(painter.selectedColorIndex != i){ //If the clicked button is not selected 
                    paintingPanel.sections[0].elements[painter.selectedColorIndex].button.clickState1 = false; //Unselect the selected one
                    painter.selectedColorIndex = i; //Select the clicked color button
                    break; 
                }
            }
        }

        //Update the color values of the color buttons
        paintingPanel.sections[0].elements[0].button.color = glm::vec4(painter.color1.RGB/glm::vec3(255.f),1.f);
        paintingPanel.sections[0].elements[1].button.color = glm::vec4(painter.color2.RGB/glm::vec3(255.f),1.f);
        paintingPanel.sections[0].elements[2].button.color = glm::vec4(painter.color3.RGB/glm::vec3(255.f),1.f);
        
        //Update the brush texture displayer's texture
        paintingPanel.sections[2].elements[4].button.texture = painter.brushTexture;



        //!WINDOW PANEL BUTTONS
        if(windowPanel.sections[0].elements[0].button.hover && mouse.LClick){//Pressed to the 3D painting button of the window panel
            painter.threeDimensionalMode = true;
        }
        else if(windowPanel.sections[0].elements[1].button.hover && mouse.LClick){//Pressed to the 2D painting button of the window panel
            painter.threeDimensionalMode = false;
        }
        else if(windowPanel.sections[0].elements[4].button.hover && mouse.LClick){//Pressed to the settings button of the window panel
            settingsDialog.activate();
        }



        //!CONTEXT MENU
        anyContextMenuActive = false; 
        for (size_t i = 0; i < contextMenus.size(); i++)//Check all the contextMenus
        {
            if(contextMenus[i].active) //Set anyContextMenuActive UI class variable 
                anyContextMenuActive = true;
            
            //CONTEXT MENU BUTTONS
            if(i == 0 && selectedLibraryElementIndex == 0 && contextMenus[i].active){ //If texture context menu is active
                if(contextMenus[i].contextPanel.sections[0].elements[0].button.hover && mouse.LClick){//Clicked to rename button
                    renamingTextBox.active = true;
                    renamingTextBox.pos = libraryPanelDisplayer.sections[0].elements[contextMenus[i].selectedElement].button.pos;
                    renamingTextBox.pos.y += 2.8f;
                    lastTitleBeforeRenaming = library.textures[contextMenus[i].selectedElement].title;
                    library.textures[contextMenus[i].selectedElement].title = "";
                    renamingTextBox.text = libraryPanelDisplayer.sections[0].elements[contextMenus[i].selectedElement].button.text;
                    renamingTextBox.activeChar = renamingTextBox.text.size();
                    renamingTextBox.activeChar2 = renamingTextBox.activeChar;
                    renamingIndices.x = 0;
                    renamingIndices.y = contextMenus[i].selectedElement;
                }
                if(contextMenus[i].contextPanel.sections[0].elements[1].button.hover && mouse.LClick){//Clicked to duplicate button
                    Texture duplicatedTexture;
                    duplicatedTexture = library.textures[contextMenus[i].selectedElement];

                    duplicatedTexture.ID = library.textures[contextMenus[i].selectedElement].duplicateTexture();

                    library.textures.push_back(duplicatedTexture);
                }
                if(contextMenus[i].contextPanel.sections[0].elements[3].button.hover && mouse.LClick){//Clicked to delete button
                    library.textures.erase(library.textures.begin() + contextMenus[i].selectedElement);
                }
            }
            
            if(i == 1 && selectedLibraryElementIndex == 1 && contextMenus[i].active){ //If material context menu is active
                if(contextMenus[i].contextPanel.sections[0].elements[0].button.hover && mouse.LClick){//Clicked to edit button
                    //Select the material that material editor will edit & show the material editor dialog
                    selectedMaterialIndex = contextMenus[i].selectedElement;
                    materialEditorDialog.activate();
                }
                if(contextMenus[i].contextPanel.sections[0].elements[1].button.hover && mouse.LClick){//Clicked to add to scene button
                    //Create the node of the materail an add to the node scene
                    Node materialNode = appNodes[0];
                    materialNode.barButton.text = library.materials[contextMenus[i].selectedElement].title;
                    materialNode.materialID = library.materials[contextMenus[i].selectedElement].ID;
                    nodeScene.push_back(materialNode); //Add material node
                }
                if(contextMenus[i].contextPanel.sections[0].elements[2].button.hover && mouse.LClick){//Clicked to rename button
                    renamingTextBox.active = true;
                    renamingTextBox.pos = libraryPanelDisplayer.sections[0].elements[contextMenus[i].selectedElement].button.pos;
                    renamingTextBox.pos.y += 2.8f;
                    lastTitleBeforeRenaming = library.materials[contextMenus[i].selectedElement].title;
                    library.materials[contextMenus[i].selectedElement].title = "";
                    renamingTextBox.text = libraryPanelDisplayer.sections[0].elements[contextMenus[i].selectedElement].button.text;
                    renamingTextBox.activeChar = renamingTextBox.text.size();
                    renamingTextBox.activeChar2 = renamingTextBox.activeChar;
                    renamingIndices.x = 1;
                    renamingIndices.y = contextMenus[i].selectedElement;
                }
                if(contextMenus[i].contextPanel.sections[0].elements[3].button.hover && mouse.LClick){//Clicked to duplicate button
                    Material duplicatedMaterial;
                    duplicatedMaterial = library.materials[contextMenus[i].selectedElement];

                    duplicatedMaterial.ID = materialIDCounter;

                    library.materials.push_back(duplicatedMaterial);
                    
                    materialIDCounter++;
                }
                if(contextMenus[i].contextPanel.sections[0].elements[5].button.hover && mouse.LClick){//Clicked to delete button
                    library.materials.erase(library.materials.begin() + contextMenus[i].selectedElement);
                }
            }

            if(i == 2 && selectedLibraryElementIndex == 2 && contextMenus[i].active){ //If brush context menu is active
                if(contextMenus[i].contextPanel.sections[0].elements[0].button.hover && mouse.LClick){//Clicked to use brush button
                    paintingPanel.sections[2].elements[0].rangeBar.value        =   library.brushes[contextMenus[i].selectedElement].sizeJitter;
                    paintingPanel.sections[2].elements[3].rangeBar.value        =   library.brushes[contextMenus[i].selectedElement].scatter;
                    paintingPanel.sections[2].elements[1].rangeBar.value        =   library.brushes[contextMenus[i].selectedElement].fade;
                    paintingPanel.sections[2].elements[7].rangeBar.value        =   library.brushes[contextMenus[i].selectedElement].rotation;
                    paintingPanel.sections[2].elements[8].rangeBar.value        =   library.brushes[contextMenus[i].selectedElement].rotationJitter;
                    paintingPanel.sections[2].elements[9].rangeBar.value        =   library.brushes[contextMenus[i].selectedElement].alphaJitter;
                    paintingPanel.sections[2].elements[6].checkBox.clickState1  =   library.brushes[contextMenus[i].selectedElement].individualTexture;
                    paintingPanel.sections[2].elements[2].checkBox.clickState1  =   library.brushes[contextMenus[i].selectedElement].sinWavePattern;
                }
                if(contextMenus[i].contextPanel.sections[0].elements[1].button.hover && mouse.LClick){//Clicked to apply brush settings
                    library.brushes[contextMenus[i].selectedElement].sizeJitter         =   paintingPanel.sections[2].elements[0].rangeBar.value;          
                    library.brushes[contextMenus[i].selectedElement].scatter            =   paintingPanel.sections[2].elements[3].rangeBar.value;          
                    library.brushes[contextMenus[i].selectedElement].fade               =   paintingPanel.sections[2].elements[1].rangeBar.value;          
                    library.brushes[contextMenus[i].selectedElement].rotation           =   paintingPanel.sections[2].elements[7].rangeBar.value;          
                    library.brushes[contextMenus[i].selectedElement].rotationJitter     =   paintingPanel.sections[2].elements[8].rangeBar.value;          
                    library.brushes[contextMenus[i].selectedElement].alphaJitter        =   paintingPanel.sections[2].elements[9].rangeBar.value;          
                    library.brushes[contextMenus[i].selectedElement].individualTexture  =   paintingPanel.sections[2].elements[6].checkBox.clickState1;    
                    library.brushes[contextMenus[i].selectedElement].sinWavePattern     =   paintingPanel.sections[2].elements[2].checkBox.clickState1;    
                }
                if(contextMenus[i].contextPanel.sections[0].elements[2].button.hover && mouse.LClick){//Clicked to rename button
                    renamingTextBox.active = true;
                    renamingTextBox.pos = libraryPanelDisplayer.sections[0].elements[contextMenus[i].selectedElement].button.pos;
                    renamingTextBox.pos.y += 2.8f;
                    lastTitleBeforeRenaming = library.brushes[contextMenus[i].selectedElement].title;
                    library.brushes[contextMenus[i].selectedElement].title = "";
                    renamingTextBox.text = libraryPanelDisplayer.sections[0].elements[contextMenus[i].selectedElement].button.text;
                    renamingTextBox.activeChar = renamingTextBox.text.size();
                    renamingTextBox.activeChar2 = renamingTextBox.activeChar;
                    renamingIndices.x = 2;
                    renamingIndices.y = contextMenus[i].selectedElement;
                }
                if(contextMenus[i].contextPanel.sections[0].elements[3].button.hover && mouse.LClick){//Clicked to duplicate button
                    library.brushes.push_back(library.brushes[contextMenus[i].selectedElement]);
                }
                if(contextMenus[i].contextPanel.sections[0].elements[5].button.hover && mouse.LClick){//Clicked to delete button
                    library.brushes.erase(library.brushes.begin() + contextMenus[i].selectedElement);
                }
            }

            if (   //Conditions to turn any context menu off
                    mouse.LClick|| //Mouse left click
                    mouse.RClick|| //Mouse right click
                    mouse.MClick|| //Mouse middle click
                    glfwGetKey(context.window,GLFW_KEY_ESCAPE) == GLFW_PRESS|| //Pressed to escape key 
                    glfwGetKey(context.window,GLFW_KEY_ENTER) == GLFW_PRESS //Pressed to enter key
                )
            {
               contextMenus[i].active = false; //Turn the context menu offs
            }

            //Render the context menu if active
            if(contextMenus[i].active){
                contextMenus[i].render(videoScale,mouse,timer,textRenderer);
                if(contextMenus[i].contextPanel.hover == false)
                    contextMenus[i].active = false; 
            }
            else{
                contextMenus[i].selectedElement = 0;
            }
        }

        //*Show context menu
        //For library panel displayer
        for (size_t i = 0; i < libraryPanelDisplayer.sections[0].elements.size(); i++)
        {
            if(libraryPanelDisplayer.sections[0].elements[i].button.hover && mouse.RClick){ //Right clicked to an element
                if(selectedLibraryElementIndex == 0){//To a texture
                    //Show the context menu
                    contextMenus[0].active = true;
                    contextMenus[0].pos.x = libraryPanelDisplayer.sections[0].elements[i].button.pos.x;
                    contextMenus[0].pos.y = libraryPanelDisplayer.sections[0].elements[i].button.pos.y;
                    contextMenus[0].pos.z = 0.95f;
                    contextMenus[0].selectedElement = i;
                    
                }
                if(selectedLibraryElementIndex == 1){//To a material
                    //Show the context menu
                    contextMenus[1].active = true;
                    contextMenus[1].pos.x = libraryPanelDisplayer.sections[0].elements[i].button.pos.x;
                    contextMenus[1].pos.y = libraryPanelDisplayer.sections[0].elements[i].button.pos.y;
                    contextMenus[1].pos.z = 0.95f;
                    contextMenus[1].selectedElement = i;
                }
                if(selectedLibraryElementIndex == 2){//To a brush
                    //Show the context menu
                    contextMenus[2].active = true;
                    contextMenus[2].pos.x = libraryPanelDisplayer.sections[0].elements[i].button.pos.x;
                    contextMenus[2].pos.y = libraryPanelDisplayer.sections[0].elements[i].button.pos.y;
                    contextMenus[2].pos.z = 0.95f;
                    contextMenus[2].selectedElement = i;
                }
            }
        }


        //!LIBRARY PANEL DISPLAYER
        //Update the library displayer panel every frame
        //if(frameCounter % 100 == 0){
        if(true){
            libraryPanelDisplayer.sections.clear(); //Remove all the elements of the library panel displayer
            
            //Create a new section
            Section libSection;
            libSection.header = Element(Button()); //Has no section button

            //Fill the elements of the section using the data in the library structure
            if(selectedLibraryElementIndex == 0){//Update textures
                for (size_t i = 0; i < library.textures.size(); i++)
                {
                    //Push texture elements into the section
                    libSection.elements.push_back(Element(Button(1,glm::vec2(2,4.f),colorPalette,shaders.buttonShader,library.textures[i].title       , library.textures[i], 0.f,false))) ;
                }
            }
            else if(selectedLibraryElementIndex == 1){ //Update materials
                for (size_t i = 0; i < library.materials.size(); i++)
                {
                    //Push texture elements into the section
                    libSection.elements.push_back(Element(Button(1,glm::vec2(2,4.f),colorPalette,shaders.buttonShader,library.materials[i].title       , Texture(library.materials[i].displayingTexture), 0.f,false))) ;
                }
            }
            else if(selectedLibraryElementIndex == 2){ //Update materials
                for (size_t i = 0; i < library.brushes.size(); i++)
                {
                    //Push texture elements into the section
                    libSection.elements.push_back(Element(Button(1,glm::vec2(2,4.f),colorPalette,shaders.buttonShader,library.brushes[i].title       , Texture(library.brushes[i].displayingTexture), 0.f,false))) ;
                }
            }
            //Give the section
            libraryPanelDisplayer.sections.push_back(Section(Element(Button()),libSection.elements));
        }

        //Add button from the barButtons in the library displayer panel clicked 
        if(libraryPanelDisplayer.barButtons[0].clickedMixVal == 1.f){
            if(selectedLibraryElementIndex == 0){//Textures
                newTextureDialog.active = true;
            }
            if(selectedLibraryElementIndex == 1){ //Materials
                //Add new material to the library & not the panel
                //Will be displayed right after library panel is updated which happens in every 100 frame
                library.materials.push_back(Material(textureRes,"material_0",materialIDCounter));
                materialIDCounter++;
            }
            if(selectedLibraryElementIndex == 2){ //Brushes
                library.brushes.push_back(
                                            Brush
                                                (    
                                                    paintingPanel.sections[2].elements[0].rangeBar.value,
                                                    paintingPanel.sections[2].elements[3].rangeBar.value,
                                                    paintingPanel.sections[2].elements[1].rangeBar.value,
                                                    paintingPanel.sections[2].elements[7].rangeBar.value,
                                                    paintingPanel.sections[2].elements[8].rangeBar.value,
                                                    paintingPanel.sections[2].elements[9].rangeBar.value,
                                                    paintingPanel.sections[2].elements[6].checkBox.clickState1,
                                                    paintingPanel.sections[2].elements[2].checkBox.clickState1,
                                                    "brush_1",
                                                    painter.brushTexture
                                                )
                                        );
                
                library.brushes[library.brushes.size()-1].updateDisplayTexture(shaders.twoDPainting,shaders.buttonShader);
                
            }
        }
        
        


        //!PAINTING MODES PANEL
        for (size_t i = 0; i < paintingModesPanel.sections[0].elements.size(); i++)
        {
            if(paintingModesPanel.sections[0].elements[i].button.clickState1){ //Painting mode pressed
                if(painter.selectedPaintingModeIndex != i){
                    paintingModesPanel.sections[0].elements[painter.selectedPaintingModeIndex].button.clickState1 = false;
                    painter.selectedPaintingModeIndex = i;
                    break;
                }
            }

            if(painter.selectedPaintingModeIndex == i){
                paintingModesPanel.sections[0].elements[painter.selectedPaintingModeIndex].button.clickState1 = true;
            }
        }
        



        //!Positioning the panels
        paintingPanel.pos.x = windowPanel.pos.x - windowPanel.scale.x - paintingPanel.scale.x; //Keep on the left side of the window panel 
        windowPanel.pos.x = 100.f - windowPanel.scale.x - screenGapPerc; //Keep on the right side
        libraryPanelDisplayer.pos.x = libraryPanelLeft.pos.x + libraryPanelLeft.scale.x + libraryPanelDisplayer.scale.x; //Keep on the left side of the window panel 
        
        paintingPanel.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + paintingPanel.scale.y; //Keep beneath the navigation bar
        windowPanel.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + windowPanel.scale.y; //Keep beneath the navigation bar
        libraryPanelLeft.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + windowPanel.scale.y; //Keep beneath the navigation bar

        libraryPanelDisplayer.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + libraryPanelDisplayer.scale.y; //Keep beneath the navigation bar
        libraryPanelDisplayer.scale.y = 50 - navigationPanel.scale.y - nodeEditorDisplayer.scale.y;
        libraryPanelLeft.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + libraryPanelDisplayer.scale.y; //Keep beneath the navigation bar
        libraryPanelLeft.scale.y = 50 - navigationPanel.scale.y - nodeEditorDisplayer.scale.y;

        nodeEditorDisplayer.pos.x = paintingPanel.pos.x - paintingPanel.scale.x - nodeEditorDisplayer.scale.x; //Keep on the left side of the window panel 
        selectedTextureDisplayer.pos.x = libraryPanelDisplayer.pos.x - libraryPanelLeft.pos.x;
        selectedTextureDisplayer.pos.y = nodeEditorDisplayer.pos.y;
        selectedTextureDisplayer.scale.y = nodeEditorDisplayer.scale.y;
        selectedTextureDisplayer.scale.x = libraryPanelDisplayer.scale.x + libraryPanelLeft.scale.x;
        
        nodeEditorDisplayer.scale.x = 50 - screenGapPerc/2.f - selectedTextureDisplayer.scale.x - (paintingPanel.scale.x+windowPanel.scale.x);
        
        twoDPaintingPanel.scale.x = 50 - screenGapPerc/2.f - selectedTextureDisplayer.scale.x - (paintingPanel.scale.x+windowPanel.scale.x);
        twoDPaintingPanel.scale.y = 50 - navigationPanel.scale.y - nodeEditorDisplayer.scale.y;
        twoDPaintingPanel.pos.x = paintingPanel.pos.x - paintingPanel.scale.x - twoDPaintingPanel.scale.x; //Keep on the left side of the window panel 
        twoDPaintingPanel.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + twoDPaintingPanel.scale.y; //Keep beneath the navigation bar

        paintingModesPanel.pos.x = libraryPanelDisplayer.pos.x + libraryPanelDisplayer.scale.x + paintingModesPanel.scale.x + 1;
        paintingModesPanel.pos.y = navigationPanel.pos.y + navigationPanel.scale.y + paintingModesPanel.scale.y;
    }
};

#endif