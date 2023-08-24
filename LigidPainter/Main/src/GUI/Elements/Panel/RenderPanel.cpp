/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

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

#include "GUI/Elements/Elements.hpp"
#include "ShaderSystem/Shader.hpp"
#include "MouseSystem/Mouse.hpp"
#include "SettingsSystem/Settings.hpp"

#include <string>
#include <iostream>
#include <vector>
    
void Panel::render(Timer &timer,bool doMouseTracking){
    
    this->doMouseTracking = doMouseTracking;
    
    // Pos value, % of the video scale
    resultPos = glm::vec3( 
                            UTIL::getPercent(*Settings::videoScale(), glm::vec2(pos.x + this->additionalPos.x, pos.y + this->additionalPos.y)), //Don't include the depth
                            pos.z //Use the original depth value
                        ); 
    
    // scale value % of the video scale
    resultScale = UTIL::getPercent(*Settings::videoScale(), scale);
    
    mouseTracking();
    
    resizeThePanel();

    drawPanel(resultPos,resultScale,timer);

}

static void drawThePanel(glm::vec3 pos, glm::vec2 scale, glm::vec4 color, glm::vec4 color2, float outlineThickness){
    
    ShaderSystem::buttonShader().setVec3  ("pos", pos);
    ShaderSystem::buttonShader().setVec2  ("scale", scale);
    ShaderSystem::buttonShader().setVec4  ("properties.color", color);
    ShaderSystem::buttonShader().setVec4  ("properties.color2", color);
    ShaderSystem::buttonShader().setFloat ("properties.colorMixVal", 0.f);
    
    ShaderSystem::buttonShader().setFloat ("properties.radius", 10.f);
    ShaderSystem::buttonShader().setInt   ("properties.outline.state", 2);
    ShaderSystem::buttonShader().setInt   ("outlineExtra", true); 
    ShaderSystem::buttonShader().setVec3  ("properties.outline.color" , color2); 
    ShaderSystem::buttonShader().setVec3  ("properties.outline.color2" , color2); 
    ShaderSystem::buttonShader().setFloat ("properties.outline.thickness" , outlineThickness);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

static void drawTheBarriers(glm::vec3 resultPos, glm::vec2 resultScale){
    
    //Bottom
    ShaderSystem::buttonShader().setVec3("pos", glm::vec3(resultPos.x,   resultPos.y + resultScale.y + 5000,   1.f)); 
    ShaderSystem::buttonShader().setVec2("scale", glm::vec2(5000));
    ShaderSystem::buttonShader().setFloat("properties.radius", 0.f); 
    ShaderSystem::buttonShader().setInt("outlineExtra" , false); 
    ShaderSystem::buttonShader().setVec4("properties.color", glm::vec4(0)); //Invisible
    ShaderSystem::buttonShader().setVec3("properties.outline.color", glm::vec4(0)); //Invisible
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    //Top
    ShaderSystem::buttonShader().setVec3("pos", glm::vec3(resultPos.x,   resultPos.y - resultScale.y - 5000,   1.f));
    ShaderSystem::buttonShader().setVec2("scale", glm::vec2(5000));
    glDrawArrays(GL_TRIANGLES, 0, 6);

}

static float calculateElementStartingPosition(bool vertical, std::vector<Section> sections, glm::vec3 pos, glm::vec2 scale){
    
    float elementPos = 0.f;
    
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
    
    return elementPos;
}

static void renderBarButtons(std::vector<Button> &barButtons, glm::vec3 pos, glm::vec2 scale, Timer timer, bool doMouseTracking){
    
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
        barButtons[i].render(timer,doMouseTracking);
    }

}

void Panel::renderTheHeader(int sectionI,float &elementPos, int &btnCounter, Timer &timer){
    //Prepare the transform data of the button    
    if(this->vertical)
        this->prepDrawBtnVertically(sections[sectionI].header,sections[std::max(sectionI,0)].elements[sections[std::max(sectionI,0)].elements.size()-1],elementPos,btnCounter);
    else
        this->prepDrawBtnHorizontally(sections[sectionI].header,sections[std::max(sectionI,0)].elements[sections[std::max(sectionI,0)].elements.size()-1],elementPos,btnCounter);
    //Draw the button
    this->sections[sectionI].header.render(timer, this->doMouseTracking && !sliderButton.hover);
    btnCounter++; //Indexing buttons to position them
}

