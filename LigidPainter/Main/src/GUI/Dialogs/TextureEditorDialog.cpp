
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
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

// Defined in the RenderPanel.cpp
extern bool updateThePreRenderedPanels;

TextureEditorDialog::TextureEditorDialog(){

    this->displayingTexture = Texture(nullptr, 512, 512, GL_LINEAR);

    //Create the panel
    this->bgPanel = Panel(
        {
            {
                Section(
                    Element(Button()),
                    {
                    }
                )
            }
        },
        glm::vec2(34.f, 35.f),
        glm::vec3(50.f,50.f,0.8f),
        ColorPalette::mainColor,
        ColorPalette::thirdColor,
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
        
        {
            {
                Section(
                    Element(Button()),
                    {
                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), "Resize", Settings::appTextures().resizingIcon, 1.f,true)),
                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), "Blur", Settings::appTextures().blurIcon, 1.f,true)),
                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), "Coloring", Settings::appTextures().colorGearIcon, 1.f,true)),
                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), "Normal Map", Settings::appTextures().normalMapIcon, 1.f,true)),
                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), "Distortion", Settings::appTextures().distortionIcon, 1.f,true)),
                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), "Filters", Settings::appTextures().filterIcon, 1.f,true)),
                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), "Images", Settings::appTextures().solidPaintingDisplayingMode, 1.f,true)),
                    }
                )
            }
        },
        glm::vec2(6.f, 34.f),
        glm::vec3(50.f,50.f,0.8f),
        ColorPalette::secondColor,
        ColorPalette::thirdColor,
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
        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), {"Stretch", "Alpha Zero", "Color", "Repeat", "Mirror"}, "Wraping Function", 0.f),
        Button(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Wraping Color", Texture(), 1.f, false),
        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), {"Top Left", "Top Right", "Bottom Left", "Bottom Right", "Center", "Custom"}, "Origin Point", 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Origin Point X", Texture(), 0.f, 0.f, 1.f, 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Origin Point Y", Texture(), 0.f, 0.f, 1.f, 0.f),
        TextBox(ELEMENT_STYLE_STYLIZED, glm::vec2(8.f, 2.f), "1024", 0.f, false),
        TextBox(ELEMENT_STYLE_STYLIZED, glm::vec2(8.f, 2.f), "1024", 0.f, false)
    };
    this->resizeElements[1].button.colorSelection = true;
    
    this->bluringElement = {
        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), {"Box", "Gaussian", "Directional", "Radial", "Lens"}, "Bluring Function", 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Directional Direction", Texture(), 0.f, 0.f, 360.f, 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Radial Pos Y", Texture(), 0.f, 0.f, 1.f, 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Radial Pos Y", Texture(), 0.f, 0.f, 1.f, 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Strength", Texture(), 0.f, 0.f, 1.f, 0.f),
    };
    
    this->coloringElements = {
        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), {"HSV", "Single Color", "Brightness"}, "Coloring Function", 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Hue",      Texture(), 0.f, 0.f, 1.f, 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Saturation",      Texture(), 0.f, 0.f, 1.f, 1.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Value",      Texture(), 0.f, 0.f, 1.f, 1.f),
        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), {"Multiply", "Hue"}, "Coloring Function", 0.f),
        Button(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Color", Texture(), 1.f, false),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Brightness",      Texture(), 0.f, 0.f, 2.f, 1.f),
    };
    this->coloringElements[5].button.colorSelection = true;
    this->coloringElements[5].button.color = glm::vec4(0.f, 0.23f, 1.f, 1.f);

    this->normalMapElements = {
        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), {"Height Map To Normal Map", "Normal Map To Height Map"}, "Mode", 0.f),
        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), {"Blue Scale", "Gray Scale"}, "Normal Map Function", 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Strength",  Texture(), 0.f, 0.f, 2.0f, 1.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Blur",  Texture(), 0.f, 0.f, 1.f, 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Rotation",  Texture(), 0.f, 0.f, 360.f, 0.f),
    };

    this->distortionElements = {
        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), {"Ripple", "Sin Wave", "Swirl", "Rain", "Texture"}, "Distortion Type", 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Ripple Roughness",  Texture(), 0.f, 0.f, 1.f, 0.1f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Ripple Octaves",  Texture(), 0.f, 0, 12, 8),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Sin X Strength",  Texture(), 0.f, 0.f, 1.f, 0.1f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Sin Y Strength",  Texture(), 0.f, 0.f, 40.f, 20.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Swirl Radius",  Texture(), 0.f, 0.f, 1.f, 0.5f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Swirl Angle",  Texture(), 0.f, 0.f, 10.f, 4.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Rain Scale",  Texture(), 0.f, 0.f, 50.f, 40.f),
        Button(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Texture", Texture(), 1.f, false),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Strength",  Texture(), 0.f, 0.f, 1.f, 0.2f),
    };

    this->imagesElements = {
        Button(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Image", Texture(), 1.f, true),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Strength",  Texture(), 0.f, 0.f, 1.f, 0.f),
        RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(8.f, 2.f), "Scale",  Texture(), 0.f, 0.f, 10.f, 1.f),
    };

    this->filterBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(8.f), "Pick A Filter", Texture(), 1.f, false);
    this->filterBtn.filterSelection = true;
    this->textureBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(8.f), "Pick A Texture", Texture(), 1.f, false);
    this->textureBtn.textureSelection2D = true;
    this->displayerBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(13.f), "", this->displayingTexture, 1.f,true);

    char whitePx[] = {127, 127, 127, 127};

    this->maskTextureButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), "Mask Texture", Texture(whitePx, 1, 1, GL_NEAREST), 1.f, false);
    this->maskTextureButton.texture.proceduralProps.proceduralID = 24; //Solid white
    this->maskTextureButton.texture.proceduralProps.proceduralnverted = 0;
    this->maskTextureButton.texture.proceduralProps.proceduralScale = 1.f;

    this->saveButton = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(8.f, 2.f), "Save", Texture(), 1.f, false);
    this->saveAsButton = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(8.f, 2.f), "Save As", Texture(), 1.f, false);

    this->maskTextureButton.textureSelection2D = true;
    distortionElements[8].button.textureSelection2D = true;
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

        glm::vec2 originPoint = glm::vec2(resizeElements[3].rangeBar.value, resizeElements[4].rangeBar.value);

        ShaderSystem::txtrEditorResizeShader().use();
        ShaderSystem::txtrEditorResizeShader().setMat4("projection", projection);
        if(resizeElements[0].comboBox.selectedIndex == 1 || resizeElements[0].comboBox.selectedIndex == 2)
            ShaderSystem::txtrEditorResizeShader().setVec3("pos", glm::vec3(pos.x + (originPoint.x * displayRes.x), pos.y + (originPoint.y * displayRes.y), pos.z));
        else
            ShaderSystem::txtrEditorResizeShader().setVec3("pos", pos);
        
        ShaderSystem::txtrEditorResizeShader().setVec2("scale", scale);
    
        if(resizeElements[0].comboBox.selectedIndex == 0)
            txtrRes = displayRes;

        ShaderSystem::txtrEditorResizeShader().setInt("txtr", 0);
        ShaderSystem::txtrEditorResizeShader().setVec2("txtrResolution", txtrRes);
        ShaderSystem::txtrEditorResizeShader().setVec2("destTxtrResolution", displayRes);
        
        ShaderSystem::txtrEditorResizeShader().setInt("wrapingIndex", resizeElements[0].comboBox.selectedIndex);
        ShaderSystem::txtrEditorResizeShader().setVec3("wrapingColor", resizeElements[1].button.color);
        ShaderSystem::txtrEditorResizeShader().setVec2("originPoint", originPoint);

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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

        glDrawArrays(GL_TRIANGLES, 0, 6);

    }
    else if(this->selectedSection == 2){
        ShaderSystem::txtrEditorColoringShader().use();
        ShaderSystem::txtrEditorColoringShader().setMat4("projection", projection);
        ShaderSystem::txtrEditorColoringShader().setVec3("pos", pos);
        ShaderSystem::txtrEditorColoringShader().setVec2("scale", scale);
    
        ShaderSystem::txtrEditorColoringShader().setInt("txtr", 0);
        ShaderSystem::txtrEditorColoringShader().setVec2("txtrResolution", displayRes);

        ShaderSystem::txtrEditorColoringShader().setInt("coloringIndex", coloringElements[0].comboBox.selectedIndex);
        ShaderSystem::txtrEditorColoringShader().setVec3("hsv", glm::vec3(coloringElements[1].rangeBar.value, coloringElements[2].rangeBar.value, coloringElements[3].rangeBar.value));
        ShaderSystem::txtrEditorColoringShader().setInt("singleColorIndex", coloringElements[4].comboBox.selectedIndex);
        ShaderSystem::txtrEditorColoringShader().setVec3("singleColor", coloringElements[5].button.color);
        ShaderSystem::txtrEditorColoringShader().setFloat("brightness", coloringElements[6].rangeBar.value);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, receivedTexture.ID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

        glDrawArrays(GL_TRIANGLES, 0, 6);

    }
    else if(this->selectedSection == 3){
        ShaderSystem::txtrEditorNormalMapShader().use();
        ShaderSystem::txtrEditorNormalMapShader().setMat4("projection", projection);
        ShaderSystem::txtrEditorNormalMapShader().setVec3("pos", pos);
        ShaderSystem::txtrEditorNormalMapShader().setVec2("scale", scale);
    
        ShaderSystem::txtrEditorNormalMapShader().setInt("txtr", 0);
        ShaderSystem::txtrEditorNormalMapShader().setVec2("txtrResolution", displayRes);
        ShaderSystem::txtrEditorNormalMapShader().setInt("mode", normalMapElements[0].comboBox.selectedIndex);
        ShaderSystem::txtrEditorNormalMapShader().setInt("grayScale", normalMapElements[1].comboBox.selectedIndex);
        ShaderSystem::txtrEditorNormalMapShader().setFloat("strength", normalMapElements[2].rangeBar.value);
        ShaderSystem::txtrEditorNormalMapShader().setFloat("blurVal", normalMapElements[3].rangeBar.value);
        ShaderSystem::txtrEditorNormalMapShader().setFloat("rot", normalMapElements[4].rangeBar.value);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, receivedTexture.ID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

        glDrawArrays(GL_TRIANGLES, 0, 6);

    }
    else if(this->selectedSection == 5){
        if(this->filterBtn.filter.shader.ID){
            this->filterBtn.filter.shader.use();
            this->filterBtn.filter.shader.setMat4("projection", projection);
            this->filterBtn.filter.shader.setVec3("pos", pos);
            this->filterBtn.filter.shader.setVec2("scale", scale);
        
            this->filterBtn.filter.shader.setFloat("strength", this->filterBtn.filter.strength);
            this->filterBtn.filter.shader.setInt("txtr", 0);
            this->filterBtn.filter.shader.setVec2("txtrResolution", displayRes);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, receivedTexture.ID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }

    ShaderSystem::grayScaleIDMaskingShader().use();
    ShaderSystem::grayScaleIDMaskingShader().setMat4("projection", projection);
    ShaderSystem::grayScaleIDMaskingShader().setVec3("pos", pos);
    ShaderSystem::grayScaleIDMaskingShader().setVec2("scale", scale);
    ShaderSystem::grayScaleIDMaskingShader().setInt("maskTexture", 0);
    ShaderSystem::grayScaleIDMaskingShader().setInt("texture_black", 1);
    ShaderSystem::grayScaleIDMaskingShader().setInt("texture_white", 2);
    ShaderSystem::grayScaleIDMaskingShader().setInt("maskAlpha", 0);
    ShaderSystem::grayScaleIDMaskingShader().setFloat("offset", 0.5f);
    
    Texture destTxtrObj = destTxtr;
    Texture copiedDestTxtr = destTxtrObj.duplicateTexture();
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->maskTextureButton.texture.ID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, receivedTexture.ID);
    glActiveTexture(GL_TEXTURE2);
    if(this->selectedSection == 6){
        glBindTexture(GL_TEXTURE_2D, this->textureBtn.texture.ID);
    }
    else
        glBindTexture(GL_TEXTURE_2D, copiedDestTxtr.ID);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    //Finish
    Settings::defaultFramebuffer()->FBO.bind();
    ShaderSystem::buttonShader().use();
    glDeleteFramebuffers(1, &captureFBO);
    glDeleteTextures(1, &copiedDestTxtr.ID);
    Settings::defaultFramebuffer()->setViewport();
}

