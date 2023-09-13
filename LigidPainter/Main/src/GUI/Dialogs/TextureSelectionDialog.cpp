/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Texture selection dialog is responsible of modifying the procedural variables inside of a Texture class & generating displaying textures

*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"
#include "LibrarySystem/Library.hpp"
#include "MouseSystem/Mouse.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

#define MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE 66
#define MAX_PROCEDURAL_NOISE_TEXTURE_SIZE 40
#define MAX_PROCEDURAL_SMART_TEXTURE_SIZE 7

#define subPanel_Library_Textures_INDEX 0
#define subPanel_Procedural_Pattern_Textures_INDEX 1
#define subPanel_Procedural_Noise_Textures_INDEX 2
#define subPanel_Texture_Packs_INDEX 3 
#define subPanel_Smart_Textures_INDEX 4
#define subPanel_Source_Textures 5
#define subPanel_Mesh_UV 6
#define subPanel_Gray_Scale_INDEX 7
#define subPanel_Brightness_INDEX 8 
#define subPanel_Invert_INDEX 9
#define subPanel_Scale_INDEX 10
#define subPanel_Normal_Map_INDEX 11
#define subPanel_Normal_Gray_Scale_INDEX 12
#define subPanel_Normal_Strength_INDEX 13
#define subPanel_Use_Texture_Coordinates_INDEX 14
#define subPanel_Select_INDEX 15

#define subPanelSmartTextures_Library_Textures_INDEX 0
#define subPanelSmartTextures_Procedural_Pattern_Textures_INDEX 1
#define subPanelSmartTextures_Procedural_Noise_Textures_INDEX 2
#define subPanelSmartTextures_Texture_Packs_INDEX 3
#define subPanelSmartTextures_Smart_Textures_INDEX 4
#define subPanelSmartTextures_Source_Textures 5
#define subPanelSmartTextures_Mesh_UV 6
#define subPanelSmartTextures_Normal_Map_INDEX 7
#define subPanelSmartTextures_Normal_Gray_Scale_INDEX 8
#define subPanelSmartTextures_Normal_Strength_INDEX 9
#define subPanelSmartTextures_Invert_INDEX 10
#define subPanelSmartTextures_Select_INDEX 11

#define subPanelTxtrPack_Library_Textures 0
#define subPanelTxtrPack_Procedural_Pattern_Textures 1
#define subPanelTxtrPack_Procedural_Noise_Textures 2
#define subPanelTxtrPack_Texture_Packs 3
#define subPanelTxtrPack_Smart_Textures 4
#define subPanelTxtrPack_Source_Textures 5
#define subPanelTxtrPack_Mesh_UV 6
#define subPanelTxtrPack_Gray_Scale 7
#define subPanelTxtrPack_Brightness 8
#define subPanelTxtrPack_Invert 9
#define subPanelTxtrPack_Scale 10
#define subPanelTxtrPack_Normal_Map 11
#define subPanelTxtrPack_Normal_Gray_Scale 12
#define subPanelTxtrPack_Normal_Strength 13
#define subPanelTxtrPack_Count 14
#define subPanelTxtrPack_Rotation_Jitter 15
#define subPanelTxtrPack_Size_Jitter 16
#define subPanelTxtrPack_Opacity_Jitter 17
#define subPanelTxtrPack_Scatter 18
#define subPanelTxtrPack_Use_Texture_Coordinates 19 
#define subPanelTxtrPack_Select 20


