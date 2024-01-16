/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Material modifier's GUI elements

*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "GUI/Elements/Elements.hpp"
#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"
#include "ShaderSystem/Shader.hpp"
#include "SettingsSystem/Settings.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

//0 = albedo
//1 = roughness
//2 = metallic 
//3 = normal map
//4 = height map
//5 = ambient Occlusion

#define MATERIAL_MODIFIERS_ELEMENT_OFFSET 2.5f

std::vector<Section> MaterialModifier::createTextureModifier(){
    unsigned char whitePixel[] = { 255, 255, 255, 255 }; // 1 pixel, RGBA format (white)
        
    std::vector<Section> sections =  
    {
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Channels"),
            {
                Element(Button(ELEMENT_STYLE_STYLIZED, glm::vec2(1, 2.f),"Auto Fill",              Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,false)),
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Albedo",              Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,false)),
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Roughness",           Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,false)),
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Metallic",            Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,false)),
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Normal map",          Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,false)),
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Height map",          Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,false)),
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Ambient Occlusion",   Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,false))
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Channel Opacities"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Albedo Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Roughness Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Metallic Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Normal Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Height Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Ambient Occlusion Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Depth Masking & Depth Properties"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1.f, 2.f), "Depth Value",  Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Blur the Height Map", MATERIAL_MODIFIERS_ELEMENT_OFFSET)
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Image Filter"),
            {
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f), "Albedo Filter", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f), "Albedo Filter Mask", Texture(whitePixel, 1, 1, GL_NEAREST, 24), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.f), "Remove The Filter", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false)
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Ambient Occlusion (Physically Based)"),
            {
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Calculate Physically Based Ambient Occlusion", MATERIAL_MODIFIERS_ELEMENT_OFFSET),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Use Previous Ambient Occlusion ", MATERIAL_MODIFIERS_ELEMENT_OFFSET),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Offset", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 2.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Smoothness", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 5.f, 2.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+X Direction", 1.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-X Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+Y Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-Y Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+Z Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-Z Direction", 0.f),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.f), "", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false)
            }
        )
    };

    sections[0].elements[0].button.textureSelection3D = true;
    sections[0].elements[0].button.selectionDialogTxtrRes = 128;
    sections[0].elements[1].button.textureSelection3D = true;
    sections[0].elements[1].button.selectionDialogTxtrRes = 128;
    sections[0].elements[2].button.textureSelection3D = true;
    sections[0].elements[2].button.selectionDialogTxtrRes = 128;
    sections[0].elements[3].button.textureSelection3D = true;
    sections[0].elements[3].button.selectionDialogTxtrRes = 128;
    sections[0].elements[4].button.textureSelection3D = true;
    sections[0].elements[4].button.selectionDialogTxtrRes = 128;
    sections[0].elements[5].button.textureSelection3D = true;
    sections[0].elements[5].button.selectionDialogTxtrRes = 128;
    sections[0].elements[6].button.textureSelection3D = true;
    sections[0].elements[6].button.selectionDialogTxtrRes = 128;

    sections[sections.size() - 2].elements[0].button.filterSelection = true;
    sections[sections.size() - 2].elements[0].button.selectionDialogTxtrRes = 128;
    sections[sections.size() - 2].elements[1].button.textureSelection3D = true;
    sections[sections.size() - 2].elements[1].button.selectionDialogTxtrRes = 128;
    
    sections[sections.size() - 1].elements[4].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[5].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[6].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[7].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[8].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[9].checkBox.clickState1 = true;

    sections[sections.size() - 1].elements[10].button.color = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.color2 = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.textColor = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.textColor2 = glm::vec4(0.f);
    
    return sections;
}

