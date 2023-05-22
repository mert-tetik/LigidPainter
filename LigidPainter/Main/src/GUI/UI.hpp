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

struct Element{
    //The UI Element

    Button button;
    RangeBar rangeBar;
    
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

    void render(glm::vec2 videoScale,Mouse& mouse, Timer &timer,TextRenderer &textRenderer){
        if(state == 0){ //Render the button
            button.pos = pos;
            button.scale = scale;
            button.panelOffset = panelOffset;
            button.render(videoScale,mouse, timer,textRenderer);
        }
        if(state == 1){ //Render the button
            rangeBar.pos = pos;
            rangeBar.scale = scale;
            rangeBar.panelOffset = panelOffset;
            rangeBar.render(videoScale,mouse, timer,textRenderer);
        }
    }
};

struct Section{ //Sections seperates the elements in the panel
    
    // Header element (After pressing that button you can display other elements in that section)
    //! Is always button
    Element header; 

    std::vector<Element> elements; //Elements of that section
    
    Section(Element header,std::vector<Element> elements){
        this->header = header;
        this->elements = elements;
    }
};


#include "GUI/Panel.hpp"
#include "Mouse.hpp"


#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>



class UI{
public:
    //UI Elements
    //(Mostly panels)
    Panel navigationPanel;
    Panel windowPanel;
    Panel paintingPanel; //Main panel (kinda)

    Shaders shaders; 

	AppTextures appTextures;
    
    //UI Rendering projection
    //Has the screen resolution
    //Example : L = 0, R = 1920, B = 1080, T = 0  
    glm::mat4 projection; 

    ColorPalette colorPalette;
    
    UI(){}

    void init(Shaders shaders,Context context,AppTextures appTextures){
        this->shaders = shaders;
        this->appTextures = appTextures;
        
        //Init the painting panel
        navigationPanel = Panel(
                                shaders.buttonShader,
                                
                                {
                                    Section(
                                        Element(Button()),
                                        {   
                                            Element(Button(1,glm::vec2(2,5),colorPalette,shaders.buttonShader,"File"    ,Texture(), 0.f)),
                                            Element(Button(1,glm::vec2(2,5),colorPalette,shaders.buttonShader,"Edit"    ,Texture(), 0.f)),
                                            Element(Button(1,glm::vec2(2,5),colorPalette,shaders.buttonShader,"Panels"  ,Texture(), 0.f)),
                                            Element(Button(1,glm::vec2(2,5),colorPalette,shaders.buttonShader,"Help"    ,Texture(), 0.f)),
                                        }
                                    )
                                },

                                glm::vec2(50,1), //Initial scale value
                                glm::vec3(50,1,0.1f),  //Initial position value
                                colorPalette.mainColor, //Color of the panel
                                false
                            );

        windowPanel  = Panel(
                                shaders.buttonShader,

                                {
                                    Section(
                                        Element(Button()),
                                        {   
                                            Element(Button(1,glm::vec2(2,5),colorPalette,shaders.buttonShader,"3D Painting"         , appTextures.TDModelIcon, 0.f)),
                                            Element(Button(1,glm::vec2(2,5),colorPalette,shaders.buttonShader,"2D Painting"         , appTextures.TDModelIcon, 0.f)),
                                            Element(Button(1,glm::vec2(2,5),colorPalette,shaders.buttonShader,"Matarial editor"     , appTextures.TDModelIcon, 0.f)),
                                            Element(Button(1,glm::vec2(2,5),colorPalette,shaders.buttonShader,"Displayer"           , appTextures.TDModelIcon, 0.f)),
                                        }
                                    )
                                },
                                
                                glm::vec2(1,48), //Initial scale value
                                glm::vec3(99,50,0.1f),  //Initial position value
                                colorPalette.mainColor, //Color of the panel
                                true
                            );

        paintingPanel=Panel(
                                shaders.buttonShader,
                                
                                {
                                    Section(
                                        Element(Button(2,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Brush"  , appTextures.TDModelIcon, 0.f)),
                                        {   
                                            Element(Button(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Stuff"  , appTextures.TDModelIcon, 5.f)),
                                        }
                                    ),
                                    Section(
                                        Element(Button(2,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Blur"  , appTextures.TDModelIcon, 0.f)),
                                        {   
                                            Element(Button(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Stuff"  , appTextures.TDModelIcon, 5.f)),
                                            Element(RangeBar(0,glm::vec2(2,2),colorPalette,shaders.buttonShader, "Stuff"  , appTextures.TDModelIcon, 5.f,0.f,100.f,50.f)),
                                        }
                                    )
                                },
                                
                                glm::vec2(5,48), //Initial scale value
                                glm::vec3(50,50,0.1f),  //Initial position value
                                colorPalette.mainColor, //Color of the panel
                                true
                            );
    }    

    void render(glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer){
        glDepthFunc(GL_LEQUAL);

        //Use the related shader
        shaders.buttonShader.use();
        
        //Send the monitor resolution to the button shader
        shaders.buttonShader.setVec2("videoScale",videoScale);  
        //Projection is changing in the renderer.render()
        //userInterface.projection = glm::mat4(...)
        shaders.buttonShader.setMat4("projection",projection); 
        
        //--Render all the UI elements there
        
        navigationPanel.render(videoScale,mouse,timer,textRenderer);
        windowPanel.render(videoScale,mouse,timer,textRenderer);
        paintingPanel.render(videoScale,mouse,timer,textRenderer);
    }
};

#endif