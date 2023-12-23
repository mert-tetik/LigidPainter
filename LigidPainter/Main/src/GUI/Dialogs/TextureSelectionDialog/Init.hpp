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

#include "GUI/GUI.hpp"
#include "LibrarySystem/Library.hpp"
#include "MouseSystem/Mouse.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

#define MAX_PROCEDURAL_PATTERN_TEXTURE_SIZE 66
#define MAX_PROCEDURAL_NOISE_TEXTURE_SIZE 50
#define MAX_PROCEDURAL_SMART_TEXTURE_SIZE 7

#define subPanel_Gray_Scale_INDEX 0
#define subPanel_Brightness_INDEX 1 
#define subPanel_Invert_INDEX 2
#define subPanel_Scale_INDEX 3
#define subPanel_Normal_Map_INDEX 4
#define subPanel_Normal_Gray_Scale_INDEX 5
#define subPanel_Normal_Strength_INDEX 6
#define subPanel_Use_Texture_Coordinates_INDEX 7
#define subPanel_MirroredRepeat_INDEX 8
#define subPanel_Stretch_INDEX 9
#define subPanel_Select_INDEX 10

#define subPanelSmartTextures_Invert_INDEX 0
#define subPanelSmartTextures_Brightness_INDEX 1
#define subPanelSmartTextures_Normal_Map_INDEX 2
#define subPanelSmartTextures_Normal_Gray_Scale_INDEX 3
#define subPanelSmartTextures_Normal_Strength_INDEX 4
#define subPanelSmartTextures_Select_INDEX 5

