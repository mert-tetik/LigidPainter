/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    In button
    Call the show dialog function - giving this->texture as a parameter
    If the texture is not procedural and the selected texture is procedural delete the received texture
    If the texture ID is not valid and the selected texture is not procedural create the texture (if the texture is procedural and the selected texture is not procedural (then update the procedural properties))
    If the texture is procedural and the selected texture is procedural just adjust the procedural properties
    If pressed to the select button (texture selected) return true (receivedTexture got modified)
    If not return false (receivedTexture is the same)
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"
#include "LibrarySystem/Library.hpp"

#include <string>
#include <iostream>
#include <vector>

#define MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE 29
#define MAX_PROCEDURAL_NOISE_TEXTURE_SIZE 37

TextureSelectionDialog::TextureSelectionDialog(){}

TextureSelectionDialog::TextureSelectionDialog(ColorPalette colorPalette,AppTextures appTextures){
    this->colorPalette = colorPalette;
    
    this->bgPanel = Panel(colorPalette, {}, scale, pos, colorPalette.secondColor, colorPalette.thirdColor, true, true, false, true, true, 1.f, 15.f, {}, 20.f, true);
    this->textureSelectingPanel = Panel(
                                                    colorPalette,
                                                    {
                                                        /*
                                                        Section(
                                                            Button(),
                                                            {
                                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2, scale.y / 2.f),colorPalette,"", Texture(), 0.f, true),
                                                            }
                                                        )
                                                        */
                                                    },
                                                    glm::vec2(scale.x / 6.f, scale.y / 2.f),
                                                    glm::vec3(pos.x, pos.y + scale.y - scale.y / 2.f, pos.z),
                                                    colorPalette.mainColor,
                                                    colorPalette.thirdColor,
                                                    true,
                                                    true,
                                                    false,
                                                    true,
                                                    true,
                                                    1.f,
                                                    12.f,
                                                    {},
                                                    20.f,
                                                    true
                                                );
    
    this->selectedTextureDisplayingPanel = Panel(
                                                    
                                                    colorPalette,
                                                    {
                                                        Section(
                                                            Button(),
                                                            {
                                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2, scale.y / 2.f),colorPalette,"", Texture(), 0.f, true),
                                                            }
                                                        )
                                                    },
                                                    glm::vec2(scale.x / 6.f, scale.y / 2.f),
                                                    glm::vec3(pos.x, pos.y - scale.y + scale.y / 2.f, pos.z),
                                                    colorPalette.mainColor,
                                                    colorPalette.thirdColor,
                                                    true,
                                                    true,
                                                    false,
                                                    true,
                                                    true,
                                                    1.f,
                                                    1.f,
                                                    {},
                                                    20.f,
                                                    true
                                                );
    this->subPanel = Panel(
                                
                                colorPalette,
                                {
                                    Section(
                                        Button(),
                                        {
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),colorPalette,"Library Textures", Texture(), 5.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),colorPalette,"Procedural Pattern Textures", Texture(), 2.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),colorPalette,"Procedural Noise Textures", Texture(), 2.f, true),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),colorPalette,"Invert", 40.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),colorPalette,"Scale", Texture(), 2.f, 0.f, 100.f, 10.f, appTextures),
                                            Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,2.f),colorPalette,"Select", Texture(), 2.f, false),
                                        }
                                    )
                                },
                                glm::vec2(scale.x / 6.f, scale.y),
                                glm::vec3(pos.x - scale.x + (scale.x / 6.f) , pos.y, pos.z),
                                colorPalette.mainColor,
                                colorPalette.thirdColor,
                                true,
                                true,
                                false,
                                true,
                                true,
                                1.f,
                                1.f,
                                {},
                                20.f,
                                true
                            );

    for (size_t i = 0; i < 3; i++)
    {
        this->subPanel.sections[0].elements[i].button.color = glm::vec4(0);
    }
}

Mouse __mouse_pos;
Mouse __mouse_button;

TextRenderer __textRenderer;

void __getMousePosDataToTheTextureSelection(Mouse& mousePos){
    __mouse_pos = mousePos;
}

void __getMouseButtonDataToTheTextureSelection(Mouse& mouseBtn){
    __mouse_button = mouseBtn;
}

void __getTextRendererDataToTheTextureSelection(TextRenderer& textRenderer){
    __textRenderer = textRenderer;
}

