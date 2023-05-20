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
    //Those values are adepting to the panel if button is attached to one
    glm::vec2 scale = glm::vec2(200,100);  
    glm::vec3 pos = glm::vec3(100,100,1); 

    Button(){}
    Button(Shader shader){
        this->shader = shader;
    }
    void render(){
        shader.setVec3("pos",pos);
        shader.setVec2("scale",scale);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
};

class Panel
{
private:
    /* data */
public:
    Shader shader;
    std::string text;
    glm::vec2 scale = glm::vec2(200,100);
    glm::vec3 pos = glm::vec3(100,100,1);
    std::vector<Button> buttons;

    Panel(){}
    Panel(Shader shader, std::vector<Button> buttons){
        this->shader = shader;
        this->buttons = buttons;
    }

    void render(){
        //Panel's itself
        shader.setVec3("pos",pos);
        shader.setVec2("scale",scale);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        //Panel's buttons
        for (size_t i = 0; i < buttons.size(); i++)
        {
            buttons[i].render();
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
        paintingPanel = Panel(shaders.buttonShader,{
            Button(shaders.buttonShader), //Buttons of the panel here
            Button(shaders.buttonShader)
        });
    }    

    void render(){
        //Use the related shader
        shaders.buttonShader.use();
        
        //Projection is changing in the renderer.render()
        //userInterface.projection = glm::mat4(...)
        shaders.buttonShader.setMat4("projection",projection); 
        
        //--Render all the UI elements there
        
        paintingPanel.render();
    }
};

#endif