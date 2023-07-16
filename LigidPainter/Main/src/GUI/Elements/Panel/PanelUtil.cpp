/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Contains constructors of the Panel class

*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/Elements/Elements.hpp"

#include <string>
#include <iostream>
#include <vector>

void Panel::mouseTracking(Mouse& mouse){
    //Check if mouse on the panel or any side of the panel 
    const float grabbingRange = 20; 
    
    //Check if mouse on top of the panel
    hover = mouse.isMouseHover(resultScale,glm::vec2(resultPos.x,resultPos.y));
    
    if(doMouseTracking){
        //Check if mouse on top of the left side of the panel
        leftSide.hover = mouse.isMouseHover(glm::vec2(grabbingRange,resultScale.y),glm::vec2(resultPos.x-resultScale.x,resultPos.y)) && !leftSide.locked;
        //Check if mouse on top of the right side of the panel
        rightSide.hover = mouse.isMouseHover(glm::vec2(grabbingRange,resultScale.y),glm::vec2(resultPos.x+resultScale.x,resultPos.y)) && !rightSide.locked;
        //Check if mouse on top of the bottom side of the panel
        bottomSide.hover = mouse.isMouseHover(glm::vec2(resultScale.x,grabbingRange),glm::vec2(resultPos.x,resultPos.y + resultScale.y)) && !bottomSide.locked;
        //Check if mouse on top of the top side of the panel
        topSide.hover = mouse.isMouseHover(glm::vec2(resultScale.x,grabbingRange),glm::vec2(resultPos.x,resultPos.y - resultScale.y)) && !topSide.locked;
    }
    else{
        leftSide.hover = false;
        rightSide.hover = false;
        bottomSide.hover = false;
        topSide.hover = false;
    }
    
    hover = hover || leftSide.hover || rightSide.hover || bottomSide.hover || topSide.hover;
    
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

void Panel::resizeThePanel(Mouse &mouse, glm::vec2 videoScale){
    //Resize the panel if necessary
    if(rightSide.pressed){
        scale.x += mouse.mouseOffset.x/videoScale.x * 50.f;
        pos.x += mouse.mouseOffset.x/videoScale.x *50.f;
        //Restrict the size of the panel
        if(scale.x < 1 || scale.x > maxScaleVal){
            scale.x -= mouse.mouseOffset.x/videoScale.x * 50.f;
            pos.x -= mouse.mouseOffset.x/videoScale.x * 50.f;
        }
    }
    else if(leftSide.pressed){
        scale.x -= mouse.mouseOffset.x/videoScale.x * 50.f;
        pos.x += mouse.mouseOffset.x/videoScale.x *50.f;
        //Restrict the size of the panel
        if(scale.x < 1 || scale.x > maxScaleVal){
            scale.x += mouse.mouseOffset.x/videoScale.x * 50.f;
            pos.x -= mouse.mouseOffset.x/videoScale.x *50.f;
        }
    }
    else if(bottomSide.pressed){
        scale.y += mouse.mouseOffset.y/videoScale.y * 50.f;
        pos.y += mouse.mouseOffset.y/videoScale.y *50.f;
        if(scale.y < 0.5f || scale.y > maxScaleVal){
            scale.y -= mouse.mouseOffset.y/videoScale.y * 50.f;
            pos.y -= mouse.mouseOffset.y/videoScale.y *50.f;
        }
    }
    
    else if(topSide.pressed){
        scale.y -= mouse.mouseOffset.y/videoScale.y * 50.f;
        pos.y += mouse.mouseOffset.y/videoScale.y *50.f;
        if(scale.y < 0.5f || scale.y > maxScaleVal){
            scale.y += mouse.mouseOffset.y/videoScale.y * 50.f;
            pos.y -= mouse.mouseOffset.y/videoScale.y *50.f;
        }
    }
}

void Panel::prepDrawBtnVertically(Element &button,Element &previousButton,float& elementPos,int btnCounter){
    button.scale.x = scale.x/rowCount;
    
    
    //Move the button on top of the panel
    button.pos = pos + this->additionalPos;

    button.pos.x -= scale.x;
    button.pos.x += button.scale.x;
    button.pos.x += button.scale.x*2.f * (btnCounter % rowCount);
    button.pos.y += button.scale.y;
    //button.pos.y += sections[sI].elements[0].scale.y;
    
    if(btnCounter && btnCounter % rowCount == 0)        
        elementPos += (button.scale.y + previousButton.scale.y) + button.panelOffset;
    button.pos.z += 0.01f;
    float screenGapY = 50 - scale.y*1.f;
    button.pos.y = elementPos - slideVal * slideRatio;
}

void Panel::prepDrawBtnHorizontally(Element &button,Element &previousButton,float& elementPos,int btnCounter){
    button.scale.y = scale.y;
    //Move the button on top of the panel
    button.pos = pos + this->additionalPos;
    //button.pos.x += sections[sI].elements[0].scale.x;
    if(btnCounter)        
        elementPos += (button.scale.x + previousButton.scale.x) + button.panelOffset;
    button.pos.z += 0.01f;
    button.pos.x = elementPos;
}