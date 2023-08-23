
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 Copyright (c) 2022-2023, Mert Tetik

 All rights reserved.

 Official GitHub Link : https:github.com/mert-tetik/LigidPainter
 Official Web Page : https:ligidtools.com/ligidpainter

 ---------------------------------------------------------------------------

    Display settings dialog.
    (Mostly manipulates the skybox )

 */
#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "MouseSystem/Mouse.hpp"
#include "SettingsSystem/Settings.hpp"
#include "LibrarySystem/Library.hpp"

#include <string>
#include <iostream>
#include <vector>

#include "tinyfiledialogs.h"

TextureEditorDialog::TextureEditorDialog(){}

TextureEditorDialog::TextureEditorDialog(
                                            ColorPalette colorPalette //LigidPainter's theme
                                        ){
    
    //Init displaying texture
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1,&this->displayingTexture);
    glBindTexture(GL_TEXTURE_2D,this->displayingTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);

    //Create the panel
    this->bgPanel = Panel(
        colorPalette,
        {
            {
                Section(
                    Element(Button()),
                    {
                    }
                )
            }
        },
        glm::vec2(34.f, 34.f),
        glm::vec3(50.f,50.f,0.8f),
        colorPalette.mainColor,
        colorPalette.thirdColor,
        true,
        true,
        true,
        true,
        true,
        1.f,
        1.f,
        {},
        0.25f,
        false
    );

    this->sectionPanel = Panel(
        colorPalette,
        {
            {
                Section(
                    Element(Button()),
                    {
                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), colorPalette, "Resize", Settings::appTextures().brushIcon, 1.f,true)),
                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), colorPalette, "Blur", Settings::appTextures().brushIcon, 1.f,true)),
                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), colorPalette, "Noise", Settings::appTextures().brushIcon, 1.f,true)),
                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), colorPalette, "Normal Map", Settings::appTextures().brushIcon, 1.f,true)),
                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), colorPalette, "Distortion", Settings::appTextures().brushIcon, 1.f,true)),
                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), colorPalette, "Filters", Settings::appTextures().brushIcon, 1.f,true)),
                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), colorPalette, "Images", Settings::appTextures().brushIcon, 1.f,true)),
                    }
                )
            }
        },
        glm::vec2(6.f, 34.f),
        glm::vec3(50.f,50.f,0.8f),
        colorPalette.secondColor,
        colorPalette.thirdColor,
        true,
        true,
        true,
        true,
        true,
        1.f,
        1.f,
        {},
        0.25f,
        false
    );

    this->resizeElements = {
        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), colorPalette, {"Stretch", "Alpha Zero", "Color", "Repeat", "Mirror"}, "Wraping Function", 0.f),
        Button(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Wraping Color", Texture(), 1.f, false),
        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), colorPalette, {"Top Left", "Top Right", "Bottom Left", "Bottom Right", "Center", "Custom"}, "Origin Point", 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Origin Point X", Texture(), 0.f, 0.f, 1.f, 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Origin Point Y", Texture(), 0.f, 0.f, 1.f, 0.f),
        TextBox(0, glm::vec2(8.f, 2.f), colorPalette, "1024", 0.f, false),
        TextBox(0, glm::vec2(8.f, 2.f), colorPalette, "1024", 0.f, false)
    };
    this->resizeElements[1].button.colorSelection = true;
    
    this->bluringElement = {
        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), colorPalette, {"Box", "Gaussian", "Directional", "Radial", "Lens"}, "Bluring Function", 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Directional Direction", Texture(), 0.f, 0.f, 360.f, 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Radial Pos Y", Texture(), 0.f, 0.f, 1.f, 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Radial Pos Y", Texture(), 0.f, 0.f, 1.f, 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Strength", Texture(), 0.f, 0.f, 1.f, 0.f),
    };
    
    this->noiseElements = {
        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), colorPalette, {"FBM", "Cloud", "Golden"}, "Noise Function", 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Cloud Radius",  Texture(), 0.f, 0.f, 1.f, 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Frequency",     Texture(), 0.f, 0.f, 1.f, 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Octaves",       Texture(), 0.f, 0.f, 1.f, 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Roughness",     Texture(), 0.f, 0.f, 1.f, 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Scale",         Texture(), 0.f, 0.f, 1.f, 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Strength",      Texture(), 0.f, 0.f, 1.f, 0.f),
        CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), colorPalette, "Color Mix", 0.f),
        Button(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Mixing Color", Texture(), 1.f, true),
    };

    this->normalMapElements = {
        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), colorPalette, {"Blue Scale", "Gray Scale"}, "Normal Map Function", 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Strength",  Texture(), 0.f, 0.f, 1.0f, 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Blur",  Texture(), 0.f, 0.f, 1.f, 0.f),
    };

    this->distortionElements = {
        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), colorPalette, {"Ripple", "Sin Wave", "Swirl", "Rain", "Texture"}, "Distortion Type", 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Ripple Roughness",  Texture(), 0.f, 0.f, 1.f, 0.1f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Ripple Octaves",  Texture(), 0.f, 0, 12, 8),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Sin X Strength",  Texture(), 0.f, 0.f, 1.f, 0.1f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Sin Y Strength",  Texture(), 0.f, 0.f, 40.f, 20.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Swirl Radius",  Texture(), 0.f, 0.f, 1.f, 0.5f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Swirl Angle",  Texture(), 0.f, 0.f, 10.f, 4.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Rain Scale",  Texture(), 0.f, 0.f, 50.f, 40.f),
        Button(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Texture", Texture(), 1.f, false),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Strength",  Texture(), 0.f, 0.f, 1.f, 0.2f),
    };

    this->imagesElements = {
        Button(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Image", Texture(), 1.f, true),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Strength",  Texture(), 0.f, 0.f, 1.f, 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), colorPalette, "Scale",  Texture(), 0.f, 0.f, 10.f, 1.f),
    };

    this->filterBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(8.f), colorPalette, "Filter", Texture(), 1.f, false);
    this->displayerBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(13.f), colorPalette, "", this->displayingTexture, 1.f,true);
    
    this->saveButton = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(8.f, 2.f), colorPalette, "Save", Texture(), 1.f, false);
    this->saveAsButton = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(8.f, 2.f), colorPalette, "Save As", Texture(), 1.f, false);

    distortionElements[8].button.textureSelection = true;
}

