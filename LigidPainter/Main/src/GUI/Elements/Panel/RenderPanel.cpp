/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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

#include "GUI/Elements.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

bool updateThePreRenderedPanels = false;
    
void Panel::render(Timer &timer, bool doMouseTracking){
    
    Debugger::block("GUI : Panel : Panel rendering start"); // Start
    Debugger::block("GUI : Panel : Panel rendering start"); // End

    Debugger::block("GUI : Panel : Prep"); // Start
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
    Debugger::block("GUI : Panel : Prep"); // End
    
    Debugger::block("GUI : Panel : Rendering start : " + std::to_string(preRenderingMode)); // Start
    Debugger::block("GUI : Panel : Rendering start : " + std::to_string(preRenderingMode)); // End

    Debugger::block("GUI : Panel : Rendering : " + std::to_string(preRenderingMode)); // Start
    if(this->preRenderingMode){
        this->updateUpdateGraphicsFlag();

        const double effectDuration = 2.;

        if(this->updateGraphicsFlag || LigidGL::getTime() - this->updateGraphicsTime < effectDuration){
            Debugger::block("GUI : Panel : Updating graphics start"); // Start
            Debugger::block("GUI : Panel : Updating graphics start"); // End
            
            Debugger::block("GUI : Panel : Updating graphics"); // Start
            if(this->updateGraphicsFlag)
                this->updateGraphicsTime = LigidGL::getTime();

            this->updateGraphics(timer);
            this->updateGraphicsFlag = false;
            Debugger::block("GUI : Panel : Updating graphics"); // End
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
        ShaderSystem::textureRenderingShader().setFloat("depthToleranceValue", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, graphics.ID);

        LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Panel : Pre-rendering mode : Rendering result texture");

        ShaderSystem::buttonShader().use();    
        
        if(this->clearDepthBuffer)
            glClear(GL_DEPTH_BUFFER_BIT);
    }
    else{
        drawPanel(resultPos, resultScale, timer);
    }
    Debugger::block("GUI : Panel : Rendering : " + std::to_string(preRenderingMode)); // End
    
    if(this->elementSelectionMode && this->sections.size()){
        for (size_t i = 0; i < this->sections[0].elements.size(); i++)
        {
            if(this->sections[0].elements[i].button.clickState1){
                if(this->selectedElement != i){
                    this->sections[0].elements[this->selectedElement].button.clickState1 = false;
                    this->selectedElement = i;
                    break;
                }
            }

            if(this->selectedElement == i){
                this->sections[0].elements[i].button.clickState1 = true;
            }
        }
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
                            updateThePreRenderedPanels ||
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

    this->graphics.update((char*)nullptr, displayRes.x, displayRes.y, GL_NEAREST);

    Framebuffer captureGraphicsFBO = Framebuffer(this->graphics, GL_TEXTURE_2D, "Panel update graphics");
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
    captureGraphicsFBO.deleteBuffers(false, false);
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
    
    LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Panel : Draw the panel");
}

static void drawTheBarriers(glm::vec3 resultPos, glm::vec2 resultScale){
    ShaderSystem::color2d().use();

    //Bottom
    ShaderSystem::color2d().setMat4("projection", getContext()->ortho_projection);
    ShaderSystem::color2d().setVec3("pos", glm::vec3(resultPos.x,   resultPos.y + resultScale.y + resultScale.y * 2.f,   1.f)); 
    ShaderSystem::color2d().setVec2("scale", glm::vec2(resultScale.y * 2.f));
    ShaderSystem::color2d().setVec4("color" , glm::vec4(0.f)); 
    LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Panel : Barrier bottom");
    
    //Top
    ShaderSystem::color2d().setVec3("pos", glm::vec3(resultPos.x, resultPos.y - resultScale.y - resultScale.y * 2.f,   1.f));
    ShaderSystem::color2d().setVec2("scale", glm::vec2(resultScale.y * 2.f));
    LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Panel : Barrier top");

    if(false){
        //Left
        ShaderSystem::color2d().setVec3("pos", glm::vec3(resultPos.x - resultScale.x - resultScale.x * 2.f, resultPos.y,   1.f));
        ShaderSystem::color2d().setVec2("scale", glm::vec2(resultScale.x * 2.f));
        LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Panel : Barrier left");
        
        //Right
        ShaderSystem::color2d().setVec3("pos", glm::vec3(resultPos.x + resultScale.x + resultScale.x * 2.f, resultPos.y,   1.f));
        ShaderSystem::color2d().setVec2("scale", glm::vec2(resultScale.x * 2.f));
        LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Panel : Barrier right");
    }
    ShaderSystem::buttonShader().use();
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
    drawTheBarriers(resultPos, resultScale);

    //Starting pos
    float elementStartPos = calculateElementStartingPosition(this->vertical, this->sections, this->pos + this->additionalPos, this->scale);
    float elementPos = elementStartPos;
    if(sections.size() && vertical){
        if((sections[0].header.button.text.size() && sections[0].header.state == 0) || sections[0].header.state == 5)
            elementStartPos -= sections[0].header.scale.y;
        else if(sections[0].elements.size()){
            elementStartPos -= sections[0].elements[0].scale.y / 2.f;
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
                    elementsLength += sections[sI].elements[i].scale.y * 2.f + sections[sI].elements[i].panelOffset;
                }
                
                Button btn = Button(ELEMENT_STYLE_SOLID, glm::vec2(4,2), "", Texture(), 0.f, true);
                btn.color = sections[sI].header.sectionHolder.containerColor;
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
                    
                    Element prevElement;
                    if(i > 0)
                        prevElement = sections[sI].elements[i-1];
                    else
                        prevElement = sections[sI].header;

                    if(vertical)
                        prepDrawBtnVertically(sections[sI].elements[i], prevElement, elementPos,btnCounter);
                    else
                        prepDrawBtnHorizontally(sections[sI].elements[i], prevElement, elementPos,btnCounter);

                }
                
                lastElementScale = sections[sI].elements[i].scale.y; 
                
                //Don't render the unshown elements
                if(this->sections[sI].elements[i].pos.y - this->sections[sI].elements[i].scale.y < (this->pos.y + this->additionalPos.y + this->scale.y) && this->sections[sI].elements[i].pos.y + this->sections[sI].elements[i].scale.y > (this->pos.y + this->additionalPos.y - this->scale.y)){
                    if(sections[sI].elements[i].scale.x && sections[sI].elements[i].scale.y && !this->dontRenderElements)
                        sections[sI].elements[i].render(timer,doMouseTracking && !sliderButton.hover);
                    
                }
                btnCounter++; //Indexing buttons to position them
            }
        }
    }
    endRenderingTheButtons:

    /* Render the slider of the panel*/
    if(hasSlider && this->doMouseTracking){
        
        sliderButton.pos = pos + this->additionalPos;
        
        if(leftSide.locked)
            sliderButton.pos.x = pos.x + this->additionalPos.x - sliderButton.scale.x + scale.x;
        else
            sliderButton.pos.x = pos.x + this->additionalPos.x + sliderButton.scale.x - scale.x;
        
        float elementHeight = 0.f;
        
        if(barButtons.size())
            elementHeight += barButtons[0].scale.y + barButtons[0].panelOffset / 2.f;

        for (size_t sectionI = 0; sectionI < sections.size(); sectionI++)
        {
            for (size_t elementI = 0; elementI < sections[sectionI].elements.size(); elementI++)
            {
                if(elementI % rowCount == 0){
                    if(sections[sectionI].header.button.clickState1 || sections[sectionI].header.sectionHolder.active || sections[sectionI].header.state == -1 || (sections[sectionI].header.state == 0 && !sections[sectionI].header.button.text.size()))
                        elementHeight += sections[sectionI].elements[elementI].scale.y + sections[sectionI].elements[elementI].panelOffset / 2.f;
                }
            }

            if(sections[sectionI].header.state != -1){
                if((sections[sectionI].header.state == 0 && sections[sectionI].header.button.text.size()) || sections[sectionI].header.state != 0)
                    elementHeight += sections[sectionI].header.scale.y + sections[sectionI].header.panelOffset / 2.f;
            }
        }
        
        slideRatio = scale.y / elementHeight;

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