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

#ifndef LGDUIPANEL_HPP
#define LGDUIPANEL_HPP

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

#include "GUI/Panel.hpp"
#include "GUI/Button.hpp"
#include "Mouse.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

struct PanelSide{ 
    bool hover = false;
    bool pressed = false;
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

    bool hover = false; //Panel's itself
    
    //Sides of the panel
    PanelSide leftSide;
    PanelSide rightSide;
    PanelSide bottomSide;

    Panel(){}
    Panel(Shader shader,std::vector<Button> buttons,glm::vec2 scale,glm::vec3 pos,glm::vec4 color){
        this->color = color;
        this->pos = pos;
        this->scale = scale;
        this->shader = shader;
        this->buttons = buttons;
    }

    void render(glm::vec2 videoScale,Mouse& mouse){
        //Panel's itself
        Util util;

        // pos value % of the video scale
        glm::vec3 resultPos = glm::vec3( 
                              util.getPercent(videoScale,glm::vec2(pos.x,pos.y)) //Don't include the depth
                              ,pos.z); //Use the original depth value

        // scale value % of the video scale
        glm::vec2 resultScale = util.getPercent(videoScale,scale);




        const float grabbingRange = 20; 

        //Check if mouse on top of the panel
        hover = mouse.isMouseHover(resultScale,glm::vec2(resultPos.x,resultPos.y));
        //Check if mouse on top of the left side of the panel
        leftSide.hover = mouse.isMouseHover(glm::vec2(grabbingRange,resultScale.y),glm::vec2(resultPos.x-resultScale.x,resultPos.y));
        //Check if mouse on top of the right side of the panel
        rightSide.hover = mouse.isMouseHover(glm::vec2(grabbingRange,resultScale.y),glm::vec2(resultPos.x+resultScale.x,resultPos.y));
        //Check if mouse on top of the bottom side of the panel
        bottomSide.hover = mouse.isMouseHover(glm::vec2(resultScale.x,grabbingRange),glm::vec2(resultPos.x,resultPos.y + resultScale.y));

        //Keep the left corner in the pressed state from the left click to left mouse button release
        if(leftSide.hover && mouse.LClick) //Clicked to the corner
            leftSide.pressed = true;
        if(!mouse.LPressed) //Corner released
            leftSide.pressed = false;
        
        //Keep the right corner in the pressed state from the left click to left mouse button release
        if(rightSide.hover && mouse.LClick) //Clicked to the corner
            rightSide.pressed = true;
        if(!mouse.LPressed) //Corner released
            rightSide.pressed = false;
        
        //Keep the bottom corner in the pressed state from the left click to left mouse button release
        if(bottomSide.hover && mouse.LClick) //Clicked to the corner
            bottomSide.pressed = true;
        if(!mouse.LPressed) //Corner released
            bottomSide.pressed = false;


        //Resize the panel if necessary
        if(rightSide.pressed){
            scale.x += mouse.mouseOffset.x/videoScale.x * 50.f;
            pos.x += mouse.mouseOffset.x/videoScale.x *50.f;
        }
        else if(leftSide.pressed){
            scale.x -= mouse.mouseOffset.x/videoScale.x * 50.f;
            pos.x += mouse.mouseOffset.x/videoScale.x *50.f;
        }
        else if(bottomSide.pressed){
            scale.y += mouse.mouseOffset.y/videoScale.y * 50.f;
            pos.y += mouse.mouseOffset.y/videoScale.y *50.f;
        }

        //Restrict the size of the panel
        if(scale.x < 1)
            scale.x = 1;


        shader.setVec3("pos",       resultPos);
        shader.setVec2("scale",     resultScale);
        shader.setVec4("color",     color   );
        
        shader.setFloat("width",     scale.x   );
        shader.setFloat("height",     scale.y   );
        
        shader.setFloat("radius",     500.f   );
        shader.setInt("outline",     false   );
        shader.setFloat("thickness" ,    0.f  );



        glDrawArrays(GL_TRIANGLES, 0, 6);

        //Panel's buttons
        for (size_t i = 0; i < buttons.size(); i++)
        {
            buttons[i].scale.x = scale.x;
            buttons[i].scale.y = 2;
            
            //Move the button on top of the panel
            buttons[i].pos = pos;
            buttons[i].pos.y -= scale.y;
            buttons[i].pos.y += buttons[i].scale.y;
            
            buttons[i].pos.z += 0.01f;
            buttons[i].pos.y +=  i * buttons[i].scale.y * 2.f;
            
            buttons[i].render(videoScale,mouse);
        }
    }
};
#endif