TextureSelectionDialog::TextureSelectionDialog(){

    this->bgPanel = Panel({}, scale, pos, ColorPalette::secondColor, ColorPalette::thirdColor, true, true, false, true, true, 1.f, 15.f, {}, 20.f, true);
    this->textureSelectingPanel = Panel(
                                                    
                                                    {
                                                        /*
                                                        Section(
                                                            Button(),
                                                            {
                                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2, scale.y / 2.f),"", Texture(), 0.f, true),
                                                            }
                                                        )
                                                        */
                                                    },
                                                    glm::vec2(scale.x / 6.f, scale.y / 2.f),
                                                    glm::vec3(pos.x, pos.y + scale.y - scale.y / 2.f, pos.z),
                                                    ColorPalette::mainColor,
                                                    ColorPalette::thirdColor,
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
                                                    
                                                    
                                                    {
                                                        Section(
                                                            Button(),
                                                            {
                                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2, scale.y / 2.f),"", Texture(), 0.f, true),
                                                            }
                                                        )
                                                    },
                                                    glm::vec2(scale.x / 6.f, scale.y / 2.f),
                                                    glm::vec3(pos.x, pos.y - scale.y + scale.y / 2.f, pos.z),
                                                    ColorPalette::mainColor,
                                                    ColorPalette::thirdColor,
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
                                
                                
                                {
                                    Section(
                                        Button(),
                                        {
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Library Textures", Texture(), 5.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Procedural Pattern Textures", Texture(), 2.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Procedural Noise Textures", Texture(), 2.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Texture Packs", Texture(), 2.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Smart Textures", Texture(), 2.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Source Textures", Texture(), 2.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Mesh UV Textures", Texture(), 2.f, true),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Gray Scale", 12.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Brightness", Texture(), 0.f, 0.f, 2.f, 1.f),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Invert", 0.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Scale", Texture(), 0.f, 0.f, 200.f, 10.f),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Normal Map", 0.f),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Normal Gray Scale", 0.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Normal Strength", Texture(), 0.f, 0.f, 100.f, 10.f),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Use Texture Coordinates", 0.f),
                                            Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,2.f),"Select", Texture(), 1.f, false)
                                        }
                                    )
                                },
                                glm::vec2(scale.x / 6.f, scale.y),
                                glm::vec3(pos.x - scale.x + (scale.x / 6.f) , pos.y, pos.z),
                                ColorPalette::mainColor,
                                ColorPalette::thirdColor,
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
    
    this->subPanelSmartTextures = Panel(
                                
                                
                                {
                                    Section(
                                        Button(),
                                        {
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Library Textures", Texture(), 5.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Procedural Pattern Textures", Texture(), 2.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Procedural Noise Textures", Texture(), 2.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Texture Packs", Texture(), 2.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Smart Textures", Texture(), 2.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Source Textures", Texture(), 2.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Mesh UV Textures", Texture(), 2.f, true),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Normal Map", 16.f),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Normal Gray Scale", 1.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Normal Strength", Texture(), 1.f, 0.f, 100.f, 10.f),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Invert", 2.f),
                                            Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,2.f),"Select", Texture(), 2.f, false)
                                        }
                                    )
                                },
                                glm::vec2(scale.x / 6.f, scale.y),
                                glm::vec3(pos.x - scale.x + (scale.x / 6.f) , pos.y, pos.z),
                                ColorPalette::mainColor,
                                ColorPalette::thirdColor,
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

    this->subPanelTxtrPack = Panel(
                                {
                                    Section(
                                        Button(),
                                        {
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Library Textures", Texture(), 5.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Procedural Pattern Textures", Texture(), 2.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Procedural Noise Textures", Texture(), 2.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Texture Packs", Texture(), 2.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Smart Textures", Texture(), 2.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Source Textures", Texture(), 2.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Mesh UV Textures", Texture(), 2.f, true),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,1.6f),"Gray Scale", 6.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Brightness", Texture(), 0.f, 0.f, 100.f, 10.f),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,1.6f),"Invert", 0.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Scale", Texture(), 0.f, 0.f, 200.f, 10.f),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,1.6f),"Normal Map", 0.f),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,1.6f),"Normal Gray Scale", 0.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Normal Strength", Texture(), 0.f, 0.f, 100.f, 10.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Count", Texture(), 0.f, 0.f, 0.5f, 0.05f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Rotation Jitter", Texture(), 0.f, 0.f, 1.f, 0.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Size Jitter", Texture(), 0.f, 0.f, 1.f, 0.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Opacity Jitter", Texture(), 0.f, 0.f, 1.f, 0.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Scatter", Texture(), 0.f, 0.f, 1.f, 1.f),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,1.6f),"Use Texture Coordinates", 2.f),
                                            Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,2.f),"Select", Texture(), 1.f, false)
                                        }
                                    )
                                },
                                glm::vec2(scale.x / 6.f, scale.y),
                                glm::vec3(pos.x - scale.x + (scale.x / 6.f) , pos.y, pos.z),
                                ColorPalette::mainColor,
                                ColorPalette::thirdColor,
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
  
    this->smartPositionTexturePanel = Panel(
                                {
                                    Section(
                                        Button(),
                                        {
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Offset", Texture(), 1.f, 0.f, 10.f, 1.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Yaw", Texture(), 1.f, 0.f, 360.f, 0.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Pitch", Texture(), 1.f, 0.f, 360.f, 0.0f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Position", Texture(), 1.f, -1.f, 1.f, 0.f)
                                        }
                                    )
                                },
                                glm::vec2(8.f, 7.f),
                                glm::vec3(glm::vec2(50.f, 80.f), pos.z),
                                ColorPalette::mainColor,
                                ColorPalette::thirdColor,
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

    this->smartStripesTexturePanel = Panel(
                                {
                                    Section(
                                        Button(),
                                        {
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Rows", Texture(), 1.f, 0.f, 10.f, 1.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Thickness", Texture(), 1.f, 0.f, 1.f, 0.25f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Blur", Texture(), 1.f, 0.f, 0.001, 0.0005f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Rotation", Texture(), 1.f, 0.f, 360.f, 0.f)
                                        }
                                    )
                                },
                                glm::vec2(8.f, 7.f),
                                glm::vec3(glm::vec2(50.f, 80.f), pos.z),
                                ColorPalette::mainColor,
                                ColorPalette::thirdColor,
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

    this->edgeWearTexturePanel = Panel(
                                {
                                    Section(
                                        Button(),
                                        {
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Radius", Texture(), 1.f, 0.f, 10.f, 0.15f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Multiply", Texture(), 1.f, 0.f, 10.f, 3.37f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Blur", Texture(), 1.f, 0.f, 10.f, 0.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Noise Strength", Texture(), 1.f, 0.f, 1.f, 1.f)
                                        }
                                    )
                                },
                                glm::vec2(8.f, 7.f),
                                glm::vec3(glm::vec2(50.f, 80.f), pos.z),
                                ColorPalette::mainColor,
                                ColorPalette::thirdColor,
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

    this->smartDistanceTexturePanel = Panel(
                                {
                                    Section(
                                        Button(),
                                        {
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Min", Texture(), 1.f, 0.f, 1.f, 0.944f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Max", Texture(), 1.f, 0.f, 1.f, 1.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Offset", Texture(), 1.f, 0.f, 20.f, 13.15f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Noise Strength", Texture(), 1.f, 0.f, 1.f, 0.f)
                                        }
                                    )
                                },
                                glm::vec2(8.f, 7.f),
                                glm::vec3(glm::vec2(50.f, 80.f), pos.z),
                                ColorPalette::mainColor,
                                ColorPalette::thirdColor,
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

    for (size_t i = 0; i < 7; i++)
    {
        this->subPanel.sections[0].elements[i].button.color = glm::vec4(0);
        this->subPanelTxtrPack.sections[0].elements[i].button.color = glm::vec4(0);
        this->subPanelSmartTextures.sections[0].elements[i].button.color = glm::vec4(0);
    }
}


