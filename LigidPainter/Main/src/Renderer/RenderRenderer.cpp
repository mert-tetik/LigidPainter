/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    TODO Remove nullptrs

*/

#include <iostream>
#include <filesystem>

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "Renderer.h"

#include "GUI/Elements.hpp"
#include "GUI/GUI.hpp"
#include "GUI/Panels.hpp"

#include "3D/ThreeD.hpp"

#include "Toolkit/Toolkits.hpp"

#include "UTIL/Util.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/Painting/Painter.hpp"

static bool last_painting_condition = false;

void Renderer::render(){

    Debugger::block("Started"); // Start
    Debugger::block("Started"); // Start

    Debugger::block("Prep"); // Start
    this->start_render();
    Debugger::block("Prep"); // End
    
    Debugger::block("Render : Scene"); // Start
    getScene()->render_scene(timer);
    Debugger::block("Render : Scene"); // End
    //Clear the depth buffer before rendering the UI elements (prevent coliding)
    glClear(GL_DEPTH_BUFFER_BIT);

    //Bind 2D square vertex buffers
    getBox()->bindBuffers();
    
    Debugger::block("Render : Toolkits"); // Start
    render_toolkits_before_panels(timer);
    Debugger::block("Render : Toolkits"); // End
    
    Debugger::block("Render : Panels"); // Start
    panels_render(timer);
    Debugger::block("Render : Panels"); // End
    
    Debugger::block("Render : Toolkits after panels"); // Start
    render_toolkits_after_panels(timer);
    Debugger::block("Render : Toolkits after panels"); // End
    
    Debugger::block("Painting"); // Start
    //Painting
    if(
            painting_paint_condition() || last_painting_condition
        )
    {
        bool success = false;
        Texture albedo = getScene()->get_selected_mesh()->layerScene.get_painting_channels(&success).albedo;  
        Texture roughness = getScene()->get_selected_mesh()->layerScene.get_painting_channels(&success).roughness;  
        Texture metallic = getScene()->get_selected_mesh()->layerScene.get_painting_channels(&success).metallic;  
        Texture normalMap = getScene()->get_selected_mesh()->layerScene.get_painting_channels(&success).normalMap;  
        Texture heightMap = getScene()->get_selected_mesh()->layerScene.get_painting_channels(&success).heightMap;  
        Texture ambientOcclusion = getScene()->get_selected_mesh()->layerScene.get_painting_channels(&success).ambientOcclusion;  

        if(success || panel_displaying_modes.selectedElement != 1)
        painting_paint_buffers(PaintSettings(
                                                PaintSettings::PaintingOverData(
                                                                                    checkComboList_painting_over.panel.sections[0].elements[0].checkBox.clickState1,
                                                                                    checkComboList_painting_over.panel.sections[0].elements[4].checkBox.clickState1,
                                                                                    getTextureFieldScene()
                                                                                ),
                                                PaintSettings::ColorBuffer(
                                                                                checkComboList_painting_color.panel.sections[0].elements[2].painterColorSelection.getSelectedColor(),
                                                                                checkComboList_painting_color.panel.sections[0].elements[4].rangeBar.value,
                                                                                checkComboList_painting_color.panel.sections[0].elements[6].rangeBar.value,
                                                                                checkComboList_painting_color.panel.sections[0].elements[8].rangeBar.value,
                                                                                checkComboList_painting_color.panel.sections[0].elements[10].rangeBar.value,
                                                                                checkComboList_painting_color.panel.sections[0].elements[12].rangeBar.value,
                                                                                checkComboList_painting_color.panel.sections[0].elements[14].button.material,
                                                                                checkComboList_painting_color.panel.sections[0].elements[13].checkBox.clickState1
                                                                            ),
                                                PaintSettings::PaintedBuffers(
                                                                                    panel_displaying_modes.selectedElement == 1,
                                                                                    panel_library_selected_texture,
                                                                                    checkComboList_painting_color.panel.sections[0].elements[1].checkBox.clickState1,
                                                                                    albedo,
                                                                                    checkComboList_painting_color.panel.sections[0].elements[3].checkBox.clickState1,
                                                                                    roughness,
                                                                                    checkComboList_painting_color.panel.sections[0].elements[5].checkBox.clickState1,
                                                                                    metallic,
                                                                                    checkComboList_painting_color.panel.sections[0].elements[7].checkBox.clickState1,
                                                                                    normalMap,
                                                                                    checkComboList_painting_color.panel.sections[0].elements[9].checkBox.clickState1,
                                                                                    heightMap,
                                                                                    checkComboList_painting_color.panel.sections[0].elements[11].checkBox.clickState1,
                                                                                    ambientOcclusion
                                                                                ),
                                                PaintSettings::MirrorSettings(
                                                                                checkComboList_painting_mirror.panel.sections[0].elements[0].checkBox.clickState1,
                                                                                checkComboList_painting_mirror.panel.sections[0].elements[1].rangeBar.value,
                                                                                checkComboList_painting_mirror.panel.sections[0].elements[2].checkBox.clickState1,
                                                                                checkComboList_painting_mirror.panel.sections[0].elements[3].rangeBar.value,
                                                                                checkComboList_painting_mirror.panel.sections[0].elements[4].checkBox.clickState1,
                                                                                checkComboList_painting_mirror.panel.sections[0].elements[5].rangeBar.value
                                                                            ),
                                                PaintSettings::PaintVertexBuffer(
                                                                                    getScene()->get_selected_mesh(),
                                                                                    &twoD_painting_box,
                                                                                    !twoD_painting_mode
                                                                                ),
                                                PaintSettings::PointData(
                                                                            *Mouse::cursorPos(),
                                                                            getScene()->get_selected_mesh()->getCurrentPosNormalDataOverCursor(),
                                                                            checkBox_wrap_mode.clickState1
                                                                        ),
                                                panel_painting_modes.selectedElement,
                                                PaintSettings::DrawMode(
                                                                            Brush(button_painting_brush.brushProperties, "")
                                                                        ),
                                                PaintSettings::SoftenMode(
                                                                            Brush(button_painting_brush.brushProperties, ""),
                                                                            1.f
                                                                        ),
                                                PaintSettings::SmearMode(
                                                                            panel_smear_painting_properties.sections[0].elements[1].rangeBar.value,
                                                                            panel_smear_painting_properties.sections[0].elements[0].rangeBar.value,
                                                                            Brush(button_painting_brush.brushProperties, "")
                                                                        ),
                                                PaintSettings::NormalMode(
                                                                            Brush(button_painting_brush.brushProperties, ""),
                                                                            1.f
                                                                        ),
                                                PaintSettings::FilterMode(
                                                                            Brush(button_painting_brush.brushProperties, ""),
                                                                            button_painting_filter_mode_filter.filter
                                                                        ),
                                                PaintSettings::BucketMode(
                                                                            Brush(button_painting_brush.brushProperties, "")
                                                                        )

                                            ), !last_painting_condition && painting_paint_condition(), last_painting_condition && !painting_paint_condition());
    }

    last_painting_condition = painting_paint_condition();
    Debugger::block("Painting"); // End

    Debugger::block("End Prep"); // Start
    this->end_render();
    Debugger::block("End Prep"); // End
}