void TextureEditorDialog::updateDisplayingTexture(Texture& receivedTexture, unsigned int destTxtr){
    //Displaying resolution
    glm::vec2 txtrRes = receivedTexture.getResolution();
    glm::vec2 displayRes = receivedTexture.getResolution();
    if(this->selectedSection == 0 && resizeElements[5].textBox.text != "" && resizeElements[6].textBox.text != "")
        displayRes = glm::vec2(std::stoi(resizeElements[5].textBox.text), std::stoi(resizeElements[6].textBox.text));

    glm::mat4 projection = glm::ortho(0.f, displayRes.x, displayRes.y, 0.f);
    glm::vec3 pos = glm::vec3(displayRes.x / 2.f, displayRes.y / 2.f, 0.9f);
    glm::vec2 scale = glm::vec2(displayRes.x / 2.f, displayRes.y / 2.f);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, destTxtr);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, displayRes.x, displayRes.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);

    //Create the framebuffer
    unsigned int captureFBO;
    glGenFramebuffers(1,&captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER,captureFBO);
    
    //Bind the displaying texture to the capture framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, destTxtr, 0);

    //Clear the capture frame buffer(displaying texture) with color alpha zero
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, displayRes.x, displayRes.y);
    
    if(this->selectedSection == 0){
        ShaderSystem::txtrEditorResizeShader().use();
        ShaderSystem::txtrEditorResizeShader().setMat4("projection", projection);
        ShaderSystem::txtrEditorResizeShader().setVec3("pos", pos);
        ShaderSystem::txtrEditorResizeShader().setVec2("scale", scale);
    
        if(resizeElements[0].comboBox.selectedIndex == 0)
            txtrRes = displayRes;

        ShaderSystem::txtrEditorResizeShader().setInt("txtr", 0);
        ShaderSystem::txtrEditorResizeShader().setVec2("txtrResolution", txtrRes);
        ShaderSystem::txtrEditorResizeShader().setVec2("destTxtrResolution", displayRes);
        
        ShaderSystem::txtrEditorResizeShader().setInt("wrapingIndex", resizeElements[0].comboBox.selectedIndex);
        ShaderSystem::txtrEditorResizeShader().setVec3("wrapingColor", resizeElements[1].button.color);
        ShaderSystem::txtrEditorResizeShader().setVec2("originPoint", glm::vec2(resizeElements[3].rangeBar.value, resizeElements[4].rangeBar.value));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, receivedTexture.ID);

        unsigned int wrapParam = GL_REPEAT;

        if(resizeElements[0].comboBox.selectedIndex == 1){
            wrapParam = GL_CLAMP_TO_BORDER;
            GLfloat borderColor[] = { 0.f, 0.f, 0.f, 0.f };  // Replace r, g, b, a with the desired color values
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        }
        if(resizeElements[0].comboBox.selectedIndex == 2){
            wrapParam = GL_CLAMP_TO_BORDER;
            GLfloat borderColor[] = { resizeElements[1].button.color.r, resizeElements[1].button.color.g, resizeElements[1].button.color.b, 1. };  // Replace r, g, b, a with the desired color values
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        }
        if(resizeElements[0].comboBox.selectedIndex == 3)
            wrapParam = GL_REPEAT;
        else if(resizeElements[0].comboBox.selectedIndex == 4)
            wrapParam = GL_MIRRORED_REPEAT;
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapParam);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapParam);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapParam);

        glDrawArrays(GL_TRIANGLES, 0, 6);

    }
    else if(this->selectedSection == 1){
        ShaderSystem::txtrEditorBlurShader().use();
        ShaderSystem::txtrEditorBlurShader().setMat4("projection", projection);
        ShaderSystem::txtrEditorBlurShader().setVec3("pos", pos);
        ShaderSystem::txtrEditorBlurShader().setVec2("scale", scale);
    
        ShaderSystem::txtrEditorBlurShader().setInt("txtr", 0);
        ShaderSystem::txtrEditorBlurShader().setVec2("txtrResolution", displayRes);
        ShaderSystem::txtrEditorBlurShader().setInt("blurIndex", bluringElement[0].comboBox.selectedIndex);
        ShaderSystem::txtrEditorBlurShader().setFloat("directionalDirection", bluringElement[1].rangeBar.value);
        ShaderSystem::txtrEditorBlurShader().setVec2("radialPos", glm::vec2(bluringElement[2].rangeBar.value, bluringElement[3].rangeBar.value));
        ShaderSystem::txtrEditorBlurShader().setFloat("strength", bluringElement[4].rangeBar.value);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, receivedTexture.ID);

        glDrawArrays(GL_TRIANGLES, 0, 6);

    }
    else if(this->selectedSection == 3){
        ShaderSystem::txtrEditorNormalMapShader().use();
        ShaderSystem::txtrEditorNormalMapShader().setMat4("projection", projection);
        ShaderSystem::txtrEditorNormalMapShader().setVec3("pos", pos);
        ShaderSystem::txtrEditorNormalMapShader().setVec2("scale", scale);
    
        ShaderSystem::txtrEditorNormalMapShader().setInt("txtr", 0);
        ShaderSystem::txtrEditorNormalMapShader().setVec2("txtrResolution", displayRes);
        ShaderSystem::txtrEditorNormalMapShader().setInt("grayScale", normalMapElements[0].comboBox.selectedIndex);
        ShaderSystem::txtrEditorNormalMapShader().setFloat("strength", normalMapElements[1].rangeBar.value);
        ShaderSystem::txtrEditorNormalMapShader().setFloat("blurVal", normalMapElements[2].rangeBar.value);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, receivedTexture.ID);

        glDrawArrays(GL_TRIANGLES, 0, 6);

    }
    else if(this->selectedSection == 4){
        ShaderSystem::txtrEditorDistortionShader().use();
        ShaderSystem::txtrEditorDistortionShader().setMat4("projection", projection);
        ShaderSystem::txtrEditorDistortionShader().setVec3("pos", pos);
        ShaderSystem::txtrEditorDistortionShader().setVec2("scale", scale);
    
        ShaderSystem::txtrEditorDistortionShader().setInt("txtr", 0);
        ShaderSystem::txtrEditorDistortionShader().setInt("distortionTxtr", 1);
        ShaderSystem::txtrEditorDistortionShader().setVec2("txtrResolution", displayRes);
        ShaderSystem::txtrEditorDistortionShader().setInt("distortionIndex", distortionElements[0].comboBox.selectedIndex);
        ShaderSystem::txtrEditorDistortionShader().setFloat("fbmRoughness", distortionElements[1].rangeBar.value);
        ShaderSystem::txtrEditorDistortionShader().setInt("fbmOctaves", distortionElements[2].rangeBar.value);
        ShaderSystem::txtrEditorDistortionShader().setFloat("xStrength", distortionElements[3].rangeBar.value);
        ShaderSystem::txtrEditorDistortionShader().setFloat("yStrength", distortionElements[4].rangeBar.value);
        ShaderSystem::txtrEditorDistortionShader().setFloat("effectRadius", distortionElements[5].rangeBar.value);
        ShaderSystem::txtrEditorDistortionShader().setFloat("effectAngle", distortionElements[6].rangeBar.value);
        ShaderSystem::txtrEditorDistortionShader().setFloat("rainScale", distortionElements[7].rangeBar.value);
        ShaderSystem::txtrEditorDistortionShader().setFloat("strength", distortionElements[9].rangeBar.value);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, receivedTexture.ID);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, distortionElements[8].button.texture.ID);

        glDrawArrays(GL_TRIANGLES, 0, 6);

    }
    else if(this->selectedSection == 5){
        if(this->filter.shader.ID){
            this->filter.shader.use();
            this->filter.shader.setMat4("projection", projection);
            this->filter.shader.setVec3("pos", pos);
            this->filter.shader.setVec2("scale", scale);
        
            this->filter.shader.setInt("txtr", 0);
            this->filter.shader.setVec2("txtrResolution", displayRes);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, receivedTexture.ID);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }

    //Finish
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ShaderSystem::buttonShader().use();
    glDeleteFramebuffers(1, &captureFBO);
    glViewport(0, 0, getContext()->windowScale.x, getContext()->windowScale.y);

}

