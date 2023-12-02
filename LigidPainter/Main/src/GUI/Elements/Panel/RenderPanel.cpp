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
#include <glm/gtx/string_cast.hpp>

#include "GUI/Elements/Elements.hpp"
#include "ShaderSystem/Shader.hpp"
#include "MouseSystem/Mouse.hpp"
#include "SettingsSystem/Settings.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

    
void Panel::render(Timer &timer, bool doMouseTracking){
    
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

    if(preRenderingMode){
        this->updateUpdateGraphicsFlag();

        const double effectDuration = 2.;

        if(this->updateGraphicsFlag || LigidGL::getTime() - this->updateGraphicsTime < effectDuration){
            if(this->updateGraphicsFlag)
                this->updateGraphicsTime = LigidGL::getTime();

            this->updateGraphics(timer);
            this->updateGraphicsFlag = false;
        }
        
        glm::mat4 projection = glm::ortho(0.f, (float)getContext()->windowScale.x,(float)getContext()->windowScale.y,0.f);

        getBox()->bindBuffers();

        // Render the panel's graphics
        ShaderSystem::textureRenderingShader().use();
        ShaderSystem::textureRenderingShader().setMat4("projection", projection);
        ShaderSystem::textureRenderingShader().setVec3("pos", resultPos);
        ShaderSystem::textureRenderingShader().setVec2("scale", resultScale);
        ShaderSystem::textureRenderingShader().setFloat("rotation", 0.f);
        ShaderSystem::textureRenderingShader().setFloat("opacity", 1.f);
        ShaderSystem::textureRenderingShader().setInt("txtr", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, graphics.ID);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        ShaderSystem::buttonShader().use();    
        
        if(this->clearDepthBuffer)
            glClear(GL_DEPTH_BUFFER_BIT);
    }

    else{
        drawPanel(resultPos, resultScale, timer);
    }
    

    prevScale = this->scale;
    prevPos = this->pos;
    prevSlideVal = this->slideVal;
    prevSections = this->sections;
    prevBarButtons = this->barButtons;
}

void Panel::updateUpdateGraphicsFlag(){
    if(this->hover)
        updateGraphicsFlag = true;

    if(barButtons.size() == prevBarButtons.size()){
        for (size_t i = 0; i < prevBarButtons.size(); i++)
        {
            if(
                    prevBarButtons[i].text != barButtons[i].text ||
                    prevBarButtons[i].hoverMixVal != barButtons[i].hoverMixVal ||
                    prevBarButtons[i].clickedMixVal != barButtons[i].clickedMixVal || 
                    prevBarButtons[i].clicked || barButtons[i].clicked ||
                    prevBarButtons[i].color != barButtons[i].color ||
                    prevBarButtons[i].texture.ID != barButtons[i].texture.ID 
                )
            {
                this->updateGraphicsFlag = true;
            }
        }
    }
    else
        this->updateGraphicsFlag = true;

    if(prevSections.size() == sections.size()){
        for (size_t secI = 0; secI < prevSections.size(); secI++)
        {
            if(
                    prevSections[secI].header.button.text != sections[secI].header.button.text ||
                    prevSections[secI].header.button.hoverMixVal != sections[secI].header.button.hoverMixVal ||
                    prevSections[secI].header.button.clickedMixVal != sections[secI].header.button.clickedMixVal || 
                    prevSections[secI].header.button.clicked || sections[secI].header.button.clicked ||
                    prevSections[secI].header.button.color != sections[secI].header.button.color ||
                    prevSections[secI].header.button.texture.ID != sections[secI].header.button.texture.ID
                )
            {
                this->updateGraphicsFlag = true;
            }

            if(prevSections[secI].elements.size() == sections[secI].elements.size()){
                for (size_t eI = 0; eI < prevSections[secI].elements.size(); eI++)
                {
                    bool comboBoxChanged = false;

                    if(
                            prevSections[secI].elements[eI].button.text != sections[secI].elements[eI].button.text ||
                            prevSections[secI].elements[eI].button.hoverMixVal != sections[secI].elements[eI].button.hoverMixVal ||
                            prevSections[secI].elements[eI].button.clickedMixVal != sections[secI].elements[eI].button.clickedMixVal ||
                            prevSections[secI].elements[eI].button.clicked || sections[secI].elements[eI].button.clicked ||
                            prevSections[secI].elements[eI].button.color != sections[secI].elements[eI].button.color ||
                            prevSections[secI].elements[eI].button.texture.ID != sections[secI].elements[eI].button.texture.ID ||
                            prevSections[secI].elements[eI].rangeBar.value != sections[secI].elements[eI].rangeBar.value ||
                            prevSections[secI].elements[eI].rangeBar.hoverMixVal != sections[secI].elements[eI].rangeBar.hoverMixVal ||
                            prevSections[secI].elements[eI].rangeBar.clickedMixVal != sections[secI].elements[eI].rangeBar.clickedMixVal ||
                            prevSections[secI].elements[eI].rangeBar.leftArrowMixVal != sections[secI].elements[eI].rangeBar.leftArrowMixVal ||
                            prevSections[secI].elements[eI].rangeBar.rightArrowMixVal != sections[secI].elements[eI].rangeBar.rightArrowMixVal ||
                            prevSections[secI].elements[eI].rangeBar.leftArrowClickedMixVal != sections[secI].elements[eI].rangeBar.leftArrowClickedMixVal ||
                            prevSections[secI].elements[eI].rangeBar.rightArrowClickedMixVal != sections[secI].elements[eI].rangeBar.rightArrowClickedMixVal ||
                            prevSections[secI].elements[eI].textBox.text != sections[secI].elements[eI].textBox.text ||
                            prevSections[secI].elements[eI].textBox.hoverMixVal != sections[secI].elements[eI].textBox.hoverMixVal ||
                            prevSections[secI].elements[eI].textBox.clickedMixVal != sections[secI].elements[eI].textBox.clickedMixVal ||
                            prevSections[secI].elements[eI].checkBox.clickState1 != sections[secI].elements[eI].checkBox.clickState1 ||
                            prevSections[secI].elements[eI].checkBox.hoverMixVal != sections[secI].elements[eI].checkBox.hoverMixVal ||
                            prevSections[secI].elements[eI].checkBox.clickedMixVal != sections[secI].elements[eI].checkBox.clickedMixVal ||
                            prevSections[secI].elements[eI].scale != sections[secI].elements[eI].scale ||
                            comboBoxChanged
                        )
                    {
                        this->updateGraphicsFlag = true;
                        break;
                    }
                }
            }
            else{
                this->updateGraphicsFlag = true;
            }
            
        }
    }
    else{
        this->updateGraphicsFlag = true;
    }

    if(prevScale != this->scale){
        this->updateGraphicsFlag = true;
    }
    if(prevPos != this->pos){
        this->updateGraphicsFlag = true;
    }
    if(prevSlideVal != this->slideVal){
        this->updateGraphicsFlag = true;
    }
}