void Panel::drawPanel(
                        glm::vec3 resultPos,
                        glm::vec2 resultScale,
                        Timer &timer
                    )
{

    //Draw the panel's itself
    drawThePanel(resultPos, resultScale, this->color, this->color2, this->outlineThickness);

    /*Render the barriers if the depth texture of the framebuffer will be refreshed at the end of the panel rendering process*/
    if(clearDepthBuffer)
    {
        drawTheBarriers(resultPos, resultScale);
    }

    //Starting pos
    float elementStartPos = calculateElementStartingPosition(this->vertical, this->sections, this->pos + this->additionalPos, this->scale);
    float elementPos = elementStartPos;

    //Indexing buttons to position them
    int btnCounter = 0; 

    //Render the bar buttons
    renderBarButtons(this->barButtons, this->pos + this->additionalPos, this->scale, timer, this->doMouseTracking && !sliderButton.hover); 
    
    //Calculate the barbuttons for positioning other elements
    if(barButtons.size())
        elementPos += (barButtons[0].scale.y)*2.f;
    
    float lastElementScale = 0.f;

    for (int sI = 0; sI < sections.size(); sI++)
    {
        /*-- HEADER --*/
        if(sections[sI].header.button.text.size()) //If there is a header
        { 
            //Section header button
            this->renderTheHeader(sI, elementPos, btnCounter, timer);
        }

        /*-- ELEMENTS --*/
        if(
                sections[sI].header.button.clickState1 == true || //If the header button is pressed 
                !sections[sI].header.button.text.size()  //Or there is no header
            ) 
        {
            /*Render each element of the section*/
            for (int i = 0; i < sections[sI].elements.size(); i++) //
            {
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
                
                //Don't render the unshown elements
                if(this->sections[sI].elements[i].pos.y - this->sections[sI].elements[i].scale.y < (this->pos.y + this->additionalPos.y + this->scale.y) && this->sections[sI].elements[i].pos.y + this->sections[sI].elements[i].scale.y > (this->pos.y + this->additionalPos.y - this->scale.y)){
                    
                    sections[sI].elements[i].render(timer,doMouseTracking && !sliderButton.hover);
                    
                    /* Render the text if rendering library displayer panel*/
                    if(isLibraryDisplayer){
                        glm::vec4 textColor = glm::vec4(1) - sections[sI].elements[i].button.color;
                        textColor.a = 1.;
                        ShaderSystem::buttonShader().setVec4("properties.color"  ,    textColor      ); //Default button color

                        textRenderer.loadTextData(
                                                    sections[sI].elements[i].button.text,
                                                    glm::vec3(sections[sI].elements[i].button.resultPos.x,sections[sI].elements[i].button.resultPos.y + sections[sI].elements[i].button.resultScale.y/1.4f,sections[sI].elements[i].button.resultPos.z),
                                                    false,
                                                    0.25f, //TODO Change with a dynamic value
                                                    sections[sI].elements[i].button.resultPos.x - sections[sI].elements[i].button.resultScale.x,
                                                    sections[sI].elements[i].button.resultPos.x + sections[sI].elements[i].button.resultScale.x,
                                                    TEXTRENDERER_ALIGNMENT_MID
                                                );

                        textRenderer.renderText();
                    }
                }
                btnCounter++; //Indexing buttons to position them
            }
        }
    }
    endRenderingTheButtons:

    /* Render the slider of the panel*/
    if(hasSlider){
        
        sliderButton.pos = pos + this->additionalPos;
        
        if(leftSide.locked)
            sliderButton.pos.x = pos.x + this->additionalPos.x - sliderButton.scale.x + scale.x;
        else
            sliderButton.pos.x = pos.x + this->additionalPos.x + sliderButton.scale.x - scale.x;
        
        float elementHeight = (elementPos - elementStartPos)/2.f + lastElementScale;

        slideRatio = scale.y/elementHeight;

        if(slideRatio < 1 && vertical){
            if(sliderButton.clickState1){ //Pressed
                //Move the slidebar
                slideVal += Mouse::mouseOffset()->y / Settings::videoScale()->y*100.f;
            } 
            if(this->hover){
                if(*Mouse::mouseScroll() > 100)
                    *Mouse::mouseScroll() = 100;
                if(*Mouse::mouseScroll() < -100)
                    *Mouse::mouseScroll() = -100;
                slideVal -= *Mouse::mouseScroll() / Settings::videoScale()->y * 100.f * slideRatio;
            }
            
            if(slideVal < 0) //If the slider is out of boundaries
                slideVal = 0; //Get the slide bar back

            if ((pos.y + this->additionalPos.y - scale.y) + sliderButton.scale.y + slideVal + sliderButton.scale.y > pos.y + this->additionalPos.y + scale.y) {
                // If the slider is out of boundaries

                // Calculate the maximum value for slideVal
                float maxSlideVal = (pos.y + this->additionalPos.y + scale.y) - (pos.y + this->additionalPos.y - scale.y) - sliderButton.scale.y - sliderButton.scale.y;

                // Set slideVal to its maximum value
                slideVal = maxSlideVal;
            }
            
            sliderButton.scale.y = scale.y * slideRatio;
            sliderButton.pos.y = (pos.y + this->additionalPos.y - scale.y) + sliderButton.scale.y + slideVal;  
            sliderButton.pos.z += 0.02f;

            sliderButton.render(timer,doMouseTracking);

        }
        else
            this->slideVal = 0.f; 
    }
    
    
    if(clearDepthBuffer)
        glClear(GL_DEPTH_BUFFER_BIT);
}