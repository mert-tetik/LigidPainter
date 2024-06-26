/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "UTIL/Util.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "UTIL/GL/GL.hpp"
#include "UTIL/Painting/Painter.hpp"

#include "GUI/GUI.hpp"
#include "GUI/Panels.hpp"

#include "3D/ThreeD.hpp"

#include "Toolkit/VectorScene/VectorScene.hpp"

#include <string>
#include <vector>

Panel dialog_panel;

static void init_GUI(){
    dialog_panel = Panel(
                            {
                                Section(
                                    Element(Button()),
                                    {   
                                        ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(2,1.5f), {"Drawing", "Softening", "Smearing", "Normal", "Filtering"}, "Stroke Painting Mode", 3.f),
                                        CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(4,2), "Paint in wrap mode", 1.f),
                                        Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,1.5f), "Stroke", Texture(), 4.f, false),
                                        Button(ELEMENT_STYLE_STYLIZED,glm::vec2(2,1.5f), "Cancel", Texture(), 0.5f, false)
                                    }
                                )
                            },
                            
                            glm::vec2(13.f, 12.f), //Initial scale value
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
}

static DialogControl dialog_control;

void VectorScene::StrokeDialog::show_stroke_dialog(bool threeD, VectorScene* vector_scene){
    
    if(vector_scene == nullptr)
        return;

    if(!dialog_panel.sections.size()){
        init_GUI();
    }
    
    dialog_control.activate();
    
    while (!getContext()->window.shouldClose())
    {
        /* code */
        dialog_control.updateStart(true);

        dialog_panel.render(*getTimer(), !dialog_log.isHovered());

        this->paint_mode = dialog_panel.sections[0].elements[0].comboBox.selectedIndex;
        if(threeD){
            dialog_panel.sections[0].elements[1].scale.y = 0.f;
            this->twoD_wrap_mode = false;
        }
        else{
            dialog_panel.sections[0].elements[1].scale.y = 2.f;
            this->twoD_wrap_mode = dialog_panel.sections[0].elements[1].checkBox.clickState1;
        }

        // Calculate dialog panel's height
        dialog_panel.scale.y = 0.f;
        for(Element el : dialog_panel.sections[0].elements){
            dialog_panel.scale.y += el.scale.y;
            dialog_panel.scale.y += el.panelOffset / 2.f;
        }
        dialog_panel.scale.y += 1.f;

        if(dialog_panel.sections[0].elements[2].button.clicked)
        {
            bool success;
            PaintSettings paint_settings = get_paint_settings_using_GUI_data(&success);
            paint_settings.painting_mode = paint_mode;
            if(success){
                vector_scene->apply_strokes(
                                            threeD,
                                            this->twoD_wrap_mode,
                                            paint_settings
                                        );
            }
        }

        if((!dialog_panel.hover && !dialog_log.isHovered() && *Mouse::LClick()) || getContext()->window.isKeyClicked(LIGIDGL_KEY_ESCAPE) || dialog_panel.sections[0].elements[3].button.clicked || dialog_panel.sections[0].elements[2].button.clicked){
            dialog_control.unActivate();
        }

        dialog_control.updateEnd(*getTimer(), 0.15f);
        if(dialog_control.mixVal == 0.f)
            break;
    }
}

void VectorScene::StrokeDialog::set_properties(int paint_mode, bool twoD_wrap_mode){
    this->paint_mode = paint_mode;
    this->twoD_wrap_mode = twoD_wrap_mode;

    if(!dialog_panel.sections.size()){
        init_GUI();
    }

    dialog_panel.sections[0].elements[0].comboBox.selectedIndex = paint_mode;
    dialog_panel.sections[0].elements[1].checkBox.clickState1 = twoD_wrap_mode;
}