void TextureSelectionDialog::generateDisplayingTexture(Texture& txtr, int displayingTextureRes){
    
    GLint viewport[4]; 
    glGetIntegerv(GL_VIEWPORT, viewport);

    glm::ivec2 viewportResolution = glm::ivec2(viewport[2], viewport[3]);

    if(this->selectedTextureMode == 0){
        txtr.proceduralID = -1;
        if(selectedTextureIndex < Library::getTextureArraySize())
            txtr.proceduralTextureID = Library::getTexture(selectedTextureIndex)->ID;
    }
    else if(this->selectedTextureMode == 1){
        txtr.proceduralID = this->selectedTextureIndex;
        txtr.proceduralTextureID = 0;
    }
    else if(this->selectedTextureMode == 2){
        txtr.proceduralID = this->selectedTextureIndex + MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE;
        txtr.proceduralTextureID = 0;
    }
    else if(this->selectedTextureMode == 3){
        txtr.proceduralID = 1000;
        txtr.proceduralTextureID = 0;
    }
    else if(this->selectedTextureMode == 4){
        txtr.proceduralID = this->selectedTextureIndex + MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE + MAX_PROCEDURAL_NOISE_TEXTURE_SIZE;
        txtr.proceduralTextureID = 0;
    }
    else if(this->selectedTextureMode == 6){
        txtr.proceduralID = -1;
        if(selectedTextureIndex < Library::getTextureArraySize())
            txtr.proceduralTextureID = getModel()->meshes[selectedTextureIndex].uvMask;
    }
    
    //TODO Set the scale & invert 
    // The texture is not generated
    if(txtr.ID == 0 || glIsTexture(txtr.ID) == GL_FALSE){
        glGenTextures(1, &txtr.ID);
    }

    const int displayRes = displayingTextureRes;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, txtr.ID);
    
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
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, txtr.ID, 0);
    glClearColor(0,0,0,0);

    unsigned int RBO;
	glGenRenderbuffers(1,&RBO);
	glBindRenderbuffer(GL_RENDERBUFFER,RBO);
	
    //Set the renderbuffer to store depth
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, displayRes, displayRes);
	
    //Give the renderbuffer to the framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glDepthFunc(GL_LEQUAL);

    if(this->selectedTextureMode != 3){
        txtr.proceduralScale = this->subPanel.sections[0].elements[subPanel_Scale_INDEX].rangeBar.value / 10.f;
        txtr.proceduralnverted = this->subPanel.sections[0].elements[subPanel_Invert_INDEX].checkBox.clickState1;
        txtr.proceduralNormalMap = this->subPanel.sections[0].elements[subPanel_Normal_Map_INDEX].checkBox.clickState1;
        txtr.proceduralNormalGrayScale = this->subPanel.sections[0].elements[subPanel_Normal_Gray_Scale_INDEX].checkBox.clickState1;
        txtr.proceduralNormalStrength = this->subPanel.sections[0].elements[subPanel_Normal_Strength_INDEX].rangeBar.value;
        txtr.proceduralUseTexCoords = this->subPanel.sections[0].elements[subPanel_Use_Texture_Coordinates_INDEX].checkBox.clickState1;
        txtr.proceduralGrayScale = this->subPanel.sections[0].elements[subPanel_Gray_Scale_INDEX].checkBox.clickState1;
        txtr.proceduralBrightness = this->subPanel.sections[0].elements[subPanel_Brightness_INDEX].rangeBar.value;
    }
    
    if(this->selectedTextureMode == 3){
        txtr.proceduralScale = 1.f;
        txtr.proceduralnverted = this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Invert].checkBox.clickState1;
        txtr.proceduralNormalMap = this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Normal_Map].checkBox.clickState1;
        txtr.proceduralNormalGrayScale = this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Normal_Gray_Scale].checkBox.clickState1;
        txtr.proceduralNormalStrength = this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Normal_Strength].rangeBar.value;
        txtr.proceduralUseTexCoords = this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Use_Texture_Coordinates].checkBox.clickState1;
        txtr.proceduralGrayScale = this->subPanel.sections[0].elements[subPanelTxtrPack_Gray_Scale].checkBox.clickState1;
        txtr.proceduralBrightness = this->subPanel.sections[0].elements[subPanelTxtrPack_Brightness].rangeBar.value;
    }
    
    if(this->selectedTextureMode == 4){
        txtr.proceduralScale = 1.f;
        txtr.proceduralnverted = this->subPanelSmartTextures.sections[0].elements[subPanelSmartTextures_Invert_INDEX].checkBox.clickState1;
        txtr.proceduralNormalMap = this->subPanelSmartTextures.sections[0].elements[subPanelSmartTextures_Normal_Map_INDEX].checkBox.clickState1;
        txtr.proceduralNormalGrayScale = this->subPanelSmartTextures.sections[0].elements[subPanelSmartTextures_Normal_Gray_Scale_INDEX].checkBox.clickState1;
        txtr.proceduralNormalStrength = this->subPanelSmartTextures.sections[0].elements[subPanelSmartTextures_Normal_Strength_INDEX].rangeBar.value;
        txtr.proceduralUseTexCoords = false;
    }

    if(this->selectedTextureMode == 3){
        if(this->selectedTextureIndex < Library::getTexturePackArraySize()){
            Library::getTexturePack(this->selectedTextureIndex)->apply(
                                                                            txtr,
                                                                            this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Scale].rangeBar.value,
                                                                            this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Count].rangeBar.value,
                                                                            this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Rotation_Jitter].rangeBar.value,
                                                                            this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Size_Jitter].rangeBar.value,
                                                                            this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Opacity_Jitter].rangeBar.value,
                                                                            this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Scatter].rangeBar.value
                                                                        );
            ShaderSystem::buttonShader().use();
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    }

    glViewport(0, 0, displayRes, displayRes);

    if(this->selectedTextureMode == 4){
        Camera matCam;
        matCam.cameraPos = glm::vec3(0,0,-8.f);
        matCam.radius = 8.f;


        //Move the camera to the side
        glm::mat4 view = glm::lookAt(matCam.cameraPos, 
                                     glm::vec3(0), 
                                     glm::vec3(0.0, 1.0, 0.0));

        //The perspective projection matrix    
        glm::mat4 projectionMatrix = glm::perspective(
                                                        glm::radians(35.f), //Fov  
                                                        -1.f,  //Ratio (is 1 since the width & the height is equal to displayRes)
                                                        100.f,  //Near (the material is pretty close to the camera actually  ) 
                                                        0.1f    //Far
                                                    );

        //Use the 3D model rendering shader
        ShaderSystem::tdModelShader().use();

        //Throw the camera data to the shader
        ShaderSystem::tdModelShader().setInt("displayingMode", 0);
        ShaderSystem::tdModelShader().setVec3("viewPos",matCam.cameraPos);
        ShaderSystem::tdModelShader().setMat4("view",view);
        ShaderSystem::tdModelShader().setMat4("projection",projectionMatrix);

        Panel* smartPropPanel;
        bool skipPanel = false;
        if(this->selectedTextureIndex == 0 || selectedTextureIndex == 1 || selectedTextureIndex == 2){
            smartPropPanel = &this->smartPositionTexturePanel;
        }
        else if(this->selectedTextureIndex == 3 || selectedTextureIndex == 4){
            smartPropPanel = &this->smartStripesTexturePanel;
        }
        else if(this->selectedTextureIndex == 5){
            smartPropPanel = &this->edgeWearTexturePanel;
        }
        else if(this->selectedTextureIndex == 6){
            smartPropPanel = &this->smartDistanceTexturePanel;
        }
        else{
            skipPanel = true;
        }
        if(!skipPanel){
            txtr.smartProperties = glm::vec4(
                                                smartPropPanel->sections[0].elements[0].rangeBar.value,
                                                smartPropPanel->sections[0].elements[1].rangeBar.value,
                                                smartPropPanel->sections[0].elements[2].rangeBar.value,
                                                smartPropPanel->sections[0].elements[3].rangeBar.value
                                            );
        }

        unsigned int proc = txtr.generateProceduralTexture(getMaterialDisplayerModel()->meshes[0], 512);
        glViewport(0, 0, displayRes, displayRes);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        ShaderSystem::tdModelShader().use();

        //Bind the channels of the material
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, proc);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, proc);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, proc);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, proc);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, proc);
        glActiveTexture(GL_TEXTURE7);
        if(this->selectedTextureIndex == 5)
            glBindTexture(GL_TEXTURE_2D, proc);
        else
            glBindTexture(GL_TEXTURE_2D, this->whiteTxtr.ID);

        getMaterialDisplayerModel()->Draw();

        //Use the button shader (Is necessary since that process is done in the middle of GUI rendering) 
        ShaderSystem::buttonShader().use();

        getBox()->bindBuffers();
        //glDeleteTextures(1, &proc);
    }


    if(this->selectedTextureMode != 3 && this->selectedTextureMode != 4){

        /* Displaying texture */
        ShaderSystem::proceduralDisplayerShader().use();
        glm::mat4 projection = glm::ortho(0.f, (float)displayRes, (float)displayRes, 0.f);
        ShaderSystem::proceduralDisplayerShader().setMat4("projection"  ,       projection);
        ShaderSystem::proceduralDisplayerShader().setVec3("pos"         ,       glm::vec3((float)displayRes / 2.f, (float)displayRes / 2.f,0.9f));
        ShaderSystem::proceduralDisplayerShader().setVec2("scale"       ,       glm::vec2((float)displayRes / 2.f));
        
        if(this->selectedTextureMode == 0)
            ShaderSystem::proceduralDisplayerShader().setInt("proceduralID", -1);
        else if(this->selectedTextureMode == 1)
            ShaderSystem::proceduralDisplayerShader().setInt("proceduralID", selectedTextureIndex);
        else if(this->selectedTextureMode == 2)
            ShaderSystem::proceduralDisplayerShader().setInt("proceduralID", selectedTextureIndex + MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE);                
        else if(this->selectedTextureMode == 4)
            ShaderSystem::proceduralDisplayerShader().setInt("proceduralID", selectedTextureIndex + MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE + MAX_PROCEDURAL_NOISE_TEXTURE_SIZE);                
        else if(this->selectedTextureMode == 6)
            ShaderSystem::proceduralDisplayerShader().setInt("proceduralID", -1);                
        
        ShaderSystem::proceduralDisplayerShader().setFloat("proceduralScale", this->subPanel.sections[0].elements[subPanel_Scale_INDEX].rangeBar.value / 10.f);
        ShaderSystem::proceduralDisplayerShader().setInt("proceduralInverted", this->subPanel.sections[0].elements[subPanel_Invert_INDEX].checkBox.clickState1);
        ShaderSystem::proceduralDisplayerShader().setInt("proceduralGrayScale", this->subPanel.sections[0].elements[subPanel_Gray_Scale_INDEX].checkBox.clickState1);
        ShaderSystem::proceduralDisplayerShader().setFloat("proceduralBrightness", this->subPanel.sections[0].elements[subPanel_Brightness_INDEX].rangeBar.value);
        ShaderSystem::proceduralDisplayerShader().setInt("proceduralGrayScale", this->subPanel.sections[0].elements[subPanel_Gray_Scale_INDEX].checkBox.clickState1);
        ShaderSystem::proceduralDisplayerShader().setFloat("proceduralBrightness", this->subPanel.sections[0].elements[subPanel_Brightness_INDEX].rangeBar.value);

        ShaderSystem::proceduralDisplayerShader().setInt("proceduralTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, txtr.proceduralTextureID);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }


    if((this->subPanel.sections[0].elements[subPanel_Normal_Map_INDEX].checkBox.clickState1 || this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Normal_Map].checkBox.clickState1) && this->selectedTextureMode != 4){
        Texture txtrObject = Texture(txtr.ID);

        unsigned int normalMapRes;

        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1,&normalMapRes);
        glBindTexture(GL_TEXTURE_2D,normalMapRes);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, displayRes, displayRes, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glGenerateMipmap(GL_TEXTURE_2D);

        if(this->selectedTextureMode == 3)
            txtrObject.generateNormalMap(normalMapRes, displayRes, this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Normal_Strength].rangeBar.value, this->subPanelTxtrPack.sections[0].elements[subPanelTxtrPack_Normal_Gray_Scale].checkBox.clickState1); 
        else
            txtrObject.generateNormalMap(normalMapRes, displayRes, this->subPanel.sections[0].elements[subPanel_Normal_Strength_INDEX].rangeBar.value, this->subPanel.sections[0].elements[subPanel_Normal_Gray_Scale_INDEX].checkBox.clickState1); 

        glDeleteTextures(1,&txtr.ID);
        txtr.ID = normalMapRes;
    }

    ShaderSystem::buttonShader().use();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &FBO);
    glDeleteRenderbuffers(1, &RBO);
    glViewport(0, 0, viewportResolution.x, viewportResolution.y);
}


