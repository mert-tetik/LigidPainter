/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

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

#include <string>
#include <iostream>
#include <vector>

MaterialEditorDialog::MaterialEditorDialog(){}

MaterialEditorDialog::MaterialEditorDialog
                                        (
                                            ColorPalette colorPalette
                                        )
{
    this->displayerCamera.cameraPos = glm::vec3(0,0,-7.f);
    this->displayerCamera.radius = 7.f;
    this->colorPalette = colorPalette; 

    this->displayModeComboBox = ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(7.f,2.f), colorPalette, {"PBR", "Albedo", "Roughness", "Metallic", "Normal Map", "Height", "Ambient Occlusion"}, "Displaying Mode", 0.f);
    
    //Back side of the dialog
    bgPanel = Panel
    (
        colorPalette,
        {
        },
        scale,
        pos,
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
        10.f,
        false
    );

    //That panel where you add the modifiers (on the left side)
    layerPanel = Panel(colorPalette,{},scaleLayer,posLayer,colorPalette.mainColor,colorPalette.thirdColor,true,true,false,true,true,1.f,1.f,                                
                            {
                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),colorPalette, "Add"        , Texture(), 0.f, false)
                            },
                            10.f,
                            true
                      );
    
    //That panel where you can display the properties of the modifier (on the right side)
    modifiersPanel = Panel( colorPalette, {}, scaleModifier, posModifier, colorPalette.mainColor,colorPalette.thirdColor , true, false, true, true, true, 1.f, 1.f,                                
                            {
                            
                            },
                            10.f,
                            true
                      );

    //Material display is a button (is taking the material's display texture as a texture) (right in the middle)
    materialDisplayer = Button(ELEMENT_STYLE_SOLID, glm::vec2(45,45), colorPalette, "Material"        , Settings::appTextures().greetingDialogImage, 0.f, false);
    
    //Init the texture modifier
    appMaterialModifiers.textureModifier = MaterialModifier(colorPalette, TEXTURE_MATERIAL_MODIFIER);
    appMaterialModifiers.textureModifier.sections[0].header.button.clickState1 = true;

    //Init the dust modifier
    appMaterialModifiers.dustModifier = MaterialModifier(colorPalette, DUST_MATERIAL_MODIFIER);
    
    //Init the asphalt modifier
    appMaterialModifiers.asphaltModifier = MaterialModifier(colorPalette, ASPHALT_MATERIAL_MODIFIER);
    
    //Init the fabric modifier
    appMaterialModifiers.fabricModifier = MaterialModifier(colorPalette, FABRIC_MATERIAL_MODIFIER);
    
    //Init the moss modifier
    appMaterialModifiers.mossModifier= MaterialModifier(colorPalette, MOSS_MATERIAL_MODIFIER);
    
    //Init the rust modifier
    appMaterialModifiers.rustModifier = MaterialModifier(colorPalette, RUST_MATERIAL_MODIFIER);
    
    //Init the skin modifier
    appMaterialModifiers.skinModifier = MaterialModifier(colorPalette, SKIN_MATERIAL_MODIFIER);
    
    //Init the solid modifier
    appMaterialModifiers.solidModifier = MaterialModifier(colorPalette, SOLID_MATERIAL_MODIFIER);
    
    //Init the wooden modifier
    appMaterialModifiers.woodenModifier = MaterialModifier(colorPalette, WOODEN_MATERIAL_MODIFIER);
    
    //Bar button
    this->barButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(bgPanel.scale.x, bgPanel.scale.y / 20), colorPalette, "Material Editor", Texture(), 0.f, false);
    barButton.pos = glm::vec3(bgPanel.pos.x, bgPanel.pos.y - bgPanel.scale.y - barButton.scale.y, bgPanel.pos.z);
}