void TextureSelectionDialog::show(glm::vec2 videoScale, Timer &timer, glm::mat4 guiProjection, Texture& receivedTexture, LigidWindow& window, TextRenderer& textRenderer){
    
    this->dialogControl.activate();
        
    __textRenderer = textRenderer;

    // Get the viewport size
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    int viewportWidth = viewport[2];
    int viewportHeight = viewport[3];

    glm::ivec2 windowSize = glm::ivec2(viewportWidth, viewportHeight);

    GLfloat* pixels = new GLfloat[windowSize.x * windowSize.y * 4];
    glReadPixels(0, 0, windowSize.x, windowSize.y, GL_RGBA, GL_FLOAT, pixels);

    unsigned int bgTexture;
    glGenTextures(1, &bgTexture);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bgTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, windowSize.x, windowSize.y, 0, GL_RGBA, GL_FLOAT, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    while (!window.shouldClose())
    {
        window.pollEvents();
        
        Mouse __mouse = __mouse_button;
        __mouse.cursorPos = __mouse_pos.cursorPos;
        __mouse.mouseOffset = __mouse_pos.mouseOffset;

        if(__mouse.action == 0){
            __mouse.LClick = false;
            __mouse.RClick = false;
            __mouse.MClick = false;
        }


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bgTexture);

        ShaderSystem::buttonShader().setVec3("pos", glm::vec3(windowSize / glm::ivec2(2), 0.1));
        ShaderSystem::buttonShader().setVec2("scale", windowSize / glm::ivec2(2));
        ShaderSystem::buttonShader().setFloat("properties.colorMixVal", 0.f);
        ShaderSystem::buttonShader().setInt("states.renderTexture",     1    );
        ShaderSystem::buttonShader().setInt("properties.txtr",     0    );

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindTexture(GL_TEXTURE_2D, 0);

        ShaderSystem::buttonShader().setInt("states.renderTexture"  ,     0    );

        dialogControl.updateStart();


        //Update the panel elements
        this->textureSelectingPanel.sections.clear();
        std::vector<Element> sectionElements;
        if(this->selectedTextureMode == 0){
            for (size_t i = 0; i < Library::getTextureArraySize(); i++)
            {
                sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),colorPalette,"texture"       , *Library::getTexture(i), 0.f,false)));
            }
        }
        else if(this->selectedTextureMode == 1){
            for (size_t i = 0; i < MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE; i++)
            {
                sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,3.f),colorPalette,""       , Texture(), 0.f,false)));
            }
        }
        else if(this->selectedTextureMode == 2){
            for (size_t i = 0; i < MAX_PROCEDURAL_NOISE_TEXTURE_SIZE; i++)
            {
                sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,3.f),colorPalette,""       , Texture(), 0.f,false)));
            }
        }

        //Push the section elements to the texture panel
        this->textureSelectingPanel.sections.push_back(
            Section
                    (
                        Element(Button()),
                        sectionElements
                    )
        );

        for (size_t i = 0; i < 3; i++)
        {
            if(this->subPanel.sections[0].elements[i].button.clickState1 && this->selectedTextureMode != i){
                this->selectedTextureMode = i;
                this->selectedTextureIndex = 0;
                for (size_t i = 0; i < this->subPanel.sections[0].elements.size(); i++)
                    this->subPanel.sections[0].elements[i].button.clickState1 = false;
            }

            if(this->selectedTextureMode == i)
                this->subPanel.sections[0].elements[i].button.clickState1 = true;
            else
                this->subPanel.sections[0].elements[i].button.clickState1 = false;
        }

        //Render the panel
        this->bgPanel.render(videoScale, __mouse,timer,__textRenderer,true);

        //Render the texture mode selection panel
        this->subPanel.render(videoScale, __mouse,timer,__textRenderer,true);

        selectedTextureDisplayingPanel.scale.x = this->scale.x - subPanel.scale.x;
        selectedTextureDisplayingPanel.pos.x = this->pos.x + subPanel.scale.x;

        textureSelectingPanel.pos.x = selectedTextureDisplayingPanel.pos.x;
        textureSelectingPanel.scale.x = selectedTextureDisplayingPanel.scale.x;

        if(Library::getTextureArraySize() && selectedTextureMode == 0 && selectedTextureIndex < Library::getTextureArraySize())
            selectedTextureDisplayingPanel.sections[0].elements[0].button.texture = *Library::getTexture(selectedTextureIndex);

        this->textureSelectingPanel.render(videoScale,__mouse,timer,__textRenderer,true);

        if(this->selectedTextureMode == 0){

            ShaderSystem::buttonShader().setInt("properties.invertTheTexture", this->subPanel.sections[0].elements[3].checkBox.clickState1);
            ShaderSystem::buttonShader().setVec2("properties.txtrScale", glm::vec2(this->subPanel.sections[0].elements[4].rangeBar.value / 10.f));
            this->selectedTextureDisplayingPanel.render(videoScale,__mouse,timer,__textRenderer,true);
            ShaderSystem::buttonShader().setInt("properties.invertTheTexture", false);
            ShaderSystem::buttonShader().setVec2("properties.txtrScale", glm::vec2(1.f));

        }
        else{
            ShaderSystem::proceduralDisplayerShader().use();
            ShaderSystem::proceduralDisplayerShader().setMat4("projection"  ,       guiProjection);
            ShaderSystem::proceduralDisplayerShader().setVec3("pos"         ,       selectedTextureDisplayingPanel.resultPos);
            ShaderSystem::proceduralDisplayerShader().setVec2("scale"       ,       glm::vec2(std::min(selectedTextureDisplayingPanel.resultScale.x,selectedTextureDisplayingPanel.resultScale.y)));
            if(this->selectedTextureMode == 1)
                ShaderSystem::proceduralDisplayerShader().setInt("proceduralID", selectedTextureIndex);
            else
                ShaderSystem::proceduralDisplayerShader().setInt("proceduralID", selectedTextureIndex + MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE);
            ShaderSystem::proceduralDisplayerShader().setFloat("proceduralScale", this->subPanel.sections[0].elements[4].rangeBar.value / 10.f);
            ShaderSystem::proceduralDisplayerShader().setInt("proceduralInverted", this->subPanel.sections[0].elements[3].checkBox.clickState1);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            for (size_t i = 0; i < this->textureSelectingPanel.sections[0].elements.size(); i++)
            {
                ShaderSystem::proceduralDisplayerShader().use();
                ShaderSystem::proceduralDisplayerShader().setVec3("pos"         ,       this->textureSelectingPanel.sections[0].elements[i].button.resultPos);
                ShaderSystem::proceduralDisplayerShader().setVec2("scale"       ,       glm::vec2(std::min(this->textureSelectingPanel.sections[0].elements[i].button.resultScale.x, this->textureSelectingPanel.sections[0].elements[i].button.resultScale.y)));
                
                if(this->selectedTextureMode == 1)
                    ShaderSystem::proceduralDisplayerShader().setInt("proceduralID", i);
                else
                    ShaderSystem::proceduralDisplayerShader().setInt("proceduralID", i + MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE);

                ShaderSystem::proceduralDisplayerShader().setFloat("proceduralScale", 1.f);
                ShaderSystem::proceduralDisplayerShader().setInt("proceduralInverted", 0);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }

            ShaderSystem::buttonShader().use();
        }

        //If pressed any of the texture select the texture
        for (size_t i = 0; i < this->textureSelectingPanel.sections[0].elements.size(); i++)
        {
            if(this->textureSelectingPanel.sections[0].elements[i].button.hover && __mouse.LClick){
                selectedTextureIndex = i;
            }
        }

        // Pressed to the select button
        if(this->subPanel.sections[0].elements[5].button.clicked){
            

            // If selected from the library
            if(this->selectedTextureMode == 0){
                receivedTexture.proceduralID = -1;

                if(Library::getTextureArraySize() <= this->selectedTextureIndex){
                    std::cout << "ERROR : Invalid selected texture index! No texture selected" << std::endl;
                }
                else{
                    if(receivedTexture.ID == 0 || glIsTexture(receivedTexture.ID) == GL_FALSE){
                        glGenTextures(1, &receivedTexture.ID);
                    }

                    const int txtrRes = std::max(Library::getTexture(this->selectedTextureIndex)->getResolution().x, Library::getTexture(this->selectedTextureIndex)->getResolution().y);
                    
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, receivedTexture.ID);
                    
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, txtrRes, txtrRes, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                    glGenerateMipmap(GL_TEXTURE_2D);

                    /* Capturing FBO */
                    unsigned int FBO; 
                    glGenFramebuffers(1,&FBO);
                    glBindFramebuffer(GL_FRAMEBUFFER,FBO);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, receivedTexture.ID, 0);
                    glViewport(0, 0, txtrRes, txtrRes);

                    glClearColor(0,0,0,0);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                    glm::mat4 projection = glm::ortho(
                                                        0.f, 
                                                        (float)txtrRes, 
                                                        (float)txtrRes, 
                                                        0.f
                                                    );
                                                    
                                                    
                    ShaderSystem::buttonShader().setMat4("projection"  ,       projection);
                    ShaderSystem::buttonShader().setVec3("pos"         ,       glm::vec3((float)txtrRes / 2.f, (float)txtrRes / 2.f, 0.9f));
                    ShaderSystem::buttonShader().setVec2("scale"       ,       glm::vec2((float)txtrRes / 2.f, (float)txtrRes / 2.f));
                    ShaderSystem::buttonShader().setInt("states.renderTexture",     1    );

                    ShaderSystem::buttonShader().setInt("properties.invertTheTexture", this->subPanel.sections[0].elements[3].checkBox.clickState1);
                    ShaderSystem::buttonShader().setVec2("properties.txtrScale", glm::vec2(this->subPanel.sections[0].elements[4].rangeBar.value / 10.f));

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, Library::getTexture(this->selectedTextureIndex)->ID);

                    glDrawArrays(GL_TRIANGLES, 0, 6);                

                    ShaderSystem::buttonShader().setInt("properties.invertTheTexture", false);
                    ShaderSystem::buttonShader().setVec2("properties.txtrScale", glm::vec2(1.f));

                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                    glDeleteFramebuffers(1, &FBO);
                }


            }

            // If procedural pattern selected 
            else if(this->selectedTextureMode == 1 || this->selectedTextureMode == 2){
                //Update the selected procedural function index
                if(this->selectedTextureMode == 1)
                    receivedTexture.proceduralID = this->selectedTextureIndex;
                else
                    receivedTexture.proceduralID = this->selectedTextureIndex + MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE;
                receivedTexture.proceduralScale = this->subPanel.sections[0].elements[4].rangeBar.value / 10.f;
                receivedTexture.proceduralnverted = this->subPanel.sections[0].elements[3].checkBox.clickState1;

                //TODO Set the scale & invert 
                // The texture is not generated
                if(receivedTexture.ID == 0 || glIsTexture(receivedTexture.ID) == GL_FALSE){
                    glGenTextures(1, &receivedTexture.ID);
                }

                const int displayRes = 512;

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, receivedTexture.ID);
               
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
                
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, displayRes, displayRes, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                glGenerateMipmap(GL_TEXTURE_2D);

                /* Capturing FBO */
                unsigned int FBO; 
                glGenFramebuffers(1,&FBO);
                glBindFramebuffer(GL_FRAMEBUFFER,FBO);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, receivedTexture.ID, 0);
                glViewport(0, 0, displayRes, displayRes);

                glClearColor(0,0,0,0);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                /* Displaying texture */
                ShaderSystem::proceduralDisplayerShader().use();
                glm::mat4 projection = glm::ortho(0.f, (float)displayRes, (float)displayRes, 0.f);
                ShaderSystem::proceduralDisplayerShader().setMat4("projection"  ,       projection);
                ShaderSystem::proceduralDisplayerShader().setVec3("pos"         ,       glm::vec3((float)displayRes / 2.f, (float)displayRes / 2.f,0.9f));
                ShaderSystem::proceduralDisplayerShader().setVec2("scale"       ,       glm::vec2((float)displayRes / 2.f));
                
                if(this->selectedTextureMode == 1)
                    ShaderSystem::proceduralDisplayerShader().setInt("proceduralID", selectedTextureIndex);
                else
                    ShaderSystem::proceduralDisplayerShader().setInt("proceduralID", selectedTextureIndex + MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE);                
                
                ShaderSystem::proceduralDisplayerShader().setFloat("proceduralScale", this->subPanel.sections[0].elements[4].rangeBar.value / 10.f);
                ShaderSystem::proceduralDisplayerShader().setInt("proceduralInverted", this->subPanel.sections[0].elements[3].checkBox.clickState1);
                
                glDrawArrays(GL_TRIANGLES, 0, 6);

                ShaderSystem::buttonShader().use();

                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glDeleteFramebuffers(1, &FBO);

            }

            receivedTexture.getMaterialIDPalette();
                
            receivedTexture.title = "SelectedTexture";

            dialogControl.unActivate();
        }

        //End the dialog
        if((window.isKeyPressed(LIGIDGL_KEY_ESCAPE)) || (!this->bgPanel.hover && __mouse.LClick)){
            dialogControl.unActivate();
            selectedTextureIndex = 0;
        }

        if(!this->dialogControl.isActive())
            break;
            
        dialogControl.updateEnd(timer,0.15f);

        window.swapBuffers();

        //Set mouse states to default
        __mouse_button.LClick = false;
        __mouse_button.RClick = false;
        __mouse_button.MClick = false;
        __mouse_button.LDoubleClick = false;
        __mouse_pos.mouseOffset = glm::vec2(0);
        __mouse_button.mods = 0;
        __mouse_button.mouseScroll = 0;
        __mouse_button.action = 0;

        //Set keyboard states to default
        __textRenderer.keyInput = false;
        __textRenderer.mods = 0;
    }

}