//Forward declarations for the utility functions
static void initTextureSelectionDialog(int &selectedTextureMode, unsigned int& bgTexture, glm::ivec2& windowSize, Panel& subPanel, int& selectedTextureIndex, Texture& receivedTexture);
static void drawBG(unsigned int bgTexture, glm::ivec2 windowSize);
static void updateTextureSelectingPanelElements(Panel& textureSelectingPanel, int selectedTextureMode, std::vector<Texture> smartTextureDisplayingTextures);
static void updateSubPanel(Panel& subPanel, Panel& subPanelTxtrPack, Panel& subPanelSmartTextures, int& selectedTextureMode, int& selectedTextureIndex);

void TextureSelectionDialog::show(Timer &timer, glm::mat4 guiProjection, Texture& receivedTexture, int displayingTextureRes){
    
    this->dialogControl.activate();
    
    if(this->whiteTxtr.ID == 0){
        std::vector<char> whitePx = {127, 127, 127, 127}; 
        this->whiteTxtr = Texture(&whitePx[0], 1, 1, GL_NEAREST);
    }
        
    if(!this->smartTextureDisplayingTextures.size()){
        for (size_t i = 0; i < MAX_PROCEDURAL_SMART_TEXTURE_SIZE; i++)
        {
            Texture dispTxtr;
            this->selectedTextureMode = 4;
            this->selectedTextureIndex = i;
            this->generateDisplayingTexture(dispTxtr, 256);
            this->smartTextureDisplayingTextures.push_back(dispTxtr);
        }
    }
    
    unsigned int bgTexture; 
    glm::ivec2 windowSize;
    initTextureSelectionDialog(this->selectedTextureMode, bgTexture, windowSize, this->subPanel, this->selectedTextureIndex, receivedTexture);


    while (!getContext()->window.shouldClose())
    {
        getContext()->window.pollEvents();
        
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawBG(bgTexture, windowSize);

        dialogControl.updateStart();

        generateDisplayingTexture(displayingTexture, 512);

        updateTextureSelectingPanelElements(this->textureSelectingPanel, this->selectedTextureMode, this->smartTextureDisplayingTextures);

        updateSubPanel(this->subPanel, this->subPanelTxtrPack, this->subPanelSmartTextures, this->selectedTextureMode, this->selectedTextureIndex);

        //Render the panel
        this->bgPanel.render(timer, true);

        //Render the texture mode selection panel
        if(this->selectedTextureMode == 3)
            this->subPanelTxtrPack.render(timer, true);
        else if(this->selectedTextureMode == 4)
            this->subPanelSmartTextures.render(timer, true);
        else
            this->subPanel.render(timer, true);

        selectedTextureDisplayingPanel.scale.x = this->scale.x - subPanel.scale.x;
        selectedTextureDisplayingPanel.pos.x = this->pos.x + subPanel.scale.x;

        textureSelectingPanel.pos.x = selectedTextureDisplayingPanel.pos.x;
        textureSelectingPanel.scale.x = selectedTextureDisplayingPanel.scale.x;

        selectedTextureDisplayingPanel.sections[0].elements[0].button.texture = displayingTexture;

        if(this->selectedTextureMode == 4)
            this->textureSelectingPanel.rowCount = 7;    
        else
            this->textureSelectingPanel.rowCount = 12;    
        this->textureSelectingPanel.render(timer, true);

        ShaderSystem::buttonShader().setInt("properties.invertTheTexture", false);
        ShaderSystem::buttonShader().setVec2("properties.txtrScale", glm::vec2(1.f));
        this->selectedTextureDisplayingPanel.render(timer, true);

        if(this->selectedTextureMode == 0 || this->selectedTextureMode == 1 || this->selectedTextureMode == 2){
            for (size_t i = 0; i < this->textureSelectingPanel.sections[0].elements.size(); i++)
            {
                ShaderSystem::proceduralDisplayerShader().use();
                ShaderSystem::proceduralDisplayerShader().setVec3("pos"         ,       this->textureSelectingPanel.sections[0].elements[i].button.resultPos);
                ShaderSystem::proceduralDisplayerShader().setVec2("scale"       ,       glm::vec2(std::min(this->textureSelectingPanel.sections[0].elements[i].button.resultScale.x, this->textureSelectingPanel.sections[0].elements[i].button.resultScale.y)));
                ShaderSystem::proceduralDisplayerShader().setMat4("projection"  ,       guiProjection);
                
                if(this->selectedTextureMode == 0)
                    ShaderSystem::proceduralDisplayerShader().setInt("proceduralID", -1);
                else if(this->selectedTextureMode == 1)
                    ShaderSystem::proceduralDisplayerShader().setInt("proceduralID", i);
                else if(this->selectedTextureMode == 2)
                    ShaderSystem::proceduralDisplayerShader().setInt("proceduralID", i + MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE);
                else if(this->selectedTextureMode == 4)
                    ShaderSystem::proceduralDisplayerShader().setInt("proceduralID", i + MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE + MAX_PROCEDURAL_NOISE_TEXTURE_SIZE);

                if(this->selectedTextureMode == 0)
                    ShaderSystem::proceduralDisplayerShader().setFloat("proceduralScale", 1.f);
                else
                    ShaderSystem::proceduralDisplayerShader().setFloat("proceduralScale", 2.5f);
                
                ShaderSystem::proceduralDisplayerShader().setInt("proceduralInverted", 0);
                ShaderSystem::proceduralDisplayerShader().setInt("proceduralGrayScale", 0);
                ShaderSystem::proceduralDisplayerShader().setFloat("proceduralBrightness", 1.f);
                
                ShaderSystem::proceduralDisplayerShader().setInt("proceduralTexture", 0);
                glActiveTexture(GL_TEXTURE0);
                if(this->selectedTextureMode == 0){
                    if(i < Library::getTextureArraySize())
                        glBindTexture(GL_TEXTURE_2D, Library::getTexture(i)->ID);
                }
                else{
                    if(i < getModel()->meshes.size())
                        glBindTexture(GL_TEXTURE_2D, getModel()->meshes[selectedTextureIndex].uvMask);
                }
                

                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }

        ShaderSystem::buttonShader().use();

        if(smartPositionTexturePanelActive){
            this->smartPositionTexturePanel.render(timer, true);
            if((getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE)) || (!this->smartPositionTexturePanel.hover && *Mouse::LClick())){
                smartPositionTexturePanelActive = false;
            }
        }

        if(smartStripesTexturePanelActive){
            this->smartStripesTexturePanel.render(timer, true);
            if((getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE)) || (!this->smartStripesTexturePanel.hover && *Mouse::LClick())){
                smartStripesTexturePanelActive = false;
            }
        }
        if(edgeWearTexturePanelActive){
            this->edgeWearTexturePanel.render(timer, true);
            if((getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE)) || (!this->edgeWearTexturePanel.hover && *Mouse::LClick())){
                edgeWearTexturePanelActive = false;
            }
        }
        if(smartDistanceTexturePanelActive){
            this->smartDistanceTexturePanel.render(timer, true);
            if((getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE)) || (!this->smartDistanceTexturePanel.hover && *Mouse::LClick())){
                smartDistanceTexturePanelActive = false;
            }
        }
        
        //If pressed any of the texture select the texture
        for (size_t i = 0; i < this->textureSelectingPanel.sections[0].elements.size(); i++)
        {
            if(this->textureSelectingPanel.sections[0].elements[i].button.hover && *Mouse::LClick()){
                selectedTextureIndex = i;
                if(this->selectedTextureMode == 4){
                    if(selectedTextureIndex == 0 || selectedTextureIndex == 1 || selectedTextureIndex == 2){
                        smartPositionTexturePanelActive = true;
                        smartPositionTexturePanel.pos = this->textureSelectingPanel.sections[0].elements[i].button.pos;
                        smartPositionTexturePanel.pos.y += this->textureSelectingPanel.sections[0].elements[i].button.scale.y + smartPositionTexturePanel.scale.y;
                    }
                    if(selectedTextureIndex == 3 || selectedTextureIndex == 4){
                        smartStripesTexturePanelActive = true;
                        smartStripesTexturePanel.pos = this->textureSelectingPanel.sections[0].elements[i].button.pos;
                        smartStripesTexturePanel.pos.y += this->textureSelectingPanel.sections[0].elements[i].button.scale.y + smartStripesTexturePanel.scale.y;
                    }
                    if(selectedTextureIndex == 5){
                        edgeWearTexturePanelActive = true;
                        edgeWearTexturePanel.pos = this->textureSelectingPanel.sections[0].elements[i].button.pos;
                        edgeWearTexturePanel.pos.y += this->textureSelectingPanel.sections[0].elements[i].button.scale.y + edgeWearTexturePanel.scale.y;
                    }
                    if(selectedTextureIndex == 6){
                        smartDistanceTexturePanelActive = true;
                        smartDistanceTexturePanel.pos = this->textureSelectingPanel.sections[0].elements[i].button.pos;
                        smartDistanceTexturePanel.pos.y += this->textureSelectingPanel.sections[0].elements[i].button.scale.y + smartDistanceTexturePanel.scale.y;
                    }
                }
            }
        }



        // Pressed to the select button
        if((this->subPanel.sections[0].elements[11].button.clicked && this->selectedTextureMode != 3) || (this->subPanelTxtrPack.sections[0].elements[16].button.clicked && this->selectedTextureMode == 3) || (this->subPanelSmartTextures.sections[0].elements[9].button.clicked && this->selectedTextureMode == 4)){
            
            generateDisplayingTexture(receivedTexture, displayingTextureRes);
            
            receivedTexture.title = "SelectedTexture";
            
            dialogControl.unActivate();
        }

        //End the dialog
        if((getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE)) || (!this->bgPanel.hover && *Mouse::LClick())){
            dialogControl.unActivate();
            selectedTextureIndex = 0;
        }

        if(!this->dialogControl.isActive())
            break;
            
        dialogControl.updateEnd(timer,0.15f);

        getContext()->window.swapBuffers();

        //Set mouse states to default
        *Mouse::LClick() = false;
        *Mouse::RClick() = false;
        *Mouse::MClick() = false;
        *Mouse::LDoubleClick() = false;
        *Mouse::mouseOffset() = glm::vec2(0);
        *Mouse::mods() = 0;
        *Mouse::mouseScroll() = 0;
        *Mouse::action() = 0;

        //Set keyboard states to default
        textRenderer.keyInput = false;
        textRenderer.mods = 0;
    }
}





