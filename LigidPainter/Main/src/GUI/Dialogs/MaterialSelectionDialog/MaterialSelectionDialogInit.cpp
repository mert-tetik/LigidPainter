
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 (c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
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
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

MaterialSelectionDialog::MaterialSelectionDialog(int){
    this->bgPanel = Panel(
                            
                            {
                                Section(
                                    Element(Button()),
                                    {
                                    }
                                )
                            },
                            glm::vec2(38.f, 40.f),
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
    this->matModePanel = Panel(
                            
                            {
                                Section(
                                    Element(Button()),
                                    {
                                    }
                                )
                            },
                            glm::vec2(5.f, 40.f),
                            glm::vec3(bgPanel.pos.x - bgPanel.scale.x + 5.f, 50.f,0.8f),
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
    this->matDisplayerPanel = Panel(
                            
                            {
                                Section(
                                    Element(Button()),
                                    {

                                    }
                                )
                            },
                            glm::vec2(22.f, 40.f),
                            glm::vec3(matModePanel.pos.x + matModePanel.scale.x + 22.f, 50.f, 0.8f),
                            ColorPalette::secondColor,
                            ColorPalette::thirdColor,
                            true,
                            true,
                            true,
                            true,
                            true,
                            1.f,
                            5,
                            {},
                            0.25f,
                            false
                        );

    this->selectedMatPanel = Panel(
                            
                            {
                                Section(
                                    Element(Button()),
                                    {
                                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(20.f), "", Texture(), 5.f, false)), 
                                        Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(10, 2), "Unselected", Texture(), 0.f, false)), 
                                        Element(CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(10, 2), "Quick Displaying", 1.f)), 
                                        Element(Button(ELEMENT_STYLE_STYLIZED, glm::vec2(10, 2), "Edit The Active Material", Texture(), 8.f, false)), 
                                        Element(Button(ELEMENT_STYLE_STYLIZED, glm::vec2(10, 2), "Add Active Material To Library", Texture(), 2.f, false)),
                                        Element(Button(ELEMENT_STYLE_STYLIZED, glm::vec2(10, 2), "Select The Active Material", Texture(), 2.f, false))
                                    }
                                )
                            },
                            glm::vec2((bgPanel.scale.x - matDisplayerPanel.scale.x - matModePanel.scale.x), 40.f),
                            glm::vec3(matDisplayerPanel.pos.x + matDisplayerPanel.scale.x + (bgPanel.scale.x - matDisplayerPanel.scale.x - matModePanel.scale.x), 50.f, 0.8f),
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
    

    this->displayingCam.cameraPos = glm::vec3(0, 0, -3.5f);
    this->displayingCam.radius = 3.5f;

    matDisplayerPanel.solidStyle = true;
    
    this->displayingTexture = Texture((char*)nullptr, 512, 512);
    
    this->selectedMatPanel.sections[0].elements[0].button.color2 = this->selectedMatPanel.sections[0].elements[0].button.color;
    this->selectedMatPanel.sections[0].elements[1].button.color2 = this->selectedMatPanel.sections[0].elements[1].button.color;
}