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

#include "GUI/UI.hpp"
#include "GUI/Button.hpp"
#include "Mouse.hpp"
#include "Timer.hpp"


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
    void mouseTracking(Mouse& mouse,glm::vec2 resultScale,glm::vec3 resultPos){
        //Check if mouse on the panel or any side of the panel 

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
    }

    void resizeThePanel(Mouse &mouse, glm::vec2 videoScale){
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
    }
    void prepDrawBtnVertically(Button &button,Button &previousButton,float& elementPos,int btnCounter){
        button.scale.x = scale.x;

        //Move the button on top of the panel
        button.pos = pos;
        button.pos.y -= scale.y;
        
        button.pos.y += button.scale.y;
        //button.pos.y += sections[sI].buttons[0].scale.y;
        
        elementPos = button.pos.y; 

        elementPos += btnCounter * (button.scale.y + previousButton.scale.y);

        button.pos.z += 0.01f;
        button.pos.y = elementPos;
    }
    void prepDrawBtnHorizontally(Button &button,Button &previousButton,float& elementPos,int btnCounter){
        button.scale.y = scale.y;

        //Move the button on top of the panel
        button.pos = pos;
        button.pos.x -= scale.x;
        
        button.pos.x += button.scale.x;
        //button.pos.x += sections[sI].buttons[0].scale.x;
        
        elementPos = button.pos.x; 

        elementPos += btnCounter * (button.scale.x + previousButton.scale.x);

        button.pos.z += 0.01f;
        button.pos.x = elementPos;
    }
    void drawPanel(glm::vec2 videoScale,Mouse &mouse, glm::vec3 resultPos,glm::vec2 resultScale,Timer &timer,TextRenderer &textRenderer){
        //Draw the panel and it's elements

        //Draw panel
        shader.setVec3("pos",       resultPos);
        shader.setVec2("scale",     resultScale);
        shader.setVec4("color",     color   );
        shader.setVec4("color2"  ,     color     );
        shader.setFloat("colorMixVal"  ,   0.f );
        
        shader.setFloat("width",     resultScale.x  );
        shader.setFloat("height",     resultScale.y   );
        
        shader.setFloat("radius",     10.f   );
        shader.setInt("outline",     false   );
        shader.setFloat("thickness" ,    0.f  );

        glDrawArrays(GL_TRIANGLES, 0, 6);

        //Panel's buttons

        float elementPos = 0.f;
        //Render sections
        int btnCounter = 0; //Indexing buttons to position them
        for (int sI = 0; sI < sections.size(); sI++)
        {
            if(sections[sI].header.text.size()){ //If there is a header
                //Section header button

                //Prepare the transform data of the button    
                if(vertical)
                    prepDrawBtnVertically(sections[sI].header,sections[max(sI-1,0)].buttons[sections[max(sI-1,0)].buttons.size()-1],elementPos,btnCounter);
                else
                    prepDrawBtnHorizontally(sections[sI].header,sections[max(sI-1,0)].buttons[sections[max(sI-1,0)].buttons.size()-1],elementPos,btnCounter);
                //Draw the button
                sections[sI].header.render(videoScale,mouse,timer,textRenderer);
                btnCounter++; //Indexing buttons to position them
            }

            for (int i = 0; i < sections[sI].buttons.size(); i++) //
            {
                //Buttons of the current section

                //Prepare the transform data of the button    
                if(vertical)
                    prepDrawBtnVertically(sections[sI].buttons[i],sections[sI].buttons[max(i-1,0)],elementPos,btnCounter);
                else
                    prepDrawBtnHorizontally(sections[sI].buttons[i],sections[sI].buttons[max(i-1,0)],elementPos,btnCounter);

                //Draw the button
                sections[sI].buttons[i].render(videoScale,mouse,timer,textRenderer);
                btnCounter++; //Indexing buttons to position them
            }
        }
    }
public:
    Shader shader;

    //Attributes
    //Values are percentage of the monitor size
    glm::vec2 scale;  //Example : w : 20, h : 30 means cover %20 of the window in x axis & cover 30% of the window in y axis
    glm::vec3 pos; //Same here
    
    glm::vec4 color;
    
    std::vector<Section> sections;

    bool hover = false; //Panel's itself
    
    bool vertical = true;

    //Sides of the panel
    PanelSide leftSide;
    PanelSide rightSide;
    PanelSide bottomSide;

    Panel(){}
    Panel(Shader shader,std::vector<Section> sections,glm::vec2 scale,glm::vec3 pos,glm::vec4 color,bool vertical){
        this->shader = shader;
        this->vertical = vertical;
        this->scale = scale;
        this->color = color;
        this->pos = pos;
        this->sections = sections;
    }

    void render(glm::vec2 videoScale,Mouse& mouse,Timer &timer,TextRenderer &textRenderer){
        //Panel's itself
        Util util;

        // pos value % of the video scale
        glm::vec3 resultPos = glm::vec3( 
                              util.getPercent(videoScale,glm::vec2(pos.x,pos.y)) //Don't include the depth
                              ,pos.z); //Use the original depth value

        // scale value % of the video scale
        glm::vec2 resultScale = util.getPercent(videoScale,scale);


        mouseTracking(mouse,resultScale,resultPos);

        resizeThePanel(mouse,videoScale);
    
        drawPanel(videoScale,mouse,resultPos,resultScale,timer,textRenderer);
    }
};
#endif