std::vector<Section> MaterialModifier::createSolidModifier(){
    unsigned char whitePixel[] = { 255, 255, 255, 255 }; // 1 pixel, RGBA format (white)
        
    std::vector<Section> sections =  
    {
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Channels"),
            {
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Albedo",              Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, false)),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Albedo Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Roughness",           Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, false)),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Roughness Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Metallic",            Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, false)),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Metallic Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Normal Map",          Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, false)),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Normal Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Height map",          Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, false)),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Height Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Ambient Occlusion",   Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, false)),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Ambient Occlusion Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f ) 
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Depth Masking & Depth Properties"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1.f, 2.f), "Depth Value",  Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Blur the Height Map", MATERIAL_MODIFIERS_ELEMENT_OFFSET)
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Image Filter"),
            {
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f), "Albedo Filter", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f), "Albedo Filter Mask", Texture(whitePixel, 1, 1, GL_NEAREST, 24), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.f), "Remove The Filter", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false)
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Ambient Occlusion (Physically Based)"),
            {
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Calculate Physically Based Ambient Occlusion", MATERIAL_MODIFIERS_ELEMENT_OFFSET),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Use Previous Ambient Occlusion ", MATERIAL_MODIFIERS_ELEMENT_OFFSET),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Offset", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 2.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Smoothness", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 5.f, 2.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+X Direction", 1.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-X Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+Y Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-Y Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+Z Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-Z Direction", 0.f),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.f), "", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false)
            }
        )
    };

    sections[0].elements[0].button.colorSelection = true;
    sections[0].elements[0].button.color = glm::vec4(0.f,0.f,0.f,1.f);
    sections[0].elements[2].button.colorSelection = true;
    sections[0].elements[4].button.colorSelection = true;
    sections[0].elements[6].button.colorSelection = true;
    sections[0].elements[6].button.color = glm::vec4(0.5f,0.5f,1.f,1.f);
    sections[0].elements[8].button.colorSelection = true;
    sections[0].elements[8].button.color = glm::vec4(0.5f,0.5f,0.5f,1.f);
    sections[0].elements[10].button.colorSelection = true;
    sections[0].elements[10].button.color = glm::vec4(1.f,1.f,1.f,1.f);

    sections[sections.size() - 2].elements[0].button.filterSelection = true;
    sections[sections.size() - 2].elements[0].button.selectionDialogTxtrRes = 128;
    sections[sections.size() - 2].elements[1].button.textureSelection3D = true;
    sections[sections.size() - 2].elements[1].button.selectionDialogTxtrRes = 128;
    
    sections[sections.size() - 1].elements[4].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[5].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[6].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[7].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[8].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[9].checkBox.clickState1 = true;

    sections[sections.size() - 1].elements[10].button.color = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.color2 = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.textColor = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.textColor2 = glm::vec4(0.f);
    
    return sections;
}

