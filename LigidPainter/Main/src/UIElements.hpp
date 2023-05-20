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

#ifndef LGDUIELEMENTS_HPP
#define LGDUIELEMENTS_HPP

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

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

class Button
{
private:
    /* data */
public:
    //For now buttons can only be usable with panels

    Shader shader;

    //---Button properties
    std::string text;
    glm::vec4 color;
    
    //Those values are adepting to the panel if button is attached to one
    //Values are percentage of the monitor size
    glm::vec2 scale;  //Example : w : 20, h : 30 means cover %20 of the window in x axis & cover 30% of the window in y axis
    glm::vec3 pos; //Same here
    

    Button(){}
    Button(Shader shader,glm::vec4 color){
        this->shader = shader;
        this->color = color;

    }
    void render(glm::vec2 videoScale){
        Util util;

        // pos value % of the video scale
        glm::vec3 resultPos = glm::vec3( 
                              util.getPercent(videoScale,glm::vec2(pos.x,pos.y)) //Don't include the depth
                              ,pos.z); //Use the original depth value

        // scale value % of the video scale
        glm::vec2 resultScale = util.getPercent(videoScale,scale);

        shader.setVec3("pos",       resultPos);
        shader.setVec2("scale",     resultScale);
        shader.setVec4("color",     color   );

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
};

class Panel
{
private:
    /* data */
public:
    Shader shader;

    //Attributes
    //Values are percentage of the monitor size
    glm::vec2 scale;  //Example : w : 20, h : 30 means cover %20 of the window in x axis & cover 30% of the window in y axis
    glm::vec3 pos; //Same here
    
    glm::vec4 color;

    std::vector<Button> buttons;

    Panel(){}
    Panel(Shader shader, std::vector<Button> buttons,glm::vec2 scale,glm::vec3 pos,glm::vec4 color){
        this->color = color;
        this->pos = pos;
        this->scale = scale;
        this->shader = shader;
        this->buttons = buttons;
    }

    void render(glm::vec2 videoScale){
        //Panel's itself
        Util util;

        // pos value % of the video scale
        glm::vec3 resultPos = glm::vec3( 
                              util.getPercent(videoScale,glm::vec2(pos.x,pos.y)) //Don't include the depth
                              ,pos.z); //Use the original depth value

        // scale value % of the video scale
        glm::vec2 resultScale = util.getPercent(videoScale,scale);

        shader.setVec3("pos",       resultPos);
        shader.setVec2("scale",     resultScale);
        shader.setVec4("color",     color   );

        glDrawArrays(GL_TRIANGLES, 0, 6);

        //Panel's buttons
        for (size_t i = 0; i < buttons.size(); i++)
        {
            buttons[i].scale.x = scale.x;
            buttons[i].scale.y = 10;
            
            buttons[i].pos = pos;
            buttons[i].pos.z += 0.01f;
            buttons[i].pos.y +=  i * buttons[i].scale.y;
            buttons[i].render(videoScale);
        }
    }
};

class UI{
public:
    //UI Elements
    //(Mostly panels)
    Panel paintingPanel;  


    Shaders shaders;
    
    //UI Rendering projection
    //Has the screen resolution
    //Example : L = 0, R = 1920, B = 0, T = 1080 
    glm::mat4 projection; 
    
    UI(){}

    void init(Shaders shaders){
        this->shaders = shaders;
        
        //Init the painting panel
        paintingPanel = Panel(
                                shaders.buttonShader,
                                {
                                    Button(shaders.buttonShader,glm::vec4(1)), //Buttons of the panel here
                                    Button(shaders.buttonShader,glm::vec4(1))
                                },
                                glm::vec2(10,100), //Initial scale value
                                glm::vec3(10,100,0.1f),  //Initial position value
                                glm::vec4(0.1f,0.1f,0.1f,1.f) //Color of the panel
                            );
    }    

    void render(glm::vec2 videoScale){
        glDepthFunc(GL_LEQUAL);

        //Use the related shader
        shaders.buttonShader.use();
        
        //Projection is changing in the renderer.render()
        //userInterface.projection = glm::mat4(...)
        shaders.buttonShader.setMat4("projection",projection); 
        
        //--Render all the UI elements there
        
        paintingPanel.render(videoScale);
    }
};

#endif