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

#include <string>
#include <iostream>
#include <vector>

MaterialEditorDialog::MaterialEditorDialog(){}

MaterialEditorDialog::MaterialEditorDialog
                                        (
                                            Shader buttonShader,
                                            Shader tdModelShader,
                                            ColorPalette colorPalette,
                                            AppTextures appTextures,
                                            Model &sphereModel
                                        )
{
    
    //Give params to the public class member variables
    this->buttonShader = buttonShader; 
    this->tdModelShader = tdModelShader; 
    this->colorPalette = colorPalette; 
    this->sphereModel = sphereModel; 
    
    //Back side of the dialog
    bgPanel = Panel
    (
        buttonShader,
        colorPalette,
        {
            // {
            //     Section(
            //             Element(Button()),
            //             {
            //                 Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(30,30),colorPalette,buttonShader,"Add"        , appTextures.greetingDialogImage, 0.f,false))
            //             }
            //         )
            // }
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
        20.f,
        false
    );

    //That panel where you add the modifiers (on the left side)
    layerPanel = Panel(buttonShader,colorPalette,{},scaleLayer,posLayer,colorPalette.mainColor,colorPalette.thirdColor,true,true,false,true,true,1.f,1.f,                                
                            {
                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),colorPalette, buttonShader, "Add"        , Texture(), 0.f, false),
                                Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1.5f),colorPalette, buttonShader, "Del"        , Texture(), 0.f, false)
                            },
                            20.f,
                            true
                      );
    
    //That panel where you can display the properties of the modifier (on the right side)
    modifiersPanel = Panel(buttonShader, colorPalette, {}, scaleModifier, posModifier, colorPalette.mainColor,colorPalette.thirdColor , true, false, true, true, true, 1.f, 1.f,                                
                            {
                            
                            },
                            20.f,
                            true
                      );

    //Material display is a button (is taking the material's display texture as a texture) (right in the middle)
    materialDisplayer = Button(ELEMENT_STYLE_SOLID, glm::vec2(45,45), colorPalette, buttonShader, "Material"        , appTextures.greetingDialogImage, 0.f, false);
    
    //Init the texture modifier
    appMaterialModifiers.textureModifier = MaterialModifier(colorPalette, buttonShader, appTextures, TEXTURE_MATERIAL_MODIFIER);
    appMaterialModifiers.textureModifier.sections[0].header.button.clickState1 = true;

    //Init the dust modifier
    appMaterialModifiers.dustModifier = MaterialModifier(colorPalette, buttonShader, appTextures, DUST_MATERIAL_MODIFIER);
    
    //Bar button
    this->barButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(bgPanel.scale.x, bgPanel.scale.y / 20), colorPalette, buttonShader, "Material Editor", Texture(), 0.f, false);
    barButton.pos = glm::vec3(bgPanel.pos.x, bgPanel.pos.y - bgPanel.scale.y - barButton.scale.y, bgPanel.pos.z);
}