void Panel::updateGraphics(Timer &timer){
    glm::vec2 displayRes = this->resultScale * 2.f; 

    if(!this->graphics.ID){
        glGenTextures(1, &this->graphics.ID);
    }
    this->graphics.update(nullptr, displayRes.x, displayRes.y, GL_NEAREST);

    Framebuffer captureGraphicsFBO = Framebuffer(this->graphics, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, glm::ivec2(displayRes)));
    captureGraphicsFBO.bind();

    glClearColor(this->color.r, this->color.g, this->color.b, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, displayRes.x, displayRes.y);

    glm::mat4 projection = glm::ortho(
                                        resultPos.x - resultScale.x, 
                                        (resultPos.x + resultScale.x) , 
                                        (resultPos.y + resultScale.y) , 
                                        resultPos.y - resultScale.y
                                    );
    ShaderSystem::buttonShader().setMat4("projection", projection);

    drawPanel(resultPos, this->resultScale, timer);
    
    //Finish
    projection = glm::ortho(0.f, (float)getContext()->windowScale.x,(float)getContext()->windowScale.y,0.f);
    ShaderSystem::buttonShader().setMat4("projection", projection);

    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();
    captureGraphicsFBO.deleteBuffers(false, true);
}

static void drawThePanel(glm::vec3 pos, glm::vec2 scale, glm::vec4 color, glm::vec4 color2, float outlineThickness, float cornerRadius){
    
    ShaderSystem::buttonShader().setVec3  ("pos", pos);
    ShaderSystem::buttonShader().setVec2  ("scale", scale);
    ShaderSystem::buttonShader().setVec4  ("properties.color", color);
    ShaderSystem::buttonShader().setVec4  ("properties.color2", color);
    ShaderSystem::buttonShader().setFloat ("properties.colorMixVal", 0.f);
    
    ShaderSystem::buttonShader().setFloat ("properties.radius", cornerRadius);
    ShaderSystem::buttonShader().setInt   ("properties.outline.state", 2);
    ShaderSystem::buttonShader().setInt   ("outlineExtra", true); 
    ShaderSystem::buttonShader().setVec3  ("properties.outline.color" , color2); 
    ShaderSystem::buttonShader().setVec3  ("properties.outline.color2" , color2); 
    ShaderSystem::buttonShader().setFloat ("properties.outline.thickness" , outlineThickness);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

static void drawTheBarriers(glm::vec3 resultPos, glm::vec2 resultScale, bool isLibraryDisplayer){
    
    //Bottom
    ShaderSystem::buttonShader().setVec3("pos", glm::vec3(resultPos.x,   resultPos.y + resultScale.y + 2000,   1.f)); 
    ShaderSystem::buttonShader().setVec2("scale", glm::vec2(2000));
    ShaderSystem::buttonShader().setFloat("properties.radius", 0.f); 
    ShaderSystem::buttonShader().setInt("outlineExtra" , false); 
    ShaderSystem::buttonShader().setVec4("properties.color", glm::vec4(0)); //Invisible
    ShaderSystem::buttonShader().setVec3("properties.outline.color", glm::vec4(0)); //Invisible
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    //Top
    ShaderSystem::buttonShader().setVec3("pos", glm::vec3(resultPos.x, resultPos.y - resultScale.y - 2000,   1.f));
    ShaderSystem::buttonShader().setVec2("scale", glm::vec2(2000));
    glDrawArrays(GL_TRIANGLES, 0, 6);

    if(isLibraryDisplayer){
        //Left
        ShaderSystem::buttonShader().setVec3("pos", glm::vec3(resultPos.x - resultScale.x - 2000, resultPos.y,   1.f));
        ShaderSystem::buttonShader().setVec2("scale", glm::vec2(2000));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        //Right
        ShaderSystem::buttonShader().setVec3("pos", glm::vec3(resultPos.x + resultScale.x + 2000, resultPos.y,   1.f));
        ShaderSystem::buttonShader().setVec2("scale", glm::vec2(2000));
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

static float calculateElementStartingPosition(bool vertical, std::vector<Section> sections, glm::vec3 pos, glm::vec2 scale){
    float elementPos = 0.f;
    
    //Get the starting position
    if(vertical){//If the panel is vertical
        if(sections.size()){//If there are sections
            if((sections[0].header.button.text.size() && sections[0].header.state == 0) || sections[0].header.state == 5) //If there is a header in the section
                elementPos = pos.y-scale.y + sections[0].header.scale.y + sections[0].header.panelOffset; //Take the starting position by including the height of the section header
            else if(sections[0].elements.size()) //If there are buttons in the first section
                elementPos = pos.y-scale.y + sections[0].elements[0].scale.y + sections[0].elements[0].panelOffset; //Take the starting position by including the height of the first button of the section
        }
    }
    else{//If the panel is horizontal
        if(sections.size()){//If there are sections
            if((sections[0].header.button.text.size() && sections[0].header.state == 0) || sections[0].header.state == 5)  //If there is a header in the section
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
        
        barButtons[i].pos.z += 0.035f;
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

    float outlineRadius = 10.f;

    if(this->solidStyle)
        outlineRadius = 1.f;

    if(preRenderingMode)
        glDisable(GL_BLEND);
    //Draw the panel's itself
    drawThePanel(resultPos, resultScale, this->color, this->color2, this->outlineThickness, outlineRadius);
    if(preRenderingMode)
        glEnable(GL_BLEND);


    /*Render the barriers if the depth texture of the framebuffer will be refreshed at the end of the panel rendering process*/
    if(clearDepthBuffer)
    {
        drawTheBarriers(resultPos, resultScale, this->isLibraryDisplayer);
    }

    //Starting pos
    float elementStartPos = calculateElementStartingPosition(this->vertical, this->sections, this->pos + this->additionalPos, this->scale);
    float elementPos = elementStartPos;
    if(sections.size() && vertical){
        if((sections[0].header.button.text.size() && sections[0].header.state == 0) || sections[0].header.state == 5)
            elementStartPos -= sections[0].header.scale.y;
        else if(sections[0].elements.size()){
            elementStartPos -= sections[0].elements[0].scale.y;
        }
    }

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
        if((sections[sI].header.button.text.size() && sections[sI].header.state == 0) || sections[sI].header.state == 5) //If there is a header
        { 
            //Section header button
            this->renderTheHeader(sI, elementPos, btnCounter, timer);
        }

        /*-- ELEMENTS --*/
        if(
                sections[sI].header.button.clickState1 == true || //If the header button is pressed 
                sections[sI].header.sectionHolder.active == true || //If the header button is pressed 
                sections[sI].header.state == -1 ||
                (sections[sI].header.state == 0 && !sections[sI].header.button.text.size())
            ) 
        {
            if(sections[sI].header.state == 5){
                float elementsLength = 2.f;
                for (size_t i = 0; i < sections[sI].elements.size(); i++)
                {
                    elementsLength += sections[sI].elements[i].scale.y * 2.f +  sections[sI].elements[i].panelOffset;
                }
                
                Button btn = Button(ELEMENT_STYLE_SOLID, glm::vec2(4,2), "", Texture(), 0.f, true);
                btn.color = ColorPalette::secondColor;
                btn.color.a /= 2.;
                btn.outline = false;
                btn.outlineExtra = false;
                
                btn.scale.x = sections[sI].header.scale.x;
                btn.scale.y = elementsLength / 2.f;
                btn.pos = sections[sI].header.pos;
                btn.pos.y += sections[sI].header.scale.y + btn.scale.y;

                btn.render(timer, false);
            }

            /*Render each element of the section*/
            for (int i = 0; i < sections[sI].elements.size(); i++) //
            {
                //Prepare the transform data of the button    
                if(sections[sI].elements[i].scale.x && sections[sI].elements[i].scale.y){
                    if(vertical)
                        prepDrawBtnVertically(sections[sI].elements[i],sections[sI].elements[std::max(i-1,0)],elementPos,btnCounter);
                    else
                        prepDrawBtnHorizontally(sections[sI].elements[i],sections[sI].elements[std::max(i-1,0)],elementPos,btnCounter);

                }
                if(isLibraryDisplayer){
                    sections[sI].elements[i].button.textureStickToTop = true;
                    sections[sI].elements[i].button.textureSizeScale = 1.5f;
                }
                
                lastElementScale = sections[sI].elements[i].scale.y; 
                
                //Don't render the unshown elements
                if(this->sections[sI].elements[i].pos.y - this->sections[sI].elements[i].scale.y < (this->pos.y + this->additionalPos.y + this->scale.y) && this->sections[sI].elements[i].pos.y + this->sections[sI].elements[i].scale.y > (this->pos.y + this->additionalPos.y - this->scale.y)){
                    
                    if(sections[sI].elements[i].scale.x && sections[sI].elements[i].scale.y)
                        sections[sI].elements[i].render(timer,doMouseTracking && !sliderButton.hover);
                    
                    /* Render the text if rendering library displayer panel*/
                    if(isLibraryDisplayer){
                        for (size_t meshI = 0; meshI < getModel()->meshes.size(); meshI++)
                        {
                            std::string channelName = "";
                            unsigned int txtrID = sections[sI].elements[i].button.texture.ID;

                            if(getModel()->meshes[meshI].albedo.ID == txtrID)
                                channelName = "Albedo";
                            if(getModel()->meshes[meshI].roughness.ID == txtrID)
                                channelName = "Roughness";
                            if(getModel()->meshes[meshI].metallic.ID == txtrID)
                                channelName = "Metallic";
                            if(getModel()->meshes[meshI].normalMap.ID == txtrID)
                                channelName = "Normal Map";
                            if(getModel()->meshes[meshI].heightMap.ID == txtrID)
                                channelName = "Height Map";
                            if(getModel()->meshes[meshI].ambientOcclusion.ID == txtrID)
                                channelName = "Ambient Occlusion";
                        
                            if(channelName != ""){
                                Button materialInfo = Button(ELEMENT_STYLE_SOLID, glm::vec2(sections[sI].elements[i].button.scale.x, sections[sI].elements[i].button.hoverMixVal + 0.05), getModel()->meshes[meshI].materialName, Texture(), 0.f, false);
                                Button channelInfo = Button(ELEMENT_STYLE_SOLID, glm::vec2(sections[sI].elements[i].button.scale.x, sections[sI].elements[i].button.hoverMixVal + 0.05), channelName, Texture(), 0.f, false);

                                materialInfo.color = ColorPalette::themeColor;
                                channelInfo.color = ColorPalette::themeColor;
                                
                                materialInfo.outlineExtra = false;
                                materialInfo.outline = false;

                                channelInfo.outlineExtra = false;
                                channelInfo.outline = false;

                                if(materialInfo.scale.y < 0.5f){
                                    materialInfo.text = "";
                                    channelInfo.text = "";
                                }

                                materialInfo.pos = sections[sI].elements[i].button.pos;
                                materialInfo.pos.y -= sections[sI].elements[i].button.scale.y - materialInfo.scale.y;
                                materialInfo.pos.z += 0.1f;
                                channelInfo.pos = materialInfo.pos;
                                channelInfo.pos.y += channelInfo.scale.y + materialInfo.scale.y;
                            
                                materialInfo.render(timer, false);
                                channelInfo.render(timer, false);
                            }
                        }
                        

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
        
        float elementHeight = (elementPos - elementStartPos + lastElementScale)/2.f;
        
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