// ---------- UTILITY FUNCTIONS -----------
static void initTextureSelectionDialog(
                                        int &selectedTextureMode, 
                                        unsigned int& bgTexture, 
                                        glm::ivec2& windowSize, 
                                        Panel& subPanel, 
                                        int& selectedTextureIndex, 
                                        Texture& receivedTexture
                                    )
{
    // Get the viewport size
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    int viewportWidth = viewport[2];
    int viewportHeight = viewport[3];

    windowSize = glm::ivec2(viewportWidth, viewportHeight);

    GLfloat* pixels = new GLfloat[windowSize.x * windowSize.y * 4];
    glReadPixels(0, 0, windowSize.x, windowSize.y, GL_RGBA, GL_FLOAT, pixels);

    glGenTextures(1, &bgTexture);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bgTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, windowSize.x, windowSize.y, 0, GL_RGBA, GL_FLOAT, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    selectedTextureMode = 0;
    selectedTextureIndex = 0;

    if(receivedTexture.proceduralID != -1){
        selectedTextureMode = 1;
        selectedTextureIndex = receivedTexture.proceduralID;
        
        if(selectedTextureIndex > MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE - 1){
            selectedTextureMode = 2;
            selectedTextureIndex -= MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE;
        }
        
        subPanel.sections[0].elements[subPanel_Invert_INDEX].checkBox.clickState1 = receivedTexture.proceduralnverted;
        subPanel.sections[0].elements[subPanel_Scale_INDEX].rangeBar.value = receivedTexture.proceduralScale * 10.f;  
        subPanel.sections[0].elements[subPanel_Normal_Map_INDEX].checkBox.clickState1 = receivedTexture.proceduralNormalMap;
        subPanel.sections[0].elements[subPanel_Normal_Gray_Scale_INDEX].checkBox.clickState1 = receivedTexture.proceduralNormalGrayScale;
        subPanel.sections[0].elements[subPanel_Normal_Strength_INDEX].rangeBar.value = receivedTexture.proceduralNormalStrength;
        subPanel.sections[0].elements[subPanel_Gray_Scale_INDEX].rangeBar.value = receivedTexture.proceduralGrayScale;
        subPanel.sections[0].elements[subPanel_Brightness_INDEX].rangeBar.value = receivedTexture.proceduralBrightness;
    }
}