std::vector<Section> MaterialModifier::createLiquidModifier(){
    unsigned char whitePixel[] = { 255, 255, 255, 255 }; // 1 pixel, RGBA format (white)
    
    std::vector<Section> sections =  
    {
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Scaling"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1,1.5f), "Scale", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 50.f, 6.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1,1.5f), "Scale Y axis", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 10.f, 1.f)
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Noise"),
            {
                RangeBar(ELEMENT_STYLE_STYLIZED, glm::vec2(1,1.5f),"Octaves",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0, 12, 8),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1,1.5f),"Fbm X Offset",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 10.f, 0.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1,1.5f),"Fbm Y Offset",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 10.f, 7.7f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1,1.5f),"Fbm X Scale",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 5.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1,1.5f),"Fbm Y Scale",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 5.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1,1.5f),"Roughness",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 0.2f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1,1.5f),"Frequency Acceleration",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 6.f, 2.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1,1.5f),"Fbm Total",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, -1.f, 1.f, 0.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1,1.5f),"Seed",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 0.3f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "General"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1,1.5f), "Noise Influence", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 10.f, 6.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1,1.5f), "Position Influence", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 10.f, 1.8f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Colors"),
            {
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1,1.5f), "Color1", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1,1.5f), "Color2", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1,1.5f), "Color3", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1,1.5f), "Color4", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Element Properties"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1,1.5f), "Wetness", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 0.6f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1,1.5f), "Metallic", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 0.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1,1.5f), "Height", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1,1.5f), "Ambient Occlusion", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f)
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Channel Opacities"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Albedo Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Roughness Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Metallic Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Normal Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Height Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Ambient Occlusion Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Depth Masking & Depth Properties"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1.f, 2.f), "Depth Value",  Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Blur the Height Map", MATERIAL_MODIFIERS_ELEMENT_OFFSET)

            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Image Filter"),
            {
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f), "Albedo Filter", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f), "Albedo Filter Mask", Texture(whitePixel, 1, 1, GL_NEAREST, 24), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.f), "Remove The Filter", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false)
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Ambient Occlusion (Physically Based)"),
            {
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Calculate Physically Based Ambient Occlusion", MATERIAL_MODIFIERS_ELEMENT_OFFSET),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Use Previous Ambient Occlusion ", MATERIAL_MODIFIERS_ELEMENT_OFFSET),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Offset", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 2.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Smoothness", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 5.f, 2.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+X Direction", 1.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-X Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+Y Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-Y Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+Z Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-Z Direction", 0.f),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.f), "", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false)
            }
        )
    };
    sections[3].elements[0].button.colorSelection = true;
    sections[3].elements[0].button.color = glm::vec4(glm::vec3(0.23f, 0.25f, 0.24f), 1.f);
    sections[3].elements[1].button.colorSelection = true;
    sections[3].elements[1].button.color = glm::vec4(glm::vec3(0.62f, 0.71f, 0.70f), 1.f);
    sections[3].elements[2].button.colorSelection = true;
    sections[3].elements[2].button.color = glm::vec4(glm::vec3(0.85f, 0.94f, 0.93f), 1.f);
    sections[3].elements[3].button.colorSelection = true;
    sections[3].elements[3].button.color = glm::vec4(glm::vec3(0.58f, 0.82f, 0.75f), 1.f);

    sections[sections.size() - 2].elements[0].button.filterSelection = true;
    sections[sections.size() - 2].elements[0].button.selectionDialogTxtrRes = 128;
    sections[sections.size() - 2].elements[1].button.textureSelection3D = true;
    sections[sections.size() - 2].elements[1].button.selectionDialogTxtrRes = 128;
    
    sections[sections.size() - 1].elements[4].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[5].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[6].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[7].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[8].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[9].checkBox.clickState1 = true;

    sections[sections.size() - 1].elements[10].button.color = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.color2 = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.textColor = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.textColor2 = glm::vec4(0.f);
    
    return sections;
} 

std::vector<Section> MaterialModifier::createMossModifier(){
    unsigned char whitePixel[] = { 255, 255, 255, 255 }; // 1 pixel, RGBA format (white)
    
    std::vector<Section> sections =  
    {
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Color"),
            {
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Moss Color Back", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Moss Color Front", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Dirt Color", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Droplets"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Droplets Count", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 100.f, 10.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Droplets Opacity Jitter", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Droplets Size", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 10.f, 1.5f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Front Layer"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Front Layer Strength", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Front Layer Scale", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 2.f, 0.015f), 
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Lighting"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Light Strength", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 2.f, 1.8f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Darkening Strength", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 2.f, 1.7f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Noise"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Noise Strength", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 2.f, 1.3f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Moss Properties"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Scale", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 100.f, 40.f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Element Properties"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Wetness", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 0.4f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Metallic", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 0.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Height", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Channel Opacities"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Albedo Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Roughness Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Metallic Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Normal Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Height Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Ambient Occlusion Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Depth Masking & Depth Properties"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1.f, 2.f), "Depth Value",  Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Blur the Height Map", MATERIAL_MODIFIERS_ELEMENT_OFFSET)
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Image Filter"),
            {
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f), "Albedo Filter", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f), "Albedo Filter Mask", Texture(whitePixel, 1, 1, GL_NEAREST, 24), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.f), "Remove The Filter", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false)
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Ambient Occlusion (Physically Based)"),
            {
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Use Physically Based AO", MATERIAL_MODIFIERS_ELEMENT_OFFSET),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Use Previous AO ", MATERIAL_MODIFIERS_ELEMENT_OFFSET),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Offset", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 10.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Smoothness", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 5.f, 2.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+X Direction", 1.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-X Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+Y Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-Y Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+Z Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-Z Direction", 0.f),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.f), "", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false)
            }
        )
    };

    sections[0].elements[0].button.colorSelection = true;
    sections[0].elements[1].button.colorSelection = true;
    sections[0].elements[2].button.colorSelection = true;

    sections[0].elements[0].button.color = glm::vec4(glm::vec3(0.19f, 0.19f, 0.13f), 1.f);
    sections[0].elements[1].button.color = glm::vec4(glm::vec3(0.21f, 0.27f, 0.01f), 1.f);
    sections[0].elements[2].button.color = glm::vec4(glm::vec3(0.27f, 0.22f, 0.15f), 1.f);
    

    sections[sections.size() - 2].elements[0].button.filterSelection = true;
    sections[sections.size() - 2].elements[0].button.selectionDialogTxtrRes = 128;
    sections[sections.size() - 2].elements[1].button.textureSelection3D = true;
    sections[sections.size() - 2].elements[1].button.selectionDialogTxtrRes = 128;
    
    sections[sections.size() - 1].elements[4].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[5].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[6].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[7].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[8].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[9].checkBox.clickState1 = true;

    sections[sections.size() - 1].elements[10].button.color = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.color2 = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.textColor = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.textColor2 = glm::vec4(0.f);
    
    return sections;
}

