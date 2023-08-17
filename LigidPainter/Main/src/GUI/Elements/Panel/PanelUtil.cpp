/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

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
#include "MouseSystem/Mouse.hpp"

#include <string>
#include <iostream>
#include <vector>

void Panel::mouseTracking(){
    //Check if mouse on the panel or any side of the panel 
    const float grabbingRange = 20; 
    
    //Check if mouse on top of the panel
    hover = Mouse::isMouseHover(resultScale,glm::vec2(resultPos.x,resultPos.y));
    
    if(doMouseTracking){
        //Check if mouse on top of the left side of the panel
        leftSide.hover = Mouse::isMouseHover(glm::vec2(grabbingRange,resultScale.y),glm::vec2(resultPos.x-resultScale.x,resultPos.y)) && !leftSide.locked;
        //Check if mouse on top of the right side of the panel
        rightSide.hover = Mouse::isMouseHover(glm::vec2(grabbingRange,resultScale.y),glm::vec2(resultPos.x+resultScale.x,resultPos.y)) && !rightSide.locked;
        //Check if mouse on top of the bottom side of the panel
        bottomSide.hover = Mouse::isMouseHover(glm::vec2(resultScale.x,grabbingRange),glm::vec2(resultPos.x,resultPos.y + resultScale.y)) && !bottomSide.locked;
        //Check if mouse on top of the top side of the panel
        topSide.hover = Mouse::isMouseHover(glm::vec2(resultScale.x,grabbingRange),glm::vec2(resultPos.x,resultPos.y - resultScale.y)) && !topSide.locked;
    }
    else{
        leftSide.hover = false;
        rightSide.hover = false;
        bottomSide.hover = false;
        topSide.hover = false;
    }

    if(Mouse::isCursorPointer()){
        leftSide.hover = false;
        leftSide.pressed = false;
        rightSide.hover = false;
        rightSide.pressed = false;
        bottomSide.hover = false;
        bottomSide.pressed = false;
        topSide.hover = false;
        topSide.pressed = false;
    }
    
    hover = hover || leftSide.hover || rightSide.hover || bottomSide.hover || topSide.hover;

    resizingDone = (leftSide.pressed || rightSide.pressed || bottomSide.pressed || topSide.pressed);
    
    //Keep the left corner in the pressed state from the left click to left mouse button release
    if(leftSide.hover && *Mouse::LClick()) //Clicked to the corner
        leftSide.pressed = true;
    if(!*Mouse::LPressed()) //Corner released
        leftSide.pressed = false;
    
    //Keep the right corner in the pressed state from the left click to left mouse button release
    if(rightSide.hover && *Mouse::LClick()) //Clicked to the corner
        rightSide.pressed = true;
    if(!*Mouse::LPressed()) //Corner released
        rightSide.pressed = false;
    
    //Keep the bottom corner in the pressed state from the left click to left mouse button release
    if(bottomSide.hover && *Mouse::LClick()) //Clicked to the corner
        bottomSide.pressed = true;
    if(!*Mouse::LPressed()) //Corner released
        bottomSide.pressed = false;
    
    //Keep the top corner in the pressed state from the left click to left mouse button release
    if(topSide.hover && *Mouse::LClick()) //Clicked to the corner
        topSide.pressed = true;
    if(!*Mouse::LPressed()) //Corner released
        topSide.pressed = false;
    
    //Change the cursor
    if(leftSide.hover || leftSide.pressed || rightSide.hover || rightSide.pressed)
        Mouse::setCursor(*Mouse::hSlideCursor());
    if(bottomSide.hover || bottomSide.pressed || topSide.hover || topSide.pressed)
        Mouse::setCursor(*Mouse::vSlideCursor());
}

void Panel::resizeThePanel(glm::vec2 videoScale){
    //Resize the panel if necessary
    if(rightSide.pressed){
        scale.x += Mouse::mouseOffset()->x/videoScale.x * 50.f;
        pos.x += Mouse::mouseOffset()->x/videoScale.x *50.f;
        //Restrict the size of the panel
        if(scale.x < 1 || scale.x > maxScaleVal){
            scale.x -= Mouse::mouseOffset()->x/videoScale.x * 50.f;
            pos.x -= Mouse::mouseOffset()->x/videoScale.x * 50.f;
        }
    }
    else if(leftSide.pressed){
        scale.x -= Mouse::mouseOffset()->x/videoScale.x * 50.f;
        pos.x += Mouse::mouseOffset()->x/videoScale.x *50.f;
        //Restrict the size of the panel
        if(scale.x < 1 || scale.x > maxScaleVal){
            scale.x += Mouse::mouseOffset()->x/videoScale.x * 50.f;
            pos.x -= Mouse::mouseOffset()->x/videoScale.x *50.f;
        }
    }
    else if(bottomSide.pressed){
        scale.y += Mouse::mouseOffset()->y/videoScale.y * 50.f;
        pos.y += Mouse::mouseOffset()->y/videoScale.y *50.f;
        if(scale.y < 0.5f || scale.y > maxScaleVal){
            scale.y -= Mouse::mouseOffset()->y/videoScale.y * 50.f;
            pos.y -= Mouse::mouseOffset()->y/videoScale.y *50.f;
        }
    }
    
    else if(topSide.pressed){
        scale.y -= Mouse::mouseOffset()->y/videoScale.y * 50.f;
        pos.y += Mouse::mouseOffset()->y/videoScale.y *50.f;
        if(scale.y < 0.5f || scale.y > maxScaleVal){
            scale.y += Mouse::mouseOffset()->y/videoScale.y * 50.f;
            pos.y -= Mouse::mouseOffset()->y/videoScale.y *50.f;
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
    button.pos.y = elementPos - slideVal / slideRatio;
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

