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
    bool locked = false;
};

class Panel
{
private:
    float slideRatio = 1.f;

    void mouseTracking(Mouse& mouse,glm::vec2 resultScale,glm::vec3 resultPos){
        //Check if mouse on the panel or any side of the panel 

        const float grabbingRange = 20; 

        //Check if mouse on top of the panel
        hover = mouse.isMouseHover(resultScale,glm::vec2(resultPos.x,resultPos.y));
        //Check if mouse on top of the left side of the panel
        leftSide.hover = mouse.isMouseHover(glm::vec2(grabbingRange,resultScale.y),glm::vec2(resultPos.x-resultScale.x,resultPos.y)) && !leftSide.locked;
        //Check if mouse on top of the right side of the panel
        rightSide.hover = mouse.isMouseHover(glm::vec2(grabbingRange,resultScale.y),glm::vec2(resultPos.x+resultScale.x,resultPos.y)) && !rightSide.locked;
        //Check if mouse on top of the bottom side of the panel
        bottomSide.hover = mouse.isMouseHover(glm::vec2(resultScale.x,grabbingRange),glm::vec2(resultPos.x,resultPos.y + resultScale.y)) && !bottomSide.locked;
        //Check if mouse on top of the top side of the panel
        topSide.hover = mouse.isMouseHover(glm::vec2(resultScale.x,grabbingRange),glm::vec2(resultPos.x,resultPos.y - resultScale.y)) && !topSide.locked;

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
        
        //Keep the top corner in the pressed state from the left click to left mouse button release
        if(topSide.hover && mouse.LClick) //Clicked to the corner
            topSide.pressed = true;
        if(!mouse.LPressed) //Corner released
            topSide.pressed = false;

        //Change the cursor
        if(leftSide.hover || leftSide.pressed || rightSide.hover || rightSide.pressed)
            mouse.setCursor(mouse.hSlideCursor);
        if(bottomSide.hover || bottomSide.pressed || topSide.hover || topSide.pressed)
            mouse.setCursor(mouse.vSlideCursor);
    }

