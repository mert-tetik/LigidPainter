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
    
void Panel::render(glm::vec2 videoScale,Mouse& mouse,Timer &timer,TextRenderer &textRenderer,bool doMouseTracking){
    
    this->doMouseTracking = doMouseTracking;
    
    // Pos value, % of the video scale
    resultPos = glm::vec3( 
                            UTIL::getPercent(videoScale,glm::vec2(pos.x,pos.y)), //Don't include the depth
                            pos.z //Use the original depth value
                        ); 
    
    // scale value % of the video scale
    resultScale = UTIL::getPercent(videoScale,scale);
    
    mouseTracking(mouse);
    
    resizeThePanel(mouse,videoScale);

    drawPanel(videoScale,mouse,resultPos,resultScale,timer,textRenderer);

}

static void drawThePanel(Shader shader, glm::vec3 pos, glm::vec2 scale, glm::vec4 color, glm::vec4 color2, float outlineThickness){
    
    shader.setVec3  ("pos", pos);
    shader.setVec2  ("scale", scale);
    shader.setVec4  ("properties.color", color);
    shader.setVec4  ("properties.color2", color);
    shader.setFloat ("properties.colorMixVal", 0.f);
    
    shader.setFloat ("properties.radius", 10.f);
    shader.setInt   ("properties.outline.state", 2);
    shader.setInt   ("outlineExtra", true); 
    shader.setVec3  ("properties.outline.color" , color2); 
    shader.setVec3  ("properties.outline.color2" , color2); 
    shader.setFloat ("properties.outline.thickness" , outlineThickness);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

static void drawTheBarriers(Shader shader, glm::vec3 resultPos, glm::vec2 resultScale){
    
    //Bottom
    shader.setVec3("pos", glm::vec3(resultPos.x,   resultPos.y + resultScale.y + 5000,   1.f)); 
    shader.setVec2("scale", glm::vec2(5000));
    shader.setFloat("properties.radius", 0.f); 
    shader.setInt("outlineExtra" , false); 
    shader.setVec4("properties.color", glm::vec4(0)); //Invisible
    shader.setVec3("properties.outline.color", glm::vec4(0)); //Invisible
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    //Top
    shader.setVec3("pos", glm::vec3(resultPos.x,   resultPos.y - resultScale.y - 5000,   1.f));
    shader.setVec2("scale", glm::vec2(5000));
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

static void renderBarButtons(std::vector<Button> &barButtons, glm::vec3 pos, glm::vec2 scale, glm::vec2 videoScale, Mouse mouse, Timer timer, TextRenderer textRenderer, bool doMouseTracking){
    
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

}

void Panel::renderTheHeader(int sectionI,float &elementPos, int &btnCounter, glm::vec2 videoScale, Mouse &mouse, Timer &timer, TextRenderer &textRenderer){
    //Prepare the transform data of the button    
    if(this->vertical)
        this->prepDrawBtnVertically(sections[sectionI].header,sections[std::max(sectionI-1,0)].elements[sections[std::max(sectionI-1,0)].elements.size()-1],elementPos,btnCounter);
    else
        this->prepDrawBtnHorizontally(sections[sectionI].header,sections[std::max(sectionI-1,0)].elements[sections[std::max(sectionI-1,0)].elements.size()-1],elementPos,btnCounter);
    //Draw the button
    this->sections[sectionI].header.render(videoScale, mouse, timer, textRenderer, this->doMouseTracking);
    btnCounter++; //Indexing buttons to position them
}

void Panel::drawPanel(
                        glm::vec2 videoScale,
                        Mouse &mouse, 
                        glm::vec3 resultPos,
                        glm::vec2 resultScale,
                        Timer &timer,
                        TextRenderer &textRenderer
                    )
{

    //Draw the panel's itself
    drawThePanel(this->shader, resultPos, resultScale, this->color, this->color2, this->outlineThickness);

    /*Render the barriers if the depth texture of the framebuffer will be refreshed at the end of the panel rendering process*/
    if(clearDepthBuffer)
    {
        drawTheBarriers(shader, resultPos, resultScale);
    }

    //Starting pos
    float elementPos = calculateElementStartingPosition(this->vertical, this->sections, this->pos, this->scale);

    //Indexing buttons to position them
    int btnCounter = 0; 

    //Render the bar buttons
    renderBarButtons(this->barButtons, this->pos, this->scale, videoScale, mouse, timer, textRenderer, this->doMouseTracking);
    
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
            this->renderTheHeader(sI, elementPos, btnCounter, videoScale, mouse, timer, textRenderer);
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
                if(this->sections[sI].elements[i].pos.y - this->sections[sI].elements[i].scale.y < (this->pos.y + this->scale.y) && this->sections[sI].elements[i].pos.y + this->sections[sI].elements[i].scale.y > (this->pos.y - this->scale.y)){
                    
                    sections[sI].elements[i].render(videoScale,mouse,timer,textRenderer,doMouseTracking);
                    
                    /* Render the text if rendering library displayer panel*/
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

    /* Render the slider of the panel*/
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