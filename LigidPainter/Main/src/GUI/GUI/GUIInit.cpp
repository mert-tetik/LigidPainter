/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Init the GUI class

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
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>


void UI::init
            (  
                Painter& painter
            ){

    
 


    //Init the Painting modes panel
    vectorPaintingModePropertyPanel = Panel(
                            {
                                Section(
                                    Element(Button()),
                                    {   
                                        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(2,1.5f), {"Drawing", "Softening", "Smearing", "Normal", "Filtering"}, "Stroke Painting Mode", 1.f),
                                        Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,1.5f), "Add Point Between The Selected Points", Texture(), 0.5f, false),
                                        Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,1.5f), "Clear All", Texture(), 0.5f, false),
                                        Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,1.5f), "Remove The Selected PointS (CTRL + X)", Texture(), 0.5f, false),
                                        Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,1.5f), "Stroke", Texture(), 0.5f, false)
                                    }
                                )
                            },
                            
                            glm::vec2(7.f, 9.f), //Initial scale value
                            glm::vec3(50 ,50 ,0.1f),  //Initial position value
                            glm::vec4(ColorPalette::secondColor.r, ColorPalette::secondColor.g, ColorPalette::secondColor.b, ColorPalette::secondColor.a/2.f), //Color of the panel
                            ColorPalette::thirdColor, //Color of the panel
                            true,
                            true,
                            false,
                            true,
                            true,
                            1.f,
                            1,
                            {},
                            20.f,
                            false
                        );

    vectorPaintingMode2DModeWrapCheckBox = CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(4,2), "Paint in wrap mode", 1.f); //
}   