
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 Copyright (c) 2022-2023, Mert Tetik

 All rights reserved.

 Official GitHub Link : https:github.com/mert-tetik/LigidPainter
 Official Web Page : https:ligidtools.com/ligidpainter

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

#include "tinyfiledialogs.h"

ObjectTexturingDialog::ObjectTexturingDialog(){
    //Create the panel
    this->panel = Panel(
        {
            {
                Section(
                    Element(Button()),
                    {
                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,40.f), ""  , Texture(), 1.f, false)),
                    }
                )
            }
        },
        glm::vec2(40.f),
        glm::vec3(50.f,50.f,0.8f),
        glm::vec4(0.),
        glm::vec4(0.),
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

    this->elementSelectionPanel = Panel(
        {
            {
                Section()
            }
        },
        glm::vec2(40.f),
        glm::vec3(50.f,50.f,0.8f),
        glm::vec4(0.),
        glm::vec4(0.),
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

    this->maskViaFaceSelection = Button(ELEMENT_STYLE_BASIC, glm::vec2(6, 2.f), "Mask via face selection", Texture(), 1.f, false);
    this->maskViaTexture = Button(ELEMENT_STYLE_BASIC, glm::vec2(6, 2.f), "Mask via texture", Texture(), 1.f, false);
    
    this->assignRelatedTexturesButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(6, 2.f), "Assign to related textures", Texture(), 1.f, false);
    this->createNewTextureButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(6, 2.f), "Create new texture and assign", Texture(), 1.f, false);
    this->assignToCustomTexturesButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(6, 2.f), "Assign to custom textures", Texture(), 1.f, false);
    
    this->materialDisplayerButton = Button(ELEMENT_STYLE_SOLID, glm::vec2(6, 4.f), "", Texture(), 1.f, false);
    this->editMaterialButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(6, 2.f), "Edit material", Texture(), 1.f, false);
    this->selectMaterialButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(2.f, 4.f), "", Settings::appTextures().arrowB, 1.f, false);
}

void ObjectTexturingDialog::render(Timer timer){
    dialogControl.updateStart();

    // Modifying the elements
    this->panel.sections[0].elements[0].button.texture = getModel()->displayingTxtr;
    this->panel.sections[0].elements[0].button.scale = this->panel.scale;

    this->maskViaFaceSelection.pos = this->panel.pos;
    this->maskViaFaceSelection.pos.x -= this->panel.scale.x - this->maskViaFaceSelection.scale.x - 2.f;
    this->maskViaFaceSelection.pos.y += 15.f;
    this->maskViaTexture.pos = this->maskViaFaceSelection.pos;
    this->maskViaTexture.pos.y += this->maskViaTexture.scale.y + this->maskViaFaceSelection.scale.y;

    this->assignRelatedTexturesButton.pos = maskViaTexture.pos;
    this->assignRelatedTexturesButton.pos.y += assignRelatedTexturesButton.scale.y + maskViaTexture.scale.y * 2.f;
    this->createNewTextureButton.pos = assignRelatedTexturesButton.pos;
    this->createNewTextureButton.pos.y += createNewTextureButton.scale.y + assignRelatedTexturesButton.scale.y;
    this->assignToCustomTexturesButton.pos = createNewTextureButton.pos;
    this->assignToCustomTexturesButton.pos.y += assignToCustomTexturesButton.scale.y + createNewTextureButton.scale.y;

    this->materialDisplayerButton.pos = this->panel.pos;
    this->materialDisplayerButton.pos.x += this->panel.scale.x - this->materialDisplayerButton.scale.x - 6.f;
    this->materialDisplayerButton.pos.y -= 25.f;
    this->editMaterialButton.pos = this->materialDisplayerButton.pos;
    this->editMaterialButton.pos.y += this->editMaterialButton.scale.y + this->materialDisplayerButton.scale.y;
    this->selectMaterialButton.pos = this->materialDisplayerButton.pos;
    this->selectMaterialButton.pos.x += this->selectMaterialButton.scale.x + this->materialDisplayerButton.scale.x;

    //Render the panel
    this->panel.render(timer, false);

    // Render the elements
    this->maskViaFaceSelection.render(timer, !this->materialSelection && !this->textureSelection);
    this->maskViaTexture.render(timer, !this->materialSelection && !this->textureSelection);
    this->assignRelatedTexturesButton.render(timer, !this->materialSelection && !this->textureSelection);
    this->createNewTextureButton.render(timer, !this->materialSelection && !this->textureSelection);
    this->assignToCustomTexturesButton.render(timer, !this->materialSelection && !this->textureSelection);
    this->materialDisplayerButton.render(timer, !this->materialSelection && !this->textureSelection);
    this->editMaterialButton.render(timer, !this->materialSelection && !this->textureSelection);
    this->selectMaterialButton.render(timer, !this->materialSelection && !this->textureSelection);

    // Element interactions
    if(this->selectMaterialButton.clicked){
        this->materialSelection = true;
    }
    else if(this->assignToCustomTexturesButton.clicked){
        this->textureSelection = true;
    }

    if(!this->panel.hover && *Mouse::LClick() || getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) && !this->materialSelection)
        this->dialogControl.unActivate();

    dialogControl.updateEnd(timer,0.15f);
}