std::vector<Section> MaterialModifier::createRustModifier(){
    unsigned char whitePixel[] = { 255, 255, 255, 255 }; // 1 pixel, RGBA format (white)

    std::vector<Section> sections =  
    {
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Colors"),
            {
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Color 1", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Color 2", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Color 3", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Color 4", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Properties"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Scale", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 100.f, 30.f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Perlin Properties"),
            {
                RangeBar(ELEMENT_STYLE_STYLIZED, glm::vec2(1, 1.5f), "First Octave", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0, 8, 3 ), 
                RangeBar(ELEMENT_STYLE_STYLIZED, glm::vec2(1, 1.5f), "Octaves", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0, 8, 8 ), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Persistence", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 0.6f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "FBM Properties"),
            {
                RangeBar(ELEMENT_STYLE_STYLIZED, glm::vec2(1, 1.5f), "FBM Octaves", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0, 16, 16 ), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "FBM Roughness", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 0.5f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Rust Properties"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Rust Radius", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 10.f, 2.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Battering Strength", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 2.f, 1.f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Noise Properties"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Noise Strength", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 2.f, 1.f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Element Properties"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Wetness", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Metallic", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 0.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Height", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 0.f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Channel Opacities"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Albedo Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Roughness Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Metallic Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Normal Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Height Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Ambient Occlusion Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Depth Masking & Depth Properties"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1.f, 2.f), "Depth Value",  Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Blur the Height Map", MATERIAL_MODIFIERS_ELEMENT_OFFSET)
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Image Filter"),
            {
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f), "Albedo Filter", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f), "Albedo Filter Mask", Texture(whitePixel, 1, 1, GL_NEAREST, 24), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.f), "Remove The Filter", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false)
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Ambient Occlusion (Physically Based)"),
            {
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Calculate Physically Based Ambient Occlusion", MATERIAL_MODIFIERS_ELEMENT_OFFSET),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Use Previous Ambient Occlusion ", MATERIAL_MODIFIERS_ELEMENT_OFFSET),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Offset", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 2.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Smoothness", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 5.f, 2.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+X Direction", 1.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-X Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+Y Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-Y Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+Z Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-Z Direction", 0.f),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.f), "", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false)
            }
        )
    };
    
    sections[0].elements[0].button.colorSelection = true;
    sections[0].elements[1].button.colorSelection = true;
    sections[0].elements[2].button.colorSelection = true;
    sections[0].elements[3].button.colorSelection = true;

    sections[0].elements[0].button.color = glm::vec4(glm::vec3(0.65f, 0.65f, 0.61f), 1.f);
    sections[0].elements[1].button.color = glm::vec4(glm::vec3(0.43f, 0.4f, 0.39f), 1.f);
    sections[0].elements[2].button.color = glm::vec4(glm::vec3(0.29f, 0.22f, 0.17f), 1.f);
    sections[0].elements[3].button.color = glm::vec4(glm::vec3(0.49f, 0.3f, 0.2f), 1.f);
    

    sections[sections.size() - 2].elements[0].button.filterSelection = true;
    sections[sections.size() - 2].elements[0].button.selectionDialogTxtrRes = 128;
    sections[sections.size() - 2].elements[1].button.textureSelection3D = true;
    sections[sections.size() - 2].elements[1].button.selectionDialogTxtrRes = 128;
    
    sections[sections.size() - 1].elements[4].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[5].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[6].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[7].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[8].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[9].checkBox.clickState1 = true;

    sections[sections.size() - 1].elements[10].button.color = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.color2 = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.textColor = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.textColor2 = glm::vec4(0.f);
    
    return sections;
}