#define subPanelTxtrPack_Gray_Scale_INDEX 0
#define subPanelTxtrPack_Brightness_INDEX 1
#define subPanelTxtrPack_Invert_INDEX 2
#define subPanelTxtrPack_Scale_INDEX 3
#define subPanelTxtrPack_Normal_Map_INDEX 4
#define subPanelTxtrPack_Normal_Gray_Scale_INDEX 5
#define subPanelTxtrPack_Normal_Strength_INDEX 6
#define subPanelTxtrPack_Count_INDEX 7
#define subPanelTxtrPack_Rotation_Jitter_INDEX 8
#define subPanelTxtrPack_Size_Jitter_INDEX 9
#define subPanelTxtrPack_Opacity_Jitter_INDEX 10
#define subPanelTxtrPack_Scatter_INDEX 11
#define subPanelTxtrPack_Use_Texture_Coordinates_INDEX 12 
#define subPanelTxtrPack_MirroredRepeat_INDEX 13
#define subPanelTxtrPack_Stretch_INDEX 14
#define subPanelTxtrPack_Select_INDEX 15


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
                                                    glm::vec2(scale.x / 6.f, scale.y),
                                                    glm::vec3(pos.x, pos.y, pos.z),
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
                                                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2, scale.y / 3.f),"", Texture(), 0.f, true),
                                                            }
                                                        )
                                                    },
                                                    glm::vec2(scale.x / 6.f, scale.y / 3.f),
                                                    glm::vec3(pos.x, pos.y - scale.y + scale.y / 3.f, pos.z),
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


    this->textureModesPanel = Panel(
                                
                                
                                {
                                    Section(
                                        Button(),
                                        {
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Selection Mode", Texture(), 1.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Library Textures", Texture(), 3.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Procedural Pattern Textures", Texture(), 2.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Procedural Noise Textures", Texture(), 2.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Texture Packs", Texture(), 2.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Smart Textures", Texture(), 2.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Source Textures", Texture(), 2.f, true),
                                            Button(ELEMENT_STYLE_SOLID,glm::vec2(2,2.f),"Mesh UV Textures", Texture(), 2.f, true)
                                        }
                                    )
                                },
                                glm::vec2(scale.x / 6.f, scale.y),
                                glm::vec3(pos.x - scale.x + (scale.x / 6.f) , pos.y, pos.z),
                                ColorPalette::secondColor,
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
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Gray Scale", 1.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Brightness", Texture(), 0.f, 0.f, 2.f, 1.f),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Invert", 0.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Scale", Texture(), 0.f, 0.1f, 300.f, 10.f),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Normal Map", 4.f),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Normal Gray Scale", 0.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Normal Strength", Texture(), 0.f, 0.f, 100.f, 10.f),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Use Texture Coordinates", 4.f),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f), "Mirrored Repeat", 2.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Stretch", Texture(), 0.5f, 0.f, 10.f, 1.f),
                                            Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,2.f),"Select", Texture(), 2.f, false),
                                        }
                                    )
                                },
                                glm::vec2(scale.x / 4.f, scale.y - selectedTextureDisplayingPanel.scale.y),
                                glm::vec3(pos.x + scale.x - (scale.x / 4.f) , selectedTextureDisplayingPanel.pos.y + selectedTextureDisplayingPanel.scale.y + (scale.y - selectedTextureDisplayingPanel.scale.y), pos.z),
                                ColorPalette::secondColor,
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
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Invert", 1.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Brightness", Texture(), 0.f, 0.f, 2.f, 1.f),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Normal Map", 4.f),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Normal Gray Scale", 0.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Normal Strength", Texture(), 0.f, 0.f, 100.f, 10.f),
                                            Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,2.f),"Select", Texture(), 6.f, false)
                                        }
                                    )
                                },
                                glm::vec2(scale.x / 4.f, scale.y - selectedTextureDisplayingPanel.scale.y),
                                glm::vec3(pos.x + scale.x - (scale.x / 4.f) , selectedTextureDisplayingPanel.pos.y + selectedTextureDisplayingPanel.scale.y + (scale.y - selectedTextureDisplayingPanel.scale.y), pos.z),
                                ColorPalette::secondColor,
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
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Gray Scale", 1.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Brightness", Texture(), 0.f, 0.f, 100.f, 10.f),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Invert", 0.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Scale", Texture(), 0.f, 0.1f, 300.f, 10.f),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Normal Map", 4.f),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Normal Gray Scale", 0.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Normal Strength", Texture(), 0.f, 0.f, 100.f, 10.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Count", Texture(), 4.f, 0.f, 0.5f, 0.05f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Rotation Jitter", Texture(), 0.5f, 0.f, 1.f, 0.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Size Jitter", Texture(), 0.5f, 0.f, 1.f, 0.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Opacity Jitter", Texture(), 0.5f, 0.f, 1.f, 0.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Scatter", Texture(), 0.5f, 0.f, 1.f, 1.f),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f),"Use Texture Coordinates", 4.f),
                                            CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2.f), "Mirrored Repeat", 2.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Stretch", Texture(), 0.5f, 0.f, 10.f, 1.f),
                                            Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,2.f),"Select", Texture(), 1.f, false),
                                        }
                                    )
                                },
                                glm::vec2(scale.x / 4.f, scale.y - selectedTextureDisplayingPanel.scale.y),
                                glm::vec3(pos.x + scale.x - (scale.x / 4.f) , selectedTextureDisplayingPanel.pos.y + selectedTextureDisplayingPanel.scale.y + (scale.y - selectedTextureDisplayingPanel.scale.y), pos.z),
                                ColorPalette::secondColor,
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
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Yaw", Texture(), 2.f, 0.f, 360.f, 0.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Pitch", Texture(), 2.f, 0.f, 360.f, 0.0f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Position", Texture(), 2.f, -1.f, 1.f, 0.f)
                                        }
                                    )
                                },
                                glm::vec2(8.f, 9.f),
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
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Thickness", Texture(), 2.f, 0.f, 1.f, 0.25f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Blur", Texture(), 2.f, 0.f, 0.001, 0.0005f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Rotation", Texture(), 2.f, 0.f, 360.f, 0.f)
                                        }
                                    )
                                },
                                glm::vec2(8.f, 9.f),
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
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Multiply", Texture(), 2.f, 0.f, 10.f, 3.37f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Blur", Texture(), 2.f, 0.f, 10.f, 0.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Noise Strength", Texture(), 2.f, 0.f, 1.f, 1.f)
                                        }
                                    )
                                },
                                glm::vec2(8.f, 9.f),
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
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Max", Texture(), 2.f, 0.f, 1.f, 1.f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Offset", Texture(), 2.f, 0.f, 20.f, 13.15f),
                                            RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(2,1.f),"Noise Strength", Texture(), 2.f, 0.f, 1.f, 0.f)
                                        }
                                    )
                                },
                                glm::vec2(8.f, 9.f),
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

    this->textureModesPanel.sections[0].elements[0].button.color = glm::vec4(0);
    this->textureModesPanel.sections[0].elements[0].button.color2 = glm::vec4(0);
    this->textureModesPanel.sections[0].elements[0].button.textColor2 = ColorPalette::oppositeColor;
    
    for (size_t i = 1; i < 8; i++)
    {
        this->textureModesPanel.sections[0].elements[i].button.color = glm::vec4(0);
    }

    this->selectedTextureSolidDisplayingModeBtn = Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1),"Solid", appTextures.twoDIcon, 1.f,false);
    this->selectedTextureSolidDisplayingModeBtn.textColor2 = glm::vec4(1.);
    this->selectedTextureMaterialBallDisplayingMode = Button(ELEMENT_STYLE_SOLID,glm::vec2(3,1),"Material Ball", appTextures.materialIcon, 1.f,false);
    this->selectedTextureMaterialBallDisplayingMode.textColor2 = glm::vec4(1.);
    this->selectedTextureCustomMeshDisplayingMode = Button(ELEMENT_STYLE_SOLID,glm::vec2(3,1),"Custom Mesh", appTextures.TDModelIcon, 1.f,false);
    this->selectedTextureCustomMeshDisplayingMode.textColor2 = glm::vec4(1.);
}