void TextureEditorDialog::render(ColorPalette colorPalette, Timer timer, TextRenderer &textRenderer, Skybox &skybox, glm::mat4 projection, Texture receivedTexture){
    
    dialogControl.updateStart();

    this->sectionPanel.pos = this->bgPanel.pos;
    this->sectionPanel.pos.z += 0.001f;
    this->sectionPanel.pos.x -= this->bgPanel.scale.x - this->sectionPanel.scale.x;

    //Render the panel
    this->bgPanel.render(timer, textRenderer, true);
    this->sectionPanel.render(timer, textRenderer, true);
    
    if(dialogControl.firstFrameActivated){
        resizeElements[5].textBox.text = std::to_string(receivedTexture.getResolution().x); 
        resizeElements[5].textBox.activeChar = resizeElements[5].textBox.text.size()-1; 
        resizeElements[5].textBox.activeChar2 = resizeElements[5].textBox.text.size()-1;
         
        resizeElements[6].textBox.text = std::to_string(receivedTexture.getResolution().y);
        resizeElements[6].textBox.activeChar = resizeElements[6].textBox.text.size()-1; 
        resizeElements[6].textBox.activeChar2 = resizeElements[6].textBox.text.size()-1;
        this->updateDisplayingTexture(receivedTexture, this->displayingTexture);
    }

    for (size_t i = 0; i < this->sectionPanel.sections[0].elements.size(); i++)
    {
        if(this->sectionPanel.sections[0].elements[i].button.clickState1 && this->selectedSection != i){
            this->selectedSection = i;
            this->updateDisplayingTexture(receivedTexture, this->displayingTexture);
            break;
        }
    }
    
    for (size_t i = 0; i < this->sectionPanel.sections[0].elements.size(); i++){
        if(this->selectedSection != i){
            this->sectionPanel.sections[0].elements[i].button.clickState1 = false;
        }
    }
    
    for (size_t i = 0; i < this->sectionPanel.sections[0].elements.size(); i++){
        if(this->selectedSection == i){
            this->sectionPanel.sections[0].elements[i].button.clickState1 = true;
        }
    }

    this->displayerBtn.scale.y = this->displayerBtn.scale.x;
    this->displayerBtn.scale.y *= Settings::videoScale()->x / Settings::videoScale()->y; 

    this->displayerBtn.pos = this->bgPanel.pos;
    this->displayerBtn.pos.x = this->sectionPanel.pos.x + this->sectionPanel.scale.x + this->displayerBtn.scale.x * 1.1f;
    this->displayerBtn.pos.y -= this->bgPanel.scale.y - this->displayerBtn.scale.y * 1.1f;
    this->displayerBtn.pos.z += 0.001f;
    
    this->displayerBtn.render(timer, textRenderer, true);

    bool anyInteraction = false;

    if(this->selectedSection == 0){
        int eCnt = 0;
        
        //Top Left
        if(resizeElements[2].comboBox.selectedIndex == 0){
            resizeElements[3].rangeBar.value = 0.f;
            resizeElements[4].rangeBar.value = 0.f;
        }

        //Top Right
        if(resizeElements[2].comboBox.selectedIndex == 1){
            resizeElements[3].rangeBar.value = 1.f;
            resizeElements[4].rangeBar.value = 0.f;
        }

        //Bottom Left
        if(resizeElements[2].comboBox.selectedIndex == 2){
            resizeElements[3].rangeBar.value = 0.f;
            resizeElements[4].rangeBar.value = 1.f;
        }

        //Bottom Right
        if(resizeElements[2].comboBox.selectedIndex == 3){
            resizeElements[3].rangeBar.value = 1.f;
            resizeElements[4].rangeBar.value = 1.f;
        }

        //Center
        if(resizeElements[2].comboBox.selectedIndex == 4){
            resizeElements[3].rangeBar.value = 0.5f;
            resizeElements[4].rangeBar.value = 0.5f;
        }

        for (size_t i = 0; i < resizeElements.size(); i++)
        {
            if(i == 1 && resizeElements[0].comboBox.selectedIndex != 2)
                i++;

            resizeElements[i].pos = displayerBtn.pos;
            resizeElements[i].pos.x += displayerBtn.scale.x * 2.f;
            resizeElements[i].pos.y -= displayerBtn.scale.y * .5f;
            resizeElements[i].pos.y += 8.f * eCnt;
            resizeElements[i].render(timer,textRenderer,true);

            if(resizeElements[i].isInteracted())
                anyInteraction = true;
            
            eCnt++;
        }
    }
    
    if(this->selectedSection == 1){
        int eCnt = 0;
        
        for (size_t i = 0; i < bluringElement.size(); i++)
        {
            blurinElementsLoopReturn:

            if(i == 1 && bluringElement[0].comboBox.selectedIndex != 2)
                i++;
            if((i == 2 || i == 3) && bluringElement[0].comboBox.selectedIndex != 3){
                i++;
                goto blurinElementsLoopReturn;
            }

            bluringElement[i].pos = displayerBtn.pos;
            bluringElement[i].pos.x += displayerBtn.scale.x * 2.f;
            bluringElement[i].pos.y -= displayerBtn.scale.y * .5f;
            bluringElement[i].pos.y += 8.f * eCnt;
            bluringElement[i].render(timer,textRenderer,true);
            eCnt++;

            if(bluringElement[i].isInteracted())
                anyInteraction = true;
        }

    }

    if(this->selectedSection == 2){
        int eCnt = 0;
        
        for (size_t i = 0; i < noiseElements.size(); i++)
        {
            noiseElementsLoopReturn:

            if(i == 1 && noiseElements[0].comboBox.selectedIndex != 1)
                i++;
            if((i == 2 || i == 3 || i == 4) && noiseElements[0].comboBox.selectedIndex != 0){
                i++;
                goto noiseElementsLoopReturn;
            }
            if(i == 8 && !noiseElements[7].checkBox.clickState1)
                i++;

            noiseElements[i].pos = displayerBtn.pos;
            noiseElements[i].pos.x += displayerBtn.scale.x * 2.f;
            noiseElements[i].pos.y -= displayerBtn.scale.y * .5f;
            noiseElements[i].pos.y += 8.f * eCnt;
            noiseElements[i].render(timer,textRenderer,true);
            eCnt++;
            
            if(noiseElements[i].isInteracted())
                anyInteraction = true;
        }
    }

    if(this->selectedSection == 3){
        int eCnt = 0;
        
        for (size_t i = 0; i < normalMapElements.size(); i++)
        {
            normalMapElements[i].pos = displayerBtn.pos;
            normalMapElements[i].pos.x += displayerBtn.scale.x * 2.f;
            normalMapElements[i].pos.y -= displayerBtn.scale.y * .5f;
            normalMapElements[i].pos.y += 8.f * eCnt;
            normalMapElements[i].render(timer,textRenderer,true);
            eCnt++;
            if(normalMapElements[i].isInteracted())
                anyInteraction = true;
        }
        
    }

    if(this->selectedSection == 4){
        int eCnt = 0;
        
        for (size_t i = 0; i < distortionElements.size(); i++)
        {
            distortionElementsLoopReturn:

            if((i == 1 || i == 2) && distortionElements[0].comboBox.selectedIndex != 0){
                i++;
                goto distortionElementsLoopReturn;
            }

            if((i == 3 || i == 4) && distortionElements[0].comboBox.selectedIndex != 1){
                i++;
                goto distortionElementsLoopReturn;
            }
            
            if((i == 5 || i == 6) && distortionElements[0].comboBox.selectedIndex != 2){
                i++;
                goto distortionElementsLoopReturn;
            }
            
            if(i == 7 && distortionElements[0].comboBox.selectedIndex != 3){
                i++;
            }
            
            if(i == 8 && distortionElements[0].comboBox.selectedIndex != 4){
                i++;
            }

            distortionElements[i].pos = displayerBtn.pos;
            distortionElements[i].pos.x += displayerBtn.scale.x * 2.f;
            distortionElements[i].pos.y -= displayerBtn.scale.y * .5f;
            distortionElements[i].pos.y += 8.f * eCnt;
            distortionElements[i].render(timer,textRenderer,true);
            eCnt++;
            
            if(distortionElements[i].isInteracted())
                anyInteraction = true;
        }

    }
    
    if(this->selectedSection == 5){

        filterBtn.pos = displayerBtn.pos;
        filterBtn.pos.x += displayerBtn.scale.x * 2.f;
        filterBtn.pos.y -= displayerBtn.scale.y * .5f;
        filterBtn.pos.y += 8.f;
        filterBtn.texture = filter.displayingTxtr;
        filterBtn.render(timer,textRenderer,true);

        if(filterBtn.clicked){
            Texture txtr;
            showTextureSelectionDialog(txtr, filter, 512, true);
            anyInteraction = true;
        }        

    }

    this->saveButton.pos = displayerBtn.pos;
    this->saveButton.pos.y += displayerBtn.scale.y * 1.2;
    this->saveButton.render(timer, textRenderer, true);
    this->saveAsButton.pos = displayerBtn.pos;
    this->saveAsButton.pos.y += displayerBtn.scale.y * 1.2 + saveButton.scale.y * 4.f;
    this->saveAsButton.render(timer, textRenderer, true);

    if(anyInteraction)
        this->updateDisplayingTexture(receivedTexture, this->displayingTexture);

    if(this->saveButton.clicked){
        Texture txtr = receivedTexture.duplicateTexture();
        this->updateDisplayingTexture(txtr, receivedTexture.ID);
        glDeleteTextures(1, &txtr.ID);
    }

    if(this->saveAsButton.clicked){
        Texture newTxtr;
        newTxtr = Texture(nullptr, receivedTexture.getResolution().x, receivedTexture.getResolution().y);
        this->updateDisplayingTexture(receivedTexture, newTxtr.ID);
        Library::addTexture(newTxtr);
    }

    //End the dialog
    if(getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || (!bgPanel.hover && *Mouse::LClick())){
        if(!dialogControl.firstFrameActivated){
            dialogControl.unActivate();
        }
    }

    dialogControl.updateEnd(timer,0.15f);
}