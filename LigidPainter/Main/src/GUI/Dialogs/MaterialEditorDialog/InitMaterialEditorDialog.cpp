/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Constructors of the MatarialEditorDialog class

*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "SettingsSystem/Settings.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

MaterialEditorDialog::MaterialEditorDialog()
{
    //Back side of the dialog
    bgPanel = Panel
    (
        {
        },
        glm::vec2(45.f, 42.f),
        glm::vec3(50.f, 50.f, 0.8f),
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
        10.f,
        false
    );

    //That panel where you add the modifiers (on the left side)
    layerPanel = Panel({Section()}, glm::vec2(7.f, bgPanel.scale.y), glm::vec3(12.f,50.f,0.8f), ColorPalette::mainColor,ColorPalette::thirdColor,true,false,true,true,true,1.f,1.f,                                
                            {
                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),"Add"        , Texture(), 0.f, false)
                            },
                            10.f,
                            true
                      );
    
    //That panel where you can display the properties of the modifier (on the right side)
    modifiersPanel = Panel( {}, glm::vec2(7.f, bgPanel.scale.y), glm::vec3(88.f,50.f,0.8f), ColorPalette::mainColor,ColorPalette::thirdColor , true, false, true, true, true, 1.f, 1.f,                                
                            {
                            
                            },
                            10.f,
                            true
                      );

    shortcutPanel = Panel(   
                            {
                                Section(),
                                Section()
                            }, 
                            glm::vec2(7.f, bgPanel.scale.y), glm::vec3(12.f,50.f,0.8f), ColorPalette::secondColor,ColorPalette::thirdColor , true, true, false, true, true, 1.f, 1.f, {}, 10.f, true);

    navPanel = Panel( {}, glm::vec2(bgPanel.scale.x, 1.6f), glm::vec3(85.f,50.f,0.8f), ColorPalette::secondColor, ColorPalette::thirdColor , true, true, true, false, true, 1.f, 1.f,                                
                            {
                            
                            },
                            10.f,
                            true
                      );
    
    zoomPanel = Panel(   
                            {
                                Section(
                                            Element(),
                                            {
                                                Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), "100", Texture(), 0.f, false),
                                                Button(ELEMENT_STYLE_SOLID, glm::vec2(4.f), "Reset", Texture(), 0.f, false)
                                            }
                                        )
                            }, 
                            glm::vec2(0.f, 2.f), glm::vec3(15.f,50.f,0.9f), ColorPalette::secondColor,ColorPalette::thirdColor , false, true, false, true, true, 1.f, 1.f, {}, 10.f, true);
    
    navPanel.color.a = 1.f;
    
    bgPanel.solidStyle = true;
    layerPanel.solidStyle = true;
    modifiersPanel.solidStyle = true;
    shortcutPanel.solidStyle = true;
    navPanel.solidStyle = true;

    this->displayerCamera.cameraPos = glm::vec3(0,0,-3.5f);
    this->displayerCamera.radius = 3.5f;

    this->displayModeComboBox = ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(5.5f,1.8f), {"PBR", "Albedo", "Roughness", "Metallic", "Normal Map", "Height", "Ambient Occlusion"}, "Displaying Mode", 0.f);

    //Material display is a button (is taking the material's display texture as a texture) (right in the middle)
    materialDisplayer = Button(ELEMENT_STYLE_SOLID, glm::vec2(45,45), "Material", Texture(), 0.f, false);
    materialDisplayer.color.a = 0.5f;
    
    //Bar button
    this->barButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(bgPanel.scale.x, bgPanel.scale.y / 20), "Material Editor", Texture(), 0.f, false);
    barButton.pos = glm::vec3(bgPanel.pos.x, bgPanel.pos.y - bgPanel.scale.y - barButton.scale.y, bgPanel.pos.z);

    this->displayingFBO = Framebuffer(Texture(nullptr, 1024, 1024, GL_LINEAR), GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, glm::ivec2(1024)));
    this->skyboxFBO = Framebuffer(Texture(nullptr, 1024, 1024, GL_LINEAR), GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, glm::ivec2(1024)));
    

    twoDModelModeBtn = Button(ELEMENT_STYLE_SOLID,glm::vec2(0.8f, Settings::videoScale()->x / Settings::videoScale()->y),"", appTextures.twoDIcon, 1.f,false);
    twoDModelModeBtn.textScale = 0.6f;
    twoDModelModeBtn.outline = false;
    twoDModelModeBtn.outlineExtra = false;
    twoDModelModeBtn.infoText = "2D Displaying Mode";
    twoDModelModeBtn.textureSizeScale = 1.4f;
    matDisplayerBallModeBtn = Button(ELEMENT_STYLE_SOLID,glm::vec2(0.8f, Settings::videoScale()->x / Settings::videoScale()->y),"", appTextures.materialIcon, 1.f,false);
    matDisplayerBallModeBtn.textScale = 0.6f;
    matDisplayerBallModeBtn.outline = false;
    matDisplayerBallModeBtn.outlineExtra = false;
    matDisplayerBallModeBtn.infoText = "Material Displaying Ball Model Mode";
    matDisplayerBallModeBtn.textureSizeScale = 1.4f;
    customModelModeBtn = Button(ELEMENT_STYLE_SOLID,glm::vec2(0.8f, Settings::videoScale()->x / Settings::videoScale()->y),"", appTextures.TDModelIcon, 1.f,false);
    customModelModeBtn.textScale = 0.6f;
    customModelModeBtn.outline = false;
    customModelModeBtn.outlineExtra = false;
    customModelModeBtn.infoText = "Active 3D Model Displaying Mode";
    customModelModeBtn.textureSizeScale = 1.4f;
    
    overallResultMode = Button(ELEMENT_STYLE_SOLID,glm::vec2(0.8f, Settings::videoScale()->x / Settings::videoScale()->y),"", appTextures.stripes1Icon, 1.f,false);
    overallResultMode.textScale = 0.6f;
    overallResultMode.outline = false;
    overallResultMode.outlineExtra = false;
    overallResultMode.infoText = "Overall displaying mode";
    overallResultMode.textureSizeScale = 1.4f;
    selectedModifierResultMode = Button(ELEMENT_STYLE_SOLID,glm::vec2(0.8f, Settings::videoScale()->x / Settings::videoScale()->y),"", appTextures.stripes2Icon, 1.f,false);
    selectedModifierResultMode.textScale = 0.6f;
    selectedModifierResultMode.outline = false;
    selectedModifierResultMode.outlineExtra = false;
    selectedModifierResultMode.infoText = "Selected modifier displaying mode";
    selectedModifierResultMode.textureSizeScale = 1.4f;

    shortcutPanel.sections[0].elements.clear();
    shortcutPanel.sections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), "Quick Modification Panel", Texture(), 0.f, false));
    shortcutPanel.sections[0].elements[shortcutPanel.sections[0].elements.size() - 1].button.color2 = shortcutPanel.sections[0].elements[shortcutPanel.sections[0].elements.size() - 1].button.color; 
    shortcutPanel.sections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(1.f), "CTRL + SHIFT + W + R-CLICK : Add shortcuts", Texture(), 0.f, false));
    shortcutPanel.sections[0].elements[shortcutPanel.sections[0].elements.size() - 1].button.color2 = shortcutPanel.sections[0].elements[shortcutPanel.sections[0].elements.size() - 1].button.color; 
    
    shortcutPanel.sections[0].elements.push_back(Button(ELEMENT_STYLE_STYLIZED, glm::vec2(2.f), "Use Uv For All The Textures", Texture(), 2.f, false));
    shortcutPanel.sections[0].elements.push_back(Button(ELEMENT_STYLE_STYLIZED, glm::vec2(2.f), "Use Model Pos For All The Textures", Texture(), 0.f, false));

    shortcutRenamingTextbox = TextBox(ELEMENT_STYLE_BASIC, glm::vec2(2.f), "", 0.f, false);
    shortcutRenamingTextbox.outlineColor = glm::vec4(0.f);
    shortcutRenamingTextbox.outlineColor2 = glm::vec4(0.f);
}