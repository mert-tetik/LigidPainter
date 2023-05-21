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

struct Section{ //Sections seperates the button elements in the panel
    
    Button header;// Header button (After pressing that button you can display other elements in that section)
    std::vector<Button> buttons; //Button elements of that section
    
    Section(Button header,std::vector<Button> buttons){
        this->header = header;
        this->buttons = buttons;
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
                                        Button(),
                                        {   
                                            //         Shader               Text           Scale %               Color                  Second Color        outline     radius %    animation style             text color              second text color      texture    text scale
                                            Button(shaders.buttonShader,  "File"        ,glm::vec2(2,5), colorPalette.secondColor, colorPalette.themeColor,  false        ,0         ,1            ,colorPalette.oppositeColor, colorPalette.mainColor    ,   Texture(),    0.5f    ),
                                            Button(shaders.buttonShader,  "Edit"        ,glm::vec2(2,5), colorPalette.secondColor, colorPalette.themeColor,  false        ,0         ,1            ,colorPalette.oppositeColor, colorPalette.mainColor    ,   Texture(),    0.5f    ),
                                            Button(shaders.buttonShader,  "Panels"      ,glm::vec2(2,5), colorPalette.secondColor, colorPalette.themeColor,  false        ,0         ,1            ,colorPalette.oppositeColor, colorPalette.mainColor    ,   Texture(),    0.5f    ),
                                            Button(shaders.buttonShader,  "Help"        ,glm::vec2(2,5), colorPalette.secondColor, colorPalette.themeColor,  false        ,0         ,1            ,colorPalette.oppositeColor, colorPalette.mainColor    ,   Texture(),    0.5f    ) //Buttons of the panel here
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
                                        Button(),
                                        {   
                                            //         Shader               Text                   Scale %               Color                  Second Color        outline     radius %    animation style             text color              second text color      texture                  text scale
                                            Button(shaders.buttonShader,  "3D Painting"        ,glm::vec2(2,2), colorPalette.secondColor, colorPalette.themeColor,  false        ,0         ,1            ,colorPalette.oppositeColor, colorPalette.mainColor    ,   appTextures.TDModelIcon,    0.5f    ),
                                            Button(shaders.buttonShader,  "2D Painting"        ,glm::vec2(2,2), colorPalette.secondColor, colorPalette.themeColor,  false        ,0         ,1            ,colorPalette.oppositeColor, colorPalette.mainColor    ,   appTextures.TDModelIcon,    0.5f    ),
                                            Button(shaders.buttonShader,  "Matarial editor"    ,glm::vec2(2,2), colorPalette.secondColor, colorPalette.themeColor,  false        ,0         ,1            ,colorPalette.oppositeColor, colorPalette.mainColor    ,   appTextures.TDModelIcon,    0.5f    ),
                                            Button(shaders.buttonShader,  "Displayer"          ,glm::vec2(2,2), colorPalette.secondColor, colorPalette.themeColor,  false        ,0         ,1            ,colorPalette.oppositeColor, colorPalette.mainColor    ,   appTextures.TDModelIcon,    0.5f    ) //Buttons of the panel here
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
                                        Button(shaders.buttonShader,  "Brush"        ,glm::vec2(2,2), colorPalette.themeColor, colorPalette.secondColor,  false        ,0         ,1            ,colorPalette.oppositeColor, colorPalette.mainColor    ,   appTextures.TDModelIcon,    0.5f    ),
                                        {   
                                            //         Shader               Text           Scale %               Color                  Second Color        outline     radius %    animation style             text color              second text color      texture                  text scale
                                            Button(shaders.buttonShader,  "Radius"      ,glm::vec2(2,2), colorPalette.secondColor, colorPalette.themeColor,  false        ,0         ,1            ,colorPalette.oppositeColor, colorPalette.mainColor    ,   appTextures.TDModelIcon,    0.5f    ),
                                            Button(shaders.buttonShader,  "Strength"    ,glm::vec2(2,2), colorPalette.secondColor, colorPalette.themeColor,  false        ,0         ,1            ,colorPalette.oppositeColor, colorPalette.mainColor    ,   appTextures.TDModelIcon,    0.5f    ),
                                            Button(shaders.buttonShader,  "Mask"        ,glm::vec2(2,2), colorPalette.secondColor, colorPalette.themeColor,  false        ,0         ,1            ,colorPalette.oppositeColor, colorPalette.mainColor    ,   appTextures.TDModelIcon,    0.5f    ),
                                            Button(shaders.buttonShader,  "Color"       ,glm::vec2(2,2), colorPalette.secondColor, colorPalette.themeColor,  false        ,0         ,1            ,colorPalette.oppositeColor, colorPalette.mainColor    ,   appTextures.TDModelIcon,    0.5f    ), //Buttons of the panel here
                                            Button(shaders.buttonShader,  "Softness"    ,glm::vec2(2,2), colorPalette.secondColor, colorPalette.themeColor,  false        ,0         ,1            ,colorPalette.oppositeColor, colorPalette.mainColor    ,   appTextures.TDModelIcon,    0.5f    ) //Buttons of the panel here
                                        }
                                    ),
                                    Section(
                                        Button(shaders.buttonShader,  "Blur"        ,glm::vec2(2,2), colorPalette.themeColor, colorPalette.secondColor,  false        ,0         ,1            ,colorPalette.oppositeColor, colorPalette.mainColor    ,   appTextures.TDModelIcon,    0.5f    ),
                                        {   
                                            //         Shader               Text           Scale %               Color                  Second Color        outline     radius %    animation style             text color              second text color      texture                  text scale
                                            Button(shaders.buttonShader,  "Radius"     ,glm::vec2(2,2), colorPalette.secondColor, colorPalette.themeColor,  false        ,0         ,1            ,colorPalette.oppositeColor, colorPalette.mainColor    ,   appTextures.TDModelIcon,    0.5f    ),
                                            Button(shaders.buttonShader,  "Strength"   ,glm::vec2(2,2), colorPalette.secondColor, colorPalette.themeColor,  false        ,0         ,1            ,colorPalette.oppositeColor, colorPalette.mainColor    ,   appTextures.TDModelIcon,    0.5f    ),
                                            Button(shaders.buttonShader,  "Mask"       ,glm::vec2(2,2), colorPalette.secondColor, colorPalette.themeColor,  false        ,0         ,1            ,colorPalette.oppositeColor, colorPalette.mainColor    ,   appTextures.TDModelIcon,    0.5f    ),
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