std::vector<Section> MaterialModifier::createSkinModifier(){
    unsigned char whitePixel[] = { 255, 255, 255, 255 }; // 1 pixel, RGBA format (white)

    std::vector<Section> sections =  
    {
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Droplets"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Droplets Count", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 100.f, 21.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Droplets Opacity Jitter", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 0.5f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Droplets Size", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 10.f, 10.f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Veins"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Veins Scale", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 10.f, 1.24f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Veins Strength", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 3.f, 1.68f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Blushing"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Blushing Strength", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 10.f, 3.f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Skin Prints"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Skin Prints Scale", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 100.f, 10.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Skin Prints Strength", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 2.f, 1.f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Noise"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Noise Strength", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 10.f, 4.f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Skin Properties"),
            {
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Skin Color", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Vein Color", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Skin Scale", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 100.f, 10.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Skin Wetness", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Skin Metallic", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 0.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Skin Height", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 2.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Skin Ambient Occlusion", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Channel Opacities"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Albedo Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Roughness Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Metallic Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Normal Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Height Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Ambient Occlusion Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Depth Masking & Depth Properties"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1.f, 2.f), "Depth Value",  Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Blur the Height Map", MATERIAL_MODIFIERS_ELEMENT_OFFSET)
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Image Filter"),
            {
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f), "Albedo Filter", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f), "Albedo Filter Mask", Texture(whitePixel, 1, 1, GL_NEAREST, 24), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.f), "Remove The Filter", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false)
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Ambient Occlusion (Physically Based)"),
            {
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Calculate Physically Based Ambient Occlusion", MATERIAL_MODIFIERS_ELEMENT_OFFSET),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Use Previous Ambient Occlusion ", MATERIAL_MODIFIERS_ELEMENT_OFFSET),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Offset", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 2.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Smoothness", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 5.f, 2.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+X Direction", 1.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-X Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+Y Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-Y Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+Z Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-Z Direction", 0.f),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.f), "", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false)
            }
        )
    };

    sections[5].elements[0].button.colorSelection = true;
    sections[5].elements[1].button.colorSelection = true;

    sections[5].elements[0].button.color = glm::vec4(glm::vec3(0.96, 0.76, 0.61), 1.);
    sections[5].elements[1].button.color = glm::vec4(glm::vec3(0.9, 0.1, 0.01), 1.);
    
    sections[sections.size() - 2].elements[0].button.filterSelection = true;
    sections[sections.size() - 2].elements[0].button.selectionDialogTxtrRes = 128;
    sections[sections.size() - 2].elements[1].button.textureSelection3D = true;
    sections[sections.size() - 2].elements[1].button.selectionDialogTxtrRes = 128;
    
    sections[sections.size() - 1].elements[4].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[5].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[6].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[7].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[8].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[9].checkBox.clickState1 = true;

    sections[sections.size() - 1].elements[10].button.color = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.color2 = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.textColor = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.textColor2 = glm::vec4(0.f);
    
    return sections;
}

