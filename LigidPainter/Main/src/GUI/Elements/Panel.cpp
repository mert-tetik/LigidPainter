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

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/Elements.hpp"

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
    button.pos = pos;
    
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
    button.pos = pos;
    //button.pos.x += sections[sI].elements[0].scale.x;
    if(btnCounter)        
        elementPos += (button.scale.x + previousButton.scale.x) + button.panelOffset;
    button.pos.z += 0.01f;
    button.pos.x = elementPos;
}
    
void Panel::drawPanel(glm::vec2 videoScale,Mouse &mouse, glm::vec3 resultPos,glm::vec2 resultScale,Timer &timer,TextRenderer &textRenderer){
    //Draw the panel and it's elements
    //Draw panel
    shader.setVec3("pos",       resultPos);
    shader.setVec2("scale",     resultScale);
    shader.setVec4("properties.color",     color   );
    shader.setVec4("properties.color2"  ,     color     );
    shader.setFloat("properties.colorMixVal"  ,   0.f );
    
    shader.setFloat("properties.radius",     10.f   );
    shader.setInt("properties.outline.state",     2   );
    shader.setInt("outlineExtra" ,    true     ); 
    shader.setVec3("properties.outline.color" ,    color2     ); 
    shader.setVec3("properties.outline.color2" ,    color2     ); 
    shader.setFloat("properties.outline.thickness" ,    outlineThickness  );
    glDrawArrays(GL_TRIANGLES, 0, 6);
    if(clearDepthBuffer){
        //Barriers (In order to prevent the overflow)
        //Bottom
        shader.setVec3("pos",       glm::vec3(resultPos.x,resultPos.y + resultScale.y + 5000,1.f)); //To the bottom
        shader.setVec2("scale",     glm::vec2(5000));
        shader.setFloat("properties.radius",     0.f   ); 
        shader.setInt("outlineExtra" ,    false     ); 
        shader.setVec4("properties.color",     glm::vec4(0)   ); //Invisible
        shader.setVec3("properties.outline.color" ,    glm::vec4(0)     ); //Invisible
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //Top
        shader.setVec3("pos",       glm::vec3(resultPos.x,resultPos.y - resultScale.y - 5000,1.f)); //To the bottom
        shader.setVec2("scale",     glm::vec2(5000));
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    //Panel's buttons
    float elementPos = 0.f; //Starting pos
    float lastElementScale = 0.f;
    //Get the starting position
    if(vertical){//If the panel is vertical
        if(sections.size()){//If there are sections
            if(sections[0].header.button.text.size()) //If there is a header in the section
                elementPos = pos.y-scale.y + sections[0].header.scale.y + sections[0].header.panelOffset; //Take the starting position by including the height of the section header
            else if(sections[0].elements.size()) //If there are buttons in the first section
                elementPos = pos.y-scale.y + sections[0].elements[0].scale.y + sections[0].elements[0].panelOffset; //Take the starting position by including the height of the first button of the section
        }
    }
    else{//If the panel is horizontal
        if(sections.size()){//If there are sections
            if(sections[0].header.button.text.size())  //If there is a header in the section
                elementPos = pos.x-scale.x + sections[0].header.scale.x + sections[0].header.panelOffset; //Take the starting position by including the height of the section header
            else if(sections[0].elements.size())//If there are buttons in the first section
                elementPos = pos.x-scale.x + sections[0].elements[0].scale.x + sections[0].elements[0].panelOffset; //Take the starting position by including the width of the first button of the section
        }
    }
    
    //Render sections
    int btnCounter = 0; //Indexing buttons to position them
    //Render the bar buttons
    for (size_t i = 0; i < barButtons.size(); i++) //Bar buttons are used only in the vertical panels
    {
        barButtons[i].scale.x = scale.x/barButtons.size();
    
        //Move the buttons on top of the panel
        barButtons[i].pos = pos;
        barButtons[i].pos.x -= scale.x;
        barButtons[i].pos.x += barButtons[i].scale.x;
        barButtons[i].pos.x += barButtons[i].scale.x*2.f * (i % barButtons.size());
        barButtons[i].pos.y -= scale.y - barButtons[i].scale.y;
        
        barButtons[i].pos.z += 0.01f;
        barButtons[i].render(videoScale,mouse,timer,textRenderer,doMouseTracking);
    }
    if(barButtons.size())
        elementPos += (barButtons[0].scale.y)*2.f;
    
    for (int sI = 0; sI < sections.size(); sI++)
    {
        if(sections[sI].header.button.text.size()){ //If there is a header
            //Section header button
            //Prepare the transform data of the button    
            if(vertical)
                prepDrawBtnVertically(sections[sI].header,sections[std::max(sI-1,0)].elements[sections[std::max(sI-1,0)].elements.size()-1],elementPos,btnCounter);
            else
                prepDrawBtnHorizontally(sections[sI].header,sections[std::max(sI-1,0)].elements[sections[std::max(sI-1,0)].elements.size()-1],elementPos,btnCounter);
            //Draw the button
            sections[sI].header.render(videoScale,mouse,timer,textRenderer,doMouseTracking);
            btnCounter++; //Indexing buttons to position them
        }
        if(sections[sI].header.button.clickState1 == true || !sections[sI].header.button.text.size()){
            for (int i = 0; i < sections[sI].elements.size(); i++) //
            {
                //Buttons of the current section
                //Prepare the transform data of the button    
                if(vertical)
                    prepDrawBtnVertically(sections[sI].elements[i],sections[sI].elements[std::max(i-1,0)],elementPos,btnCounter);
                else
                    prepDrawBtnHorizontally(sections[sI].elements[i],sections[sI].elements[std::max(i-1,0)],elementPos,btnCounter);
                if(isLibraryDisplayer){
                    sections[sI].elements[i].button.textureStickToTop = true;
                    sections[sI].elements[i].button.textureSizeScale = 1.5f;
                }
                lastElementScale = sections[sI].elements[i].scale.y; 
                //Draw the button
                if(sections[sI].elements[i].pos.y - sections[sI].elements[i].scale.y < (pos.y + scale.y) && sections[sI].elements[i].pos.y + sections[sI].elements[i].scale.y > (pos.y - scale.y)){
                    //Don't render the unshown elements
                    
                    sections[sI].elements[i].render(videoScale,mouse,timer,textRenderer,doMouseTracking);
                    
                    if(isLibraryDisplayer){
                        glm::vec4 textColor = glm::vec4(1) - sections[sI].elements[i].button.color;
                        textColor.a = 1.;
                        shader.setVec4("properties.color"  ,    textColor      ); //Default button color

                        textRenderer.loadTextData(
                                                    shader,
                                                    sections[sI].elements[i].button.text,
                                                    glm::vec3(sections[sI].elements[i].button.resultPos.x,sections[sI].elements[i].button.resultPos.y + sections[sI].elements[i].button.resultScale.y/1.4f,sections[sI].elements[i].button.resultPos.z),
                                                    false,
                                                    0.25f, //TODO Change with a dynamic value
                                                    sections[sI].elements[i].button.resultPos.x - sections[sI].elements[i].button.resultScale.x,
                                                    sections[sI].elements[i].button.resultPos.x + sections[sI].elements[i].button.resultScale.x,
                                                    TEXTRENDERER_ALIGNMENT_MID
                                                );

                        textRenderer.renderText(shader);
                    }
                }
                btnCounter++; //Indexing buttons to position them
            }
        }
    }
    endRenderingTheButtons:

    if(hasSlider){
        sliderButton.pos = pos;
        if(leftSide.locked)
            sliderButton.pos.x = pos.x + sliderButton.scale.x + scale.x;
        else
            sliderButton.pos.x = pos.x - sliderButton.scale.x - scale.x;
        slideRatio = (elementPos + lastElementScale + (lastElementScale*2.f)) / (pos.y + scale.y);

        if(slideRatio > 1 && vertical){
            sliderButton.scale.y = scale.y / slideRatio;
            sliderButton.pos.y = (pos.y - scale.y) + sliderButton.scale.y + slideVal;  

            sliderButton.render(videoScale,mouse,timer,textRenderer,doMouseTracking);

            //if(hover)
            //    slideVal += mouse.mouseScroll;

            if(sliderButton.clickState1){ //Pressed
                //Move the slidebar
                slideVal += mouse.mouseOffset.y/videoScale.y*100.f;
            } 

            if(slideVal < 0) //If the slider is out of boundaries
                slideVal = 0; //Get the slide bar back
            if (sliderButton.pos.y + sliderButton.scale.y >= pos.y + scale.y && mouse.mouseOffset.y > 0) {
                // If the slider is out of boundaries
                slideVal = sliderButton.pos.y - (pos.y - scale.y) - sliderButton.scale.y; // Set slideVal to its maximum value
            }
        }
    }
    
    if(clearDepthBuffer)
        glClear(GL_DEPTH_BUFFER_BIT);
}


Panel::Panel(){}
Panel::Panel(Shader shader,ColorPalette colorPalette,std::vector<Section> sections,glm::vec2 scale,glm::vec3 pos,glm::vec4 color,glm::vec4 color2,bool vertical,bool lockL,bool lockR,bool lockB,bool lockT,
      float outlineThickness,int rowCount,std::vector<Button> barButtons,float maxScaleVal,bool hasSlider){
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
    this->maxScaleVal = maxScaleVal; 
    this->barButtons = barButtons; 
    this->sliderButton = Button(ELEMENT_STYLE_STYLIZED,glm::vec2(0.25f,20),colorPalette,shader,"",Texture(),0.f,false);
    this->sliderButton.color = colorPalette.mainColor;
    this->sliderButton.color2 = colorPalette.themeColor;
    this->sliderButton.radius = 0.25f;
    this->hasSlider = hasSlider;
}

void Panel::render(glm::vec2 videoScale,Mouse& mouse,Timer &timer,TextRenderer &textRenderer,bool doMouseTracking){
    this->doMouseTracking = doMouseTracking;
    
    //Panel's itself
    ;
    // pos value % of the video scale
    resultPos = glm::vec3( 
                          UTIL::getPercent(videoScale,glm::vec2(pos.x,pos.y)) //Don't include the depth
                          ,pos.z); //Use the original depth value
    // scale value % of the video scale
    resultScale = UTIL::getPercent(videoScale,scale);
    mouseTracking(mouse);
    resizeThePanel(mouse,videoScale);

    drawPanel(videoScale,mouse,resultPos,resultScale,timer,textRenderer);
}