void TextureEditorDialog::render(Timer timer, Skybox &skybox, glm::mat4 projection, Texture receivedTexture, LogDialog& logDialog){
    
    dialogControl.updateStart();

    this->sectionPanel.pos = this->bgPanel.pos;
    this->sectionPanel.pos.z += 0.001f;
    this->sectionPanel.pos.x -= this->bgPanel.scale.x - this->sectionPanel.scale.x;

    //Render the panel
    this->bgPanel.render(timer, true);
    this->sectionPanel.render(timer, true);
    
    if(dialogControl.firstFrameActivated){
        resizeElements[5].textBox.text = std::to_string(receivedTexture.getResolution().x); 
        resizeElements[5].textBox.activeChar = resizeElements[5].textBox.text.size()-1; 
        resizeElements[5].textBox.activeChar2 = resizeElements[5].textBox.text.size()-1;
         
        resizeElements[6].textBox.text = std::to_string(receivedTexture.getResolution().y);
        resizeElements[6].textBox.activeChar = resizeElements[6].textBox.text.size()-1; 
        resizeElements[6].textBox.activeChar2 = resizeElements[6].textBox.text.size()-1;
        this->updateDisplayingTexture(receivedTexture, this->displayingTexture.ID);
    }

    for (size_t i = 0; i < this->sectionPanel.sections[0].elements.size(); i++)
    {
        if(this->sectionPanel.sections[0].elements[i].button.clickState1 && this->selectedSection != i){
            this->selectedSection = i;
            this->updateDisplayingTexture(receivedTexture, this->displayingTexture.ID);
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
    
    this->displayerBtn.render(timer, false);

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

            if(resizeElements[i].isInteracted())
                anyInteraction = true;
            
            resizeElements[i].pos = displayerBtn.pos;
            resizeElements[i].pos.x += displayerBtn.scale.x * 2.f;
            resizeElements[i].pos.y -= displayerBtn.scale.y * .5f;
            resizeElements[i].pos.y += 8.f * eCnt;
            resizeElements[i].render(timer,true);

            
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
            bluringElement[i].render(timer,true);
            eCnt++;

            if(bluringElement[i].isInteracted())
                anyInteraction = true;
        }

    }

    if(this->selectedSection == 2){
        int eCnt = 0;
        
        for (size_t i = 0; i < coloringElements.size(); i++)
        {
            coloringElementsLoopReturn:

            if((i == 1 || i == 2 || i == 3) && coloringElements[0].comboBox.selectedIndex != 0){
                i++;
                goto coloringElementsLoopReturn;
            }
            if((i == 4 || i == 5) && coloringElements[0].comboBox.selectedIndex != 1){
                i++;
                goto coloringElementsLoopReturn;
            }
            
            if(i == 6 && coloringElements[0].comboBox.selectedIndex != 2)
                break;

            coloringElements[i].pos = displayerBtn.pos;
            coloringElements[i].pos.x += displayerBtn.scale.x * 2.f;
            coloringElements[i].pos.y -= displayerBtn.scale.y * .5f;
            coloringElements[i].pos.y += 8.f * eCnt;
            coloringElements[i].render(timer,true);
            eCnt++;
            
            if(coloringElements[i].isInteracted())
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
            normalMapElements[i].render(timer,true);
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
            distortionElements[i].render(timer,true);
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
        filterBtn.render(timer,true);

        if(filterBtn.clicked){
            anyInteraction = true;
        }        
    }
    
    if(this->selectedSection == 6){

        textureBtn.pos = displayerBtn.pos;
        textureBtn.pos.x += displayerBtn.scale.x * 2.f;
        textureBtn.pos.y -= displayerBtn.scale.y * .5f;
        textureBtn.pos.y += 8.f;
        textureBtn.render(timer,true);

        if(textureBtn.clicked){
            anyInteraction = true;
        }        
    }

    this->maskTextureButton.pos = displayerBtn.pos;
    this->maskTextureButton.pos.y += displayerBtn.scale.y * 1.15;
    this->maskTextureButton.render(timer, true);
    
    this->saveButton.pos = displayerBtn.pos;
    this->saveButton.pos.y += displayerBtn.scale.y * 1.2 + maskTextureButton.scale.y * 3.f;
    this->saveButton.render(timer, true);
    
    this->saveAsButton.pos = saveButton.pos;
    this->saveAsButton.pos.y += saveButton.scale.y * 3.f;
    this->saveAsButton.render(timer, true);

    if(this->maskTextureButton.clicked)
       anyInteraction = true; 

    if(anyInteraction)
        this->updateDisplayingTexture(receivedTexture, this->displayingTexture.ID);

    if(this->saveButton.clicked){
        registerImageEditorAction("Texture manipulated via texture editor dialog", Texture(), receivedTexture);

        Texture txtr = receivedTexture.duplicateTexture();
        this->updateDisplayingTexture(txtr, receivedTexture.ID);
        
        for (size_t i = 0; i < Library::getTextureArraySize(); i++)
        {
            if(receivedTexture.ID == Library::getTexture(i)->ID){
                Library::getTexture(i)->copyDataToTheCopyContext();
                projectUpdatingThreadElements.updateTextures = true;
            }
        }

        glDeleteTextures(1, &txtr.ID);
    
        if(!getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL))
            this->dialogControl.unActivate();
     
        updateThePreRenderedPanels = true;
    }

    if(this->saveAsButton.clicked){
        Texture newTxtr;
        newTxtr = Texture(nullptr, receivedTexture.getResolution().x, receivedTexture.getResolution().y);
        this->updateDisplayingTexture(receivedTexture, newTxtr.ID);
        Library::addTexture(newTxtr, "New texture via image editor");
        
        if(!getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL))
            this->dialogControl.unActivate();

        updateThePreRenderedPanels = true;
    }

    //End the dialog
    if(getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || (!bgPanel.hover && !logDialog.isHovered() && *Mouse::LClick())){
        if(!dialogControl.firstFrameActivated){
            dialogControl.unActivate();
        }
    }

    dialogControl.updateEnd(timer,0.15f);
}