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
        PaintSettings paint_settings = get_paint_settings_using_GUI_data(&success);

        if(success){
            bool first_frame = !last_painting_condition && painting_paint_condition();
            bool last_frame = last_painting_condition && !painting_paint_condition();

            if(last_frame){
                std::string actionTitle = "Unknown painting mode";
    
                if(paint_settings.painting_mode == 0)
                    actionTitle = "Painting";
                if(paint_settings.painting_mode == 1)
                    actionTitle = "Softening";
                if(paint_settings.painting_mode == 2)
                    actionTitle = "Smearing";
                if(paint_settings.painting_mode == 3)
                    actionTitle = "Normal map painting";
                if(paint_settings.painting_mode == 4)
                    actionTitle = "Filter painting";
                if(paint_settings.painting_mode == 6)
                    actionTitle = "Bucket painting";
                
                if(paint_settings.painted_buffers.material_painting){
                    dialog_log.registerMultiChannelPaintingAction(
                                                                    "Multi-channel painting", 
                                                                    MaterialChannels(
                                                                        paint_settings.painted_buffers.material_channel_albedo,
                                                                        paint_settings.painted_buffers.material_channel_roughness,
                                                                        paint_settings.painted_buffers.material_channel_metallic,
                                                                        paint_settings.painted_buffers.material_channel_normalMap,
                                                                        paint_settings.painted_buffers.material_channel_heightMap,
                                                                        paint_settings.painted_buffers.material_channel_ao
                                                                    )
                                                                );
                }
                else{
                    dialog_log.registerPaintingAction(actionTitle, paint_settings.painted_buffers.solid_painted_texture);
                }
            }

            painting_paint_buffers(paint_settings, first_frame, last_frame);
        }

        if(last_painting_condition && !painting_paint_condition()){

            int layerIndex = -1;
            Layer* layer = getScene()->get_selected_mesh()->layerScene.get_selected_layer(&layerIndex);

            if(layerIndex != -1 && layer->layerType == "painting"){
                Texture* capture_txtr;
                layer->get_type_specific_variable(nullptr, nullptr, nullptr, nullptr, &capture_txtr);
               
                ThreeDPoint point = getScene()->get_selected_mesh()->getCurrentPosNormalDataOverCursor();
                Camera cam = getScene()->camera;
                cam.originPos = point.pos;
                cam.setCameraPosition(point.pos + point.normal);
                cam.updateViewMatrix();
               
                MaterialChannels channels = getScene()->get_selected_mesh()->layerScene.get_painting_channels(&success);
                channels.ambientOcclusion = appTextures.white;
                capture_txtr->render_mesh(*getScene()->get_selected_mesh(), channels, cam);
            }
        }
    }

    last_painting_condition = painting_paint_condition();
    Debugger::block("Painting"); // End

    Debugger::block("End Prep"); // Start
    this->end_render();
    Debugger::block("End Prep"); // End
}