std::vector<Section> MaterialModifier::createWoodenModifier(){
    unsigned char whitePixel[] = { 255, 255, 255, 255 }; // 1 pixel, RGBA format (white)

    std::vector<Section> sections =  
    {
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Colors"),
            {
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Color 1", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Color 2", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Color 3", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Properties"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Scale", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 10.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Noise Offset", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 10.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Noise Seed", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 10000.f, 3000.f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Perlin Properties"),
            {
                RangeBar(ELEMENT_STYLE_STYLIZED, glm::vec2(1, 1.5f), "Max Octaves", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0, 8, 8 ), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Persistance", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 0.5f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Musgrave Properties"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Musgrave Lacunarity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 10.f, 2.5f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Musgrave Strength", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 2.f, 0.75f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Musgrave Noise", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 10.f, 1.f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Base"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Noise Strength", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 10.f, 4.6f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Color Saturation", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 2.f, 1.f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "FBM Properties"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "FBM Frequency", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 10.f, 1.f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Element Properties"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Wood Gamma", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 0.6f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Shininess", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 2.f, 0.5f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Metallic", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 0.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Height", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 0.f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Channel Opacities"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Albedo Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Roughness Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Metallic Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Normal Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Height Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Ambient Occlusion Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Depth Masking & Depth Properties"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1.f, 2.f), "Depth Value",  Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Blur the Height Map", MATERIAL_MODIFIERS_ELEMENT_OFFSET)
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Image Filter"),
            {
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f), "Albedo Filter", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f), "Albedo Filter Mask", Texture(whitePixel, 1, 1, GL_NEAREST, 24), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.f), "Remove The Filter", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false)
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Ambient Occlusion (Physically Based)"),
            {
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Calculate Physically Based Ambient Occlusion", MATERIAL_MODIFIERS_ELEMENT_OFFSET),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Use Previous Ambient Occlusion ", MATERIAL_MODIFIERS_ELEMENT_OFFSET),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Offset", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 2.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Smoothness", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 5.f, 2.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+X Direction", 1.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-X Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+Y Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-Y Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+Z Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-Z Direction", 0.f),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.f), "", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false)
            }
        )
    };

    sections[0].elements[0].button.colorSelection = true;
    sections[0].elements[1].button.colorSelection = true;
    sections[0].elements[2].button.colorSelection = true;

    sections[0].elements[0].button.color = glm::vec4(glm::vec3(0.f), 1.f);
    sections[0].elements[1].button.color = glm::vec4(glm::vec3(0.25f, 0.11f, 0.04f), 1.f);
    sections[0].elements[2].button.color = glm::vec4(glm::vec3(0.52f, 0.32f, 0.19f), 1.f);


    sections[sections.size() - 2].elements[0].button.filterSelection = true;
    sections[sections.size() - 2].elements[0].button.selectionDialogTxtrRes = 128;
    sections[sections.size() - 2].elements[1].button.textureSelection3D = true;
    sections[sections.size() - 2].elements[1].button.selectionDialogTxtrRes = 128;
    
    sections[sections.size() - 1].elements[4].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[5].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[6].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[7].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[8].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[9].checkBox.clickState1 = true;

    sections[sections.size() - 1].elements[10].button.color = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.color2 = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.textColor = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.textColor2 = glm::vec4(0.f);
    
    return sections;
}

