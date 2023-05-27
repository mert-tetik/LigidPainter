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
#include "Box.hpp"
#include "Renderer.h"
#include "Util.hpp"
#include "Timer.hpp"
#include "ColorPalette.hpp"
#include "TextRendering/TextRenderer.hpp"

#include "GUI/Button.hpp"
#include "GUI/RangeBar.hpp"
#include "GUI/CheckBox.hpp"

struct Element{
    //The UI Element

    int nodeState; //Used by the elements of nodes (0 : Input , 1 : Input no connection , 2 : Output , 3 : Output no connection)

    Button button;
    RangeBar rangeBar;
    CheckBox checkBox;
    
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
    Element(CheckBox checkBox){
        //Init as range bar
        this->checkBox = checkBox;
        pos = checkBox.pos;
        scale = checkBox.scale;
        panelOffset = checkBox.panelOffset;
        state = 2;
    }

    void render(glm::vec2 videoScale,Mouse& mouse, Timer &timer,TextRenderer &textRenderer){
        if(state == 0){ //Render the button
            button.pos = pos;
            button.scale = scale;
            button.panelOffset = panelOffset;
            button.render(videoScale,mouse, timer,textRenderer);
        }
        if(state == 1){ //Render the rangeBar
            rangeBar.pos = pos;
            rangeBar.scale = scale;
            rangeBar.panelOffset = panelOffset;
            rangeBar.render(videoScale,mouse, timer,textRenderer);
        }
        if(state == 2){ //Render the checkBox
            checkBox.pos = pos;
            checkBox.scale = scale;
            checkBox.panelOffset = panelOffset;
            checkBox.render(videoScale,mouse, timer,textRenderer);
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


#include "GUI/Panel.hpp"
#include "GUI/Dialogs/GreetingDialog.hpp"
#include "GUI/Dialogs/MaterialEditorDialog.hpp"
#include "GUI/Dialogs/TextureSelectionDialog.hpp"
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


    //Dialogs    
    //GreetingDialog greetingDialog;
    MaterialEditorDialog materialEditorDialog;
    TextureSelectionDialog textureSelectionDialog;

    int frameCounter = 0; //Reset every 1000 frame

    int selectedLibraryElementIndex = 0; //0 For the textures , 1 for the materials bla bla
    int selectedMaterialIndex = 0; //In the library (to manipulate with )
    

    //UI Rendering projection
    //Has the screen resolution
    //Example : L = 0, R = 1920, B = 1080, T = 0  
    glm::mat4 projection; 

    ColorPalette colorPalette;
    
    UI(){}

    void init(Shaders shaders,Context context,AppTextures appTextures,glm::vec2 videoScale){
        this->shaders = shaders;
        this->appTextures = appTextures;
        
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
                                            Element(Button(1,glm::vec2(2,5),colorPalette,shaders.buttonShader,"3D Painting"         , appTextures.TDModelIcon, 0.f,false)),
                                            Element(Button(1,glm::vec2(2,5),colorPalette,shaders.buttonShader,"2D Painting"         , appTextures.TDModelIcon, 0.f,false)),
                                            Element(Button(1,glm::vec2(2,5),colorPalette,shaders.buttonShader,"Matarial editor"     , appTextures.TDModelIcon, 0.f,false)),
                                            Element(Button(1,glm::vec2(2,5),colorPalette,shaders.buttonShader,"Displayer"           , appTextures.TDModelIcon, 0.f,false)),
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
                                        Element(Button(2,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Brush"  , appTextures.TDModelIcon, 0.f,true)),
                                        {   
                                            Element(Button(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Color"  , appTextures.TDModelIcon, 1.f,false)),
                                            Element(RangeBar(0,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Radius"  , appTextures.TDModelIcon, 1.f,0.f,100.f,50.f)),
                                            Element(RangeBar(0,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Strength"  , appTextures.TDModelIcon, 1.f,0.f,100.f,50.f)),
                                            Element(RangeBar(0,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Smoothness"  , appTextures.TDModelIcon, 1.f,0.f,100.f,50.f)),
                                            Element(RangeBar(0,glm::vec2(2,1),colorPalette,shaders.buttonShader, "Spacing"  , appTextures.TDModelIcon, 1.f,0.f,100.f,50.f)),
                                        }
                                    ),
                                    Section(
                                        Element(Button(2,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Soften"  , appTextures.TDModelIcon, 3.f,true)),
                                        {   
                                            Element(Button(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Radius"  , appTextures.TDModelIcon, 1.f,false)),
                                            Element(RangeBar(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Strength"  , appTextures.TDModelIcon, 1.f,0.f,100.f,50.f)),
                                        }
                                    ),
                                    Section(
                                        Element(Button(2,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Smear"  , appTextures.TDModelIcon, 3.f,true)),
                                        {   
                                            Element(Button(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Radius"  , appTextures.TDModelIcon, 1.f,false)),
                                            Element(RangeBar(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Strength"  , appTextures.TDModelIcon, 1.f,0.f,100.f,50.f)),
                                        }
                                    ),
                                    Section(
                                        Element(Button(2,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Vector"  , appTextures.TDModelIcon, 3.f,true)),
                                        {   
                                            Element(Button(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Fill"  , appTextures.TDModelIcon, 1.f,false)),
                                            Element(Button(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Stroke"  , appTextures.TDModelIcon, 1.f,false)),
                                        }
                                    ),
                                    Section(
                                        Element(Button(2,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Area"  , appTextures.TDModelIcon, 3.f,true)),
                                        {   
                                            Element(Button(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Box"  , appTextures.TDModelIcon, 1.f,false)),
                                            Element(CheckBox(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Gradient"  , 1.f)),
                                            Element(CheckBox(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Depth Test"  , 1.f)),
                                            Element(RangeBar(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Lasso"  , appTextures.TDModelIcon, 1.f,0.f,100.f,50.f)),
                                            Element(RangeBar(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Polygonal Lasso"  , appTextures.TDModelIcon, 1.f,0.f,100.f,50.f)),
                                            Element(RangeBar(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Circle"  , appTextures.TDModelIcon, 1.f,0.f,100.f,50.f)),
                                        }
                                    ),
                                    Section(
                                        Element(Button(2,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Text"  , appTextures.TDModelIcon, 3.f,true)),
                                        {   
                                            Element(Button(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Text"  , appTextures.TDModelIcon, 1.f,false)),
                                            Element(Button(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Font"  , appTextures.TDModelIcon, 1.f,false)),
                                            Element(Button(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Color"  , appTextures.TDModelIcon, 1.f,false)),
                                            Element(Button(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Alignment"  , appTextures.TDModelIcon, 1.f,false)),
                                            Element(RangeBar(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Size"  , appTextures.TDModelIcon, 1.f,0.f,100.f,50.f)),
                                            Element(RangeBar(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Bearing"  , appTextures.TDModelIcon, 1.f,0.f,100.f,50.f)),
                                        }
                                    )
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
                                3,
                                {
                                    Button(1,glm::vec2(2,1.5f),colorPalette,shaders.buttonShader,"Add"        , Texture(), 0.f,false),
                                    Button(1,glm::vec2(2,1.5f),colorPalette,shaders.buttonShader,"Del"        , Texture(), 0.f,false)
                                },
                                20.f
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
        

        //greetingDialog = GreetingDialog(context,videoScale,colorPalette,shaders.buttonShader,appTextures);
        materialEditorDialog = MaterialEditorDialog(shaders.buttonShader,colorPalette,appTextures);
        textureSelectionDialog = TextureSelectionDialog(shaders.buttonShader,colorPalette);
    }    

    void render(glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer,Context context,Box box,Library &library,std::vector<Node> &appNodes,std::vector<Node> &nodeScene,
                std::vector<ContextMenu> &contextMenus,int &textureRes){
        glDepthFunc(GL_LEQUAL);
        
        shaders.singleCurve.use();
        shaders.singleCurve.setMat4("projection",projection); 

        //Use the related shader
        shaders.buttonShader.use();
        
        //Send the monitor resolution to the button shader
        shaders.buttonShader.setVec2("videoScale",videoScale);  
        //Projection is changing in the renderer.render()
        //userInterface.projection = glm::mat4(...)
        shaders.buttonShader.setMat4("projection",projection); 
        
        //Update the library displayer panel every 100 frame
        if(frameCounter % 100 == 0){
            Section libSection;
            libSection.header = Element(Button()); //Has no section button
            libraryPanelDisplayer.sections.clear();
            if(selectedLibraryElementIndex == 0){//Update textures
                for (size_t i = 0; i < library.textures.size(); i++)
                {
                    libSection.elements.push_back(Element(Button(1,glm::vec2(2,4.f),colorPalette,shaders.buttonShader,"texture_0"       , library.textures[i], 0.f,false))) ;
                }
            }
            else if(selectedLibraryElementIndex == 1){ //Update materials
                for (size_t i = 0; i < library.materials.size(); i++)
                {
                    libSection.elements.push_back(Element(Button(1,glm::vec2(2,4.f),colorPalette,shaders.buttonShader,"material"       , Texture(), 0.f,false))) ;
                }
            }
            libraryPanelDisplayer.sections.push_back(Section(Element(Button()),libSection.elements));
        }
        
        float screenGap = videoScale.x - context.windowScale.x; //Use that value to keep panels on the left side
        float screenGapPerc = screenGap / videoScale.x * 100.f; 

        //--Render all the UI elements there
        
        navigationPanel.render(videoScale,mouse,timer,textRenderer);
        windowPanel.render(videoScale,mouse,timer,textRenderer);
        paintingPanel.render(videoScale,mouse,timer,textRenderer);
        libraryPanelLeft.render(videoScale,mouse,timer,textRenderer);
        libraryPanelDisplayer.render(videoScale,mouse,timer,textRenderer);
        nodeEditorDisplayer.render(videoScale,mouse,timer,textRenderer);
        selectedTextureDisplayer.render(videoScale,mouse,timer,textRenderer);

        for (size_t i = 0; i < contextMenus.size(); i++)//Check contextMenus
        {
            if(i == 1 && selectedLibraryElementIndex == 1){ //Material context menu
                if(contextMenus[i].contextPanel.sections[0].elements[0].button.hover && mouse.LClick && contextMenus[i].active){//Clicked to edit button
                    selectedMaterialIndex = contextMenus[i].selectedElement;
                    materialEditorDialog.activate();
                }
            }
            if(mouse.LClick||mouse.RClick||mouse.MClick||glfwGetKey(context.window,GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetKey(context.window,GLFW_KEY_ENTER) == GLFW_PRESS){
               contextMenus[i].active = false; 
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

            }
        }
        
        

        if(libraryPanelDisplayer.barButtons[0].clickedMixVal == 1.f && selectedLibraryElementIndex == 1){//Add button clicked
            library.materials.push_back(Material(textureRes));
        } 

        //Update the selected library element index
        for (size_t i = 0; i < libraryPanelLeft.sections[0].elements.size(); i++)
        {
            if(libraryPanelLeft.sections[0].elements[i].button.clickState1){ //If a button is clicked (textures or materials for example)
                if(selectedLibraryElementIndex != i){ //If the clicked button is not selected 
                    libraryPanelLeft.sections[0].elements[selectedLibraryElementIndex].button.clickState1 = false; //Unselect the selected one
                    selectedLibraryElementIndex = i; //Select the clicked button
                    break; 
                }
            }
        }
        

        //Positioning the panels
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
        
        nodeEditorDisplayer.scale.x = 50 - selectedTextureDisplayer.scale.x - (paintingPanel.scale.x+windowPanel.scale.x);
        Util util;
        
        std::cout<<50.f/100.f*60.f <<std::endl;

        selectedTextureDisplayer.sections[0].elements[0].scale.y = selectedTextureDisplayer.scale.y;

        
        //Render the nodes
        for (size_t i = 0; i < nodeScene.size(); i++)
        {
            nodeScene[i].render(videoScale,mouse,timer,textRenderer,nodeEditorDisplayer,nodeScene);
        }

        //greetingDialog.render(context.window,colorPalette,mouse,timer,textRenderer,videoScale);
        if(materialEditorDialog.active && library.materials.size()){
            if(glfwGetKey(context.window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
                materialEditorDialog.active = false;
            materialEditorDialog.render(videoScale,mouse,timer,textRenderer,textureSelectionDialog,library,library.materials[selectedMaterialIndex]);
        }
        
        if(textureSelectionDialog.active)
            textureSelectionDialog.render(videoScale,mouse,timer,textRenderer,library);

        if(frameCounter > 1000)
            frameCounter = 0;
    }
};

#endif