    void resizeThePanel(Mouse &mouse, glm::vec2 videoScale){
        //Resize the panel if necessary
        if(rightSide.pressed){
            scale.x += mouse.mouseOffset.x/videoScale.x * 50.f;
            pos.x += mouse.mouseOffset.x/videoScale.x *50.f;
            //Restrict the size of the panel
            if(scale.x < 1 || scale.x > 20){
                scale.x -= mouse.mouseOffset.x/videoScale.x * 50.f;
                pos.x -= mouse.mouseOffset.x/videoScale.x *50.f;
            }
        }
        else if(leftSide.pressed){
            scale.x -= mouse.mouseOffset.x/videoScale.x * 50.f;
            pos.x += mouse.mouseOffset.x/videoScale.x *50.f;
            //Restrict the size of the panel
            if(scale.x < 1 || scale.x > 20){
                scale.x += mouse.mouseOffset.x/videoScale.x * 50.f;
                pos.x -= mouse.mouseOffset.x/videoScale.x *50.f;
            }
        }
        else if(bottomSide.pressed){
            scale.y += mouse.mouseOffset.y/videoScale.y * 50.f;
            pos.y += mouse.mouseOffset.y/videoScale.y *50.f;
            if(scale.y < 0.5f || scale.y > 20){
                scale.y -= mouse.mouseOffset.y/videoScale.y * 50.f;
                pos.y -= mouse.mouseOffset.y/videoScale.y *50.f;
            }
        }
        
        else if(topSide.pressed){
            scale.y -= mouse.mouseOffset.y/videoScale.y * 50.f;
            pos.y += mouse.mouseOffset.y/videoScale.y *50.f;
            if(scale.y < 0.5f || scale.y > 20){
                scale.y += mouse.mouseOffset.y/videoScale.y * 50.f;
                pos.y -= mouse.mouseOffset.y/videoScale.y *50.f;
            }
        }

    }
    void prepDrawBtnVertically(Element &button,Element &previousButton,float& elementPos,int btnCounter){
        button.scale.x = scale.x/rowCount;
        
        

        //Move the button on top of the panel
        button.pos = pos;
        
        button.pos.x -= scale.x;
        button.pos.x += button.scale.x;
        button.pos.x += button.scale.x*2.f * (btnCounter % rowCount);
        button.pos.y += button.scale.y;
        //button.pos.y += sections[sI].elements[0].scale.y;
        
        if(btnCounter && btnCounter % rowCount == 0)        
            elementPos += (button.scale.y + previousButton.scale.y) + button.panelOffset;

        button.pos.z += 0.01f;
        button.pos.y = elementPos - slideVal * slideRatio;
    }
    void prepDrawBtnHorizontally(Element &button,Element &previousButton,float& elementPos,int btnCounter){
        button.scale.y = scale.y;

        //Move the button on top of the panel
        button.pos = pos;
        //button.pos.x += sections[sI].elements[0].scale.x;

        if(btnCounter)        
            elementPos += (button.scale.x + previousButton.scale.x) + button.panelOffset;

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

        shader.setInt("outlineExtra" ,    true     ); 
        shader.setVec3("outlineColor" ,    color2     ); 
        shader.setVec3("outlineColor2" ,    color2     ); 

        shader.setFloat("thickness" ,    outlineThickness  );

        glDrawArrays(GL_TRIANGLES, 0, 6);


        //Barriers (In order to prevent the overflow)
        //Bottom
        shader.setVec3("pos",       glm::vec3(resultPos.x,resultPos.y + resultScale.y + 5000,1.f)); //To the bottom
        shader.setVec2("scale",     glm::vec2(5000));
        
        shader.setFloat("radius",     0.f   ); 
        shader.setInt("outlineExtra" ,    false     ); 
        
        shader.setVec4("color",     glm::vec4(0)   ); //Invisible
        shader.setVec3("outlineColor" ,    glm::vec4(0)     ); //Invisible
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //Top
        shader.setVec3("pos",       glm::vec3(resultPos.x,resultPos.y - resultScale.y - 5000,1.f)); //To the bottom
        shader.setVec2("scale",     glm::vec2(5000));
        glDrawArrays(GL_TRIANGLES, 0, 6);





        //Panel's buttons

        float elementPos = 0.f; //Starting pos
        
        //Get the starting position
        if(vertical){//If the panel is vertical
            if(sections.size()){//If there are sections
                if(sections[0].header.button.text.size()) //If there is a header in the section
                    elementPos = pos.y-scale.y + sections[0].header.scale.y; //Take the starting position by including the height of the section header
                else if(sections[0].elements.size()) //If there are buttons in the first section
                    elementPos = pos.y-scale.y + sections[0].elements[0].scale.y; //Take the starting position by including the height of the first button of the section
            }
        }
        else{//If the panel is horizontal
            if(sections.size()){//If there are sections
                if(sections[0].header.button.text.size())  //If there is a header in the section
                    elementPos = pos.x-scale.x + sections[0].header.scale.x; //Take the starting position by including the height of the section header
                else if(sections[0].elements.size())//If there are buttons in the first section
                    elementPos = pos.x-scale.x + sections[0].elements[0].scale.x; //Take the starting position by including the width of the first button of the section
            }
        }
        
        //Render sections
        int btnCounter = 0; //Indexing buttons to position them
        for (int sI = 0; sI < sections.size(); sI++)
        {
            if(sections[sI].header.button.text.size()){ //If there is a header
                //Section header button

                //Prepare the transform data of the button    
                if(vertical)
                    prepDrawBtnVertically(sections[sI].header,sections[max(sI-1,0)].elements[sections[max(sI-1,0)].elements.size()-1],elementPos,btnCounter);
                else
                    prepDrawBtnHorizontally(sections[sI].header,sections[max(sI-1,0)].elements[sections[max(sI-1,0)].elements.size()-1],elementPos,btnCounter);
                //Draw the button
                sections[sI].header.render(videoScale,mouse,timer,textRenderer);
                btnCounter++; //Indexing buttons to position them
            }
            if(sections[sI].header.button.clickState1 == true || !sections[sI].header.button.text.size()){
                for (int i = 0; i < sections[sI].elements.size(); i++) //
                {
                    //Buttons of the current section

                    //Prepare the transform data of the button    
                    if(vertical)
                        prepDrawBtnVertically(sections[sI].elements[i],sections[sI].elements[max(i-1,0)],elementPos,btnCounter);
                    else
                        prepDrawBtnHorizontally(sections[sI].elements[i],sections[sI].elements[max(i-1,0)],elementPos,btnCounter);

                    //Draw the button
                    if(sections[sI].elements[i].pos.y < (pos.y + scale.y) && sections[sI].elements[i].pos.y > (pos.y - scale.y)) //Don't render the unshown textures
                        sections[sI].elements[i].render(videoScale,mouse,timer,textRenderer);
                        
                    btnCounter++; //Indexing buttons to position them

                }
            }
        }

        endRenderingTheButtons:

        sliderButton.pos = pos;
        sliderButton.pos.x = pos.x + sliderButton.scale.x + scale.x;
        slideRatio = elementPos / (pos.y + scale.y);
        
        if(slideRatio > 1 && vertical){
            sliderButton.scale.y = scale.y / slideRatio;
            sliderButton.pos.y = (pos.y - scale.y) + sliderButton.scale.y + slideVal;  
            sliderButton.render(videoScale,mouse,timer,textRenderer);
            if(sliderButton.clickState1){ //Pressed
                slideVal += mouse.mouseOffset.y;
                if(slideVal < 0)
                    slideVal = 0;
                if(sliderButton.pos.y + sliderButton.scale.y >= pos.y + scale.y && mouse.mouseOffset.y > 0)
                    slideVal -= mouse.mouseOffset.y;
            } 
        }

        glClear(GL_DEPTH_BUFFER_BIT);
    }



public:
    Shader shader;

    //Attributes
    //Values are percentage of the monitor size
    glm::vec2 scale;  //Example : w : 20, h : 30 means cover %20 of the window in x axis & cover 30% of the window in y axis
    glm::vec3 pos; //Same here
    
    glm::vec4 color;
    glm::vec4 color2;
    
    std::vector<Section> sections;

    bool hover = false; //Panel's itself
    
    bool vertical = true;
    
    int rowCount; //How many element will share the same row

    float outlineThickness;

    //Sides of the panel
    PanelSide leftSide;
    PanelSide rightSide;
    PanelSide bottomSide;
    PanelSide topSide;

    float slideVal = 0.f;

    Button sliderButton;

    Panel(){}
    Panel(Shader shader,ColorPalette colorPalette,std::vector<Section> sections,glm::vec2 scale,glm::vec3 pos,glm::vec4 color,glm::vec4 color2,bool vertical,bool lockL,bool lockR,bool lockB,bool lockT,
          float outlineThickness,int rowCount){

        this->shader = shader;
        this->vertical = vertical;
        this->scale = scale;
        this->color = color;
        this->color2 = color2;
        this->pos = pos;
        this->sections = sections;
        this->outlineThickness = outlineThickness;
        this->leftSide.locked = lockL;
        this->rightSide.locked = lockR;
        this->bottomSide.locked = lockB;
        this->topSide.locked = lockT;
        this->rowCount = rowCount; 

        this->sliderButton = Button(1,glm::vec2(1,20),colorPalette,shader,"",Texture(),0.f,false);
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