std::vector<Section> MaterialModifier::createAsphaltModifier(){
    unsigned char whitePixel[] = { 255, 255, 255, 255 }; // 1 pixel, RGBA format (white)

    std::vector<Section> sections =  
    {
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Color"),
            {
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Color1",              Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,false),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Color2",              Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,false),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Dirt"),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Scale",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,0.f, 10.f,4.f), 
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Strength",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,0.f,2.f,1.f), 
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "2nd Color"),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Noise Scale",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,0.f,10.f,5.f), 
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Noise Strength",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,0.f,2.f,1.f), 
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Noise"),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Strength",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,0.f,2.f,1.f), 
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Element Properties"),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Wetness",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,0.f,1.f,1.f), 
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Metallic",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,0.f,1.f,0.f), 
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Height",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,0.f,1.f,1.f), 
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Perlin Properties"),
            {
                RangeBar(ELEMENT_STYLE_STYLIZED,glm::vec2(1,1.5f),"First Octave",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0, 8, 3 ),
                RangeBar(ELEMENT_STYLE_STYLIZED,glm::vec2(1,1.5f),"Octaves",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0, 8, 8 ), 
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Persistence",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 0.7f), 
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Channel Opacities"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Albedo Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Roughness Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Metallic Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Normal Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Height Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Ambient Occlusion Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Depth Masking & Depth Properties"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1.f, 2.f), "Depth Value",  Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Blur the Height Map", MATERIAL_MODIFIERS_ELEMENT_OFFSET)
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Image Filter"),
            {
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f), "Albedo Filter", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f), "Albedo Filter Mask", Texture(whitePixel, 1, 1, GL_NEAREST, 24), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.f), "Remove The Filter", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false)
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Ambient Occlusion (Physically Based)"),
            {
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Calculate Physically Based Ambient Occlusion", MATERIAL_MODIFIERS_ELEMENT_OFFSET),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Use Previous Ambient Occlusion ", MATERIAL_MODIFIERS_ELEMENT_OFFSET),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Offset", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 2.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Smoothness", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 5.f, 2.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+X Direction", 1.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-X Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+Y Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-Y Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+Z Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-Z Direction", 0.f),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.f), "", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false)
            }
        )
    };
    sections[0].elements[0].button.colorSelection = true;
    sections[0].elements[1].button.colorSelection = true;
    
    sections[0].elements[0].button.color = glm::vec4(glm::vec3(0.25f), 1.f);
    sections[0].elements[1].button.color = glm::vec4(glm::vec3(0.23f, 0.25f, 0.27f), 1.f);
    
    sections[sections.size() - 2].elements[0].button.filterSelection = true;
    sections[sections.size() - 2].elements[0].button.selectionDialogTxtrRes = 128;
    sections[sections.size() - 2].elements[1].button.textureSelection3D = true;
    sections[sections.size() - 2].elements[1].button.selectionDialogTxtrRes = 128;
    
    sections[sections.size() - 1].elements[4].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[5].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[6].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[7].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[8].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[9].checkBox.clickState1 = true;

    sections[sections.size() - 1].elements[10].button.color = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.color2 = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.textColor = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.textColor2 = glm::vec4(0.f);
    
    return sections;
}

std::vector<Section> MaterialModifier::createDustModifier(){
    unsigned char whitePixel[] = { 255, 255, 255, 255 }; // 1 pixel, RGBA format (white)

    std::vector<Section> sections =  
    {
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Noise"),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Size",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,0.f, 5.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Offset Intensity",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,0.f, 5.f, 5.f), 
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Rotation",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,0.f, 360.f, 360.f), 
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Brightness",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,0.f, 1.f, 0.17f), 
            }
        ),
        
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Blur"),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Intensity",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,0.f, 1.f, 0.f), 
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Direction",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,0.f,360.f,0.f), 
            }
        ),
        
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Scratches"),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Wavyness",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,0.f, 1.f, 0.1f), 
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Scale",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,0.f, 40.f, 20.f), 
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Base Frequency",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 0.5f), 
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"FrequencyStep",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,0.f, 1.f, 0.25f), 
            }
        ),

        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Droplets"),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Count",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,0.f, 1.f, 0.1f), 
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"OpacityJitter",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Size",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,0.f, 10.f, 5.f), 
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Colors"),
            {
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Color1", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,false),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Color2", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,false),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Color3", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,false),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Element Properties"),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Wetness",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Metallic",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Height",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,0.f, 1.f, 1.f), 
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Channel Opacities"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Albedo Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Roughness Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Metallic Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Normal Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Height Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Ambient Occlusion Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Depth Masking & Depth Properties"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1.f, 2.f), "Depth Value",  Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Blur the Height Map", MATERIAL_MODIFIERS_ELEMENT_OFFSET)
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Image Filter"),
            {
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f), "Albedo Filter", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f), "Albedo Filter Mask", Texture(whitePixel, 1, 1, GL_NEAREST, 24), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.f), "Remove The Filter", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false)
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Ambient Occlusion (Physically Based)"),
            {
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Calculate Physically Based Ambient Occlusion", MATERIAL_MODIFIERS_ELEMENT_OFFSET),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Use Previous Ambient Occlusion ", MATERIAL_MODIFIERS_ELEMENT_OFFSET),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Offset", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 2.f, 1.f),
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Smoothness", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 5.f, 2.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+X Direction", 1.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-X Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+Y Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-Y Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "+Z Direction", 0.f),
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "-Z Direction", 0.f),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.f), "", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false)
            }
        )
    };


    sections[4].elements[0].button.colorSelection = true;
    sections[4].elements[1].button.colorSelection = true;
    sections[4].elements[2].button.colorSelection = true;

    sections[4].elements[0].button.color = glm::vec4(glm::vec3(1.f), 1.f);
    sections[4].elements[1].button.color = glm::vec4(glm::vec3(0.f), 1.f);
    sections[4].elements[2].button.color = glm::vec4(glm::vec3(0.f), 1.f);

    sections[sections.size() - 2].elements[0].button.filterSelection = true;
    sections[sections.size() - 2].elements[0].button.selectionDialogTxtrRes = 128;
    sections[sections.size() - 2].elements[1].button.textureSelection3D = true;
    sections[sections.size() - 2].elements[1].button.selectionDialogTxtrRes = 128;
    
    sections[sections.size() - 1].elements[4].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[5].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[6].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[7].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[8].checkBox.clickState1 = true;
    sections[sections.size() - 1].elements[9].checkBox.clickState1 = true;

    sections[sections.size() - 1].elements[10].button.color = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.color2 = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.textColor = glm::vec4(0.f);
    sections[sections.size() - 1].elements[10].button.textColor2 = glm::vec4(0.f);
    
    return sections;
}

