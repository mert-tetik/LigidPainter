
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

#include "3D/ThreeD.hpp" 
#include "GUI/GUI.hpp" 
#include "NodeSystem/Node/Node.hpp" 
#include "MouseSystem/Mouse.hpp"
#include "SettingsSystem/Settings.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

MaterialDisplayerDialog::MaterialDisplayerDialog(){
    
    //Create the panel
    this->panel = Panel(
                            
                            {
                                Section(
                                    Element(Button()),
                                    {
                                        Element(Button(ELEMENT_STYLE_BASIC, glm::vec2(4,2), "Material Displayer", Texture(),0.f,false)), 
                                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(20, 20), "", this->material.displayingTexture, 0.f, false)), 
                                    }
                                )
                            },
                            glm::vec2(22),
                            glm::vec3(50.f,50.f,0.8f),
                            ColorPalette::mainColor,
                            ColorPalette::thirdColor,
                            true,
                            true,
                            true,
                            true,
                            true,
                            1.f,
                            1,
                            {},
                            0.25f,
                            false
                        );

    this->displayingCam.cameraPos = glm::vec3(0,0,-3.5f);
    this->displayingCam.radius = 3.5f;
}

void MaterialDisplayerDialog::render(Timer timer){
    
    dialogControl.updateStart();

    //Render the panel
    this->panel.sections[0].elements[1].button.texture = this->material.displayingTexture;
    panel.render(timer,true);
     
    //Close the dialog
    if(getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || (!panel.hover && *Mouse::LClick()) || (panel.sections[0].elements[0].button.hover && *Mouse::LDoubleClick())){
        if(!dialogControl.firstFrameActivated)
            this->dialogControl.unActivate();
    }

    this->material.updateMaterialDisplayingTexture(512, false, this->displayingCam, 0, true);
    
    dialogControl.updateEnd(timer,0.15f);
}