static void drawBG(
                    unsigned int bgTexture, 
                    glm::ivec2 windowSize
                )
{
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
}

static void updateTextureSelectingPanelElements(Panel& textureSelectingPanel, int selectedTextureMode, std::vector<Texture> smartTextureDisplayingTextures){
    textureSelectingPanel.sections.clear();
    std::vector<Element> sectionElements;
    if(selectedTextureMode == 0){
        for (size_t i = 0; i < Library::getTextureArraySize(); i++)
        {
            sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),""       , Texture(), 0.f,false)));
        }
    }
    else if(selectedTextureMode == 1){
        for (size_t i = 0; i < MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE; i++)
        {
            sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,3.f),""       , Texture(), 0.f,false)));
        }
    }
    else if(selectedTextureMode == 2){
        for (size_t i = 0; i < MAX_PROCEDURAL_NOISE_TEXTURE_SIZE; i++)
        {
            sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,3.f),""       , Texture(), 0.f,false)));
        }
    }
    else if(selectedTextureMode == 3){
        for (size_t i = 0; i < Library::getTexturePackArraySize(); i++)
        {
            sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,3.f),Library::getTexturePack(i)->title   , Texture(), 0.f,false)));
        }
    }
    else if(selectedTextureMode == 4){
        for (size_t i = 0; i < MAX_PROCEDURAL_SMART_TEXTURE_SIZE; i++)
        {
            sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,3.f),""       , smartTextureDisplayingTextures[i], 0.f,false)));
        }
    }
    else if(selectedTextureMode == 6){
        for (size_t i = 0; i < getModel()->meshes.size(); i++)
        {
            sectionElements.push_back(Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,3.f),getModel()->meshes[i].materialName       , getModel()->meshes[i].uvMask, 0.f,false)));
        }
    }

    textureSelectingPanel.sections.push_back
                                            (
                                                Section
                                                        (
                                                            Element(Button()),
                                                            sectionElements
                                                        )
                                            );  
}