std::vector<Section> MaterialModifier::createMathModifier(){
    unsigned char whitePixel[] = { 255, 255, 255, 255 }; // 1 pixel, RGBA format (white)

    std::vector<Section> sections =  
    {
        Section(
            SectionHolder(ColorPalette::secondColor, MATERIAL_MODIFIERS_ELEMENT_OFFSET, "Math"),
            {
                Element(ComboBox(ELEMENT_STYLE_BASIC,glm::vec2(4,1.5f),
                                                                    {
                                                                        "Add", 
                                                                        "Subtract", 
                                                                        "Multiply", 
                                                                        "Divide", 
                                                                        "Power", 
                                                                        "Logarithm", 
                                                                        "Min", 
                                                                        "Max", 
                                                                        "Sqrt", 
                                                                        "Exponent", 
                                                                        "Sin", 
                                                                        "Cos", 
                                                                        "Tan", 
                                                                        "Cross", 
                                                                        "Dot", 
                                                                        "Distance", 
                                                                    },"File Format", 4.f)),

                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.f), "Left Side : From The Source", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.f), "Right Side : From The Slide Bar", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),"Right Side Value",Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET,-10.f, 10.f, 0.f), 
                CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(1.f, 2.f), "Use Texture", MATERIAL_MODIFIERS_ELEMENT_OFFSET),
                Button(ELEMENT_STYLE_SOLID,glm::vec2(1, 4.f), "Right Side Texture", Texture(), MATERIAL_MODIFIERS_ELEMENT_OFFSET, false),
            }
        ),
        Section(
            SectionHolder(ColorPalette::secondColor, 4.f, "Channel Opacities"),
            {
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Albedo Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Roughness Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Metallic Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Normal Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Height Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
                RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(1, 1.5f), "Ambient Occlusion Opacity", Texture(),MATERIAL_MODIFIERS_ELEMENT_OFFSET, 0.f, 1.f, 1.f), 
            }
        )
    };

    sections[0].elements[1].button.color2 = sections[0].elements[1].button.color;
    sections[0].elements[2].button.color2 = sections[0].elements[2].button.color;
    sections[0].elements[5].button.textureSelection3D = true;
    sections[0].elements[5].button.selectionDialogTxtrRes = 128;
    
    return sections;
}