static void updateSubPanel(Panel& subPanel, Panel& subPanelTxtrPack, Panel& subPanelSmartTextures, int& selectedTextureMode, int& selectedTextureIndex){
        for (size_t i = 0; i < 7; i++)
        {
            if(selectedTextureMode == 3){
                if(subPanelTxtrPack.sections[0].elements[i].button.clickState1 && selectedTextureMode != i){
                    selectedTextureMode = i;
                    selectedTextureIndex = 0;
                    for (size_t i = 0; i < subPanel.sections[0].elements.size(); i++){
                        subPanel.sections[0].elements[i].button.clickState1 = false;
                        subPanelTxtrPack.sections[0].elements[i].button.clickState1 = false;
                        subPanelSmartTextures.sections[0].elements[i].button.clickState1 = false;
                    }
                }
            }
            else if(selectedTextureMode == 4){
                if(subPanelSmartTextures.sections[0].elements[i].button.clickState1 && selectedTextureMode != i){
                    selectedTextureMode = i;
                    selectedTextureIndex = 0;
                    for (size_t i = 0; i < subPanel.sections[0].elements.size(); i++){
                        subPanel.sections[0].elements[i].button.clickState1 = false;
                        subPanelSmartTextures.sections[0].elements[i].button.clickState1 = false;
                        subPanelSmartTextures.sections[0].elements[i].button.clickState1 = false;
                    }
                }
            }
            else{
                if(subPanel.sections[0].elements[i].button.clickState1 && selectedTextureMode != i){
                    selectedTextureMode = i;
                    selectedTextureIndex = 0;
                    for (size_t i = 0; i < subPanel.sections[0].elements.size(); i++){
                        subPanel.sections[0].elements[i].button.clickState1 = false;
                        subPanelTxtrPack.sections[0].elements[i].button.clickState1 = false;
                        subPanelSmartTextures.sections[0].elements[i].button.clickState1 = false;
                    }
                }
            }

            if(selectedTextureMode == i){
                subPanel.sections[0].elements[i].button.clickState1 = true;
                subPanelTxtrPack.sections[0].elements[i].button.clickState1 = false;
                subPanelSmartTextures.sections[0].elements[i].button.clickState1 = false;
            }
            else{
                subPanel.sections[0].elements[i].button.clickState1 = false;
                subPanelTxtrPack.sections[0].elements[i].button.clickState1 = false;
                subPanelSmartTextures.sections[0].elements[i].button.clickState1 = false;
            }
        }
}