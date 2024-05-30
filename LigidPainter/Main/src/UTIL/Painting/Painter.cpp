/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

TODO : Update color buffer resolutions
TODO : Refresh painting buffers

*/

//OpenGL & window 
#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

//OpenGL Math Library GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector> //std::vector
#include <string> //std::string
#include <map> //std::map
#include <atomic>

#include "UTIL/Util.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/Painting/Painter.hpp"
#include "UTIL/Painting/Painting_UTIL.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Threads/Threads.hpp"

#include "3D/Material/Material.hpp"

#include "GUI/Panels.hpp"

extern bool updateThePreRenderedPanels;

Framebuffer painting_projected_painting_FBO;

MirrorSide O_side;
MirrorSide X_side;
MirrorSide Y_side;
MirrorSide XY_side;
MirrorSide Z_side;
MirrorSide XZ_side;
MirrorSide YZ_side;
MirrorSide XYZ_side;

static int frame_counter = 0;

static ThreeDPoint last_threeD_point;
static Texture one_px_txtr;

#define RETURN_FROM_PAINTING glEnable(GL_BLEND);\
                             return;


void painting_paint_buffers(PaintSettings settings, bool first_frame, bool last_frame){

    glDisable(GL_BLEND);

    if(last_frame){
        std::string actionTitle = "Unknown painting mode";

        if(settings.painting_mode == 0)
            actionTitle = "Painting";
        if(settings.painting_mode == 1)
            actionTitle = "Softening";
        if(settings.painting_mode == 2)
            actionTitle = "Smearing";
        if(settings.painting_mode == 3)
            actionTitle = "Normal map painting";
        if(settings.painting_mode == 4)
            actionTitle = "Filter painting";
        if(settings.painting_mode == 6)
            actionTitle = "Bucket painting";
        
        if(settings.painted_buffers.material_painting){
            dialog_log.registerMultiChannelPaintingAction(
                                                            "Multi-channel painting", 
                                                            MaterialChannels(
                                                                settings.painted_buffers.material_channel_albedo,
                                                                settings.painted_buffers.material_channel_roughness,
                                                                settings.painted_buffers.material_channel_metallic,
                                                                settings.painted_buffers.material_channel_normalMap,
                                                                settings.painted_buffers.material_channel_heightMap,
                                                                settings.painted_buffers.material_channel_ao
                                                            )
                                                        );
        }
        else{
            dialog_log.registerPaintingAction(actionTitle, settings.painted_buffers.solid_painted_texture);
        }
    }

    if(first_frame){
        frame_counter = 0;
        settings.painting_over_data.texture_field_scene->update_painting_over_texture(settings.point.use_3D);
        painting_update_buffers((settings.painted_buffers.material_painting) ? settings.painted_buffers.material_channel_albedo.getResolution().x : settings.painted_buffers.solid_painted_texture.getResolution().x);
    }

    std::vector<MirrorSide*> mirrorSides = painting_get_selected_mirror_sides(settings.mirror_settings.X, settings.mirror_settings.Y, settings.mirror_settings.Z);
    
    if(settings.painting_mode == 6){
        bucket_paint_texture(painting_projected_painting_FBO.colorBuffer, Color(settings.color_buffer.stroke_albedo_color), settings.stroke_brush.properties.opacity, (settings.vertex_buffer.paint_model) ? settings.vertex_buffer.model_mesh : nullptr);
        goto UPDATE_TEXTURE;
    } 

    if(settings.point.use_3D){
        for (MirrorSide* mirrorSide : mirrorSides)
        {
            glm::vec3 offset = glm::vec3(
                                            settings.mirror_settings.X_offset,
                                            settings.mirror_settings.Y_offset,
                                            settings.mirror_settings.Z_offset
                                        );

            offset *= glm::max(mirrorSide->effectAxis, 0.f);

            if(first_frame && mirrorSide->effectAxis == O_side.effectAxis)
                last_threeD_point = settings.point.point_3D;
            
            ThreeDPoint des_point = ThreeDPoint(glm::vec3(settings.point.point_3D.pos * (mirrorSide->effectAxis * -1.f) - offset * 2.f), glm::vec3(settings.point.point_3D.normal * (mirrorSide->effectAxis * -1.f)));
            ThreeDPoint last_des_point = ThreeDPoint(glm::vec3(last_threeD_point.pos * (mirrorSide->effectAxis * -1.f) - offset * 2.f), glm::vec3(last_threeD_point.normal * (mirrorSide->effectAxis * -1.f)));

            std::vector<glm::vec2> strokes;
            Camera cam;
            process_3D_point(des_point, last_des_point, *mirrorSide, &cam, &strokes, settings.vertex_buffer.model_mesh, first_frame, settings.stroke_brush.properties.spacing);
            
            if(first_frame && mirrorSide->effectAxis == O_side.effectAxis)
                last_threeD_point = settings.point.point_3D;
        
            // Perform window painting
            window_paint(&mirrorSide->paintingBuffers.window_painting_texture, strokes, settings.stroke_brush, frame_counter, settings.painting_mode == 2);

            update_depth_texture(mirrorSide->paintingBuffers.depth_texture, cam, settings.vertex_buffer.model_mesh);

            project_window_painting_texture(
                                                &mirrorSide->paintingBuffers.projected_painting_texture, 
                                                mirrorSide->paintingBuffers.window_painting_texture, 
                                                mirrorSide->paintingBuffers.depth_texture.ID, 
                                                cam,
                                                settings.vertex_buffer,
                                                settings.painting_over_data,
                                                settings.color_buffer.stroke_albedo_color.getRGB_normalized(),
                                                settings.painting_mode, 
                                                settings.stroke_brush.properties.opacity, 
                                                true
                                            );
        }
    }
    else{
        std::vector<glm::vec2> strokes;
        process_2D_point(settings.point.point_2D, &strokes, first_frame, settings.stroke_brush.properties.spacing);

        // Perform window painting
        window_paint(&O_side.paintingBuffers.window_painting_texture, strokes, settings.stroke_brush, frame_counter, settings.painting_mode == 2);
        
        for (MirrorSide* mirrorSide : mirrorSides){
            Camera cam = getScene()->camera;
            
            if(mirrorSide->effectAxis != O_side.effectAxis){
                cam = twoD_painting_calculate_camera(settings, *mirrorSide);

                twoD_painting_generate_mirrored_window_painting_texture(mirrorSide);                
            }

            if(first_frame){
                update_depth_texture(mirrorSide->paintingBuffers.depth_texture, cam, settings.vertex_buffer.model_mesh);
            }

                Texture* projected_painting_texture = &mirrorSide->paintingBuffers.projected_painting_texture_low; 
                if(last_frame){
                    projected_painting_texture = &mirrorSide->paintingBuffers.projected_painting_texture; 
                }

            project_window_painting_texture(
                                                projected_painting_texture, 
                                                mirrorSide->paintingBuffers.window_painting_texture, 
                                                mirrorSide->paintingBuffers.depth_texture.ID, 
                                                cam,
                                                settings.vertex_buffer,
                                                settings.painting_over_data,
                                                settings.color_buffer.stroke_albedo_color.getRGB_normalized(),
                                                settings.painting_mode, 
                                                settings.stroke_brush.properties.opacity, 
                                                false
                                            );
        }
    }
    
    GENERATE_PROJECTED_PAINTING_TEXTURE_AND_UPDATE_TEXTURE:
    generate_projected_painting_texture(&painting_projected_painting_FBO, settings.mirror_settings.X, settings.mirror_settings.Y, settings.mirror_settings.Z, !last_frame && !settings.point.use_3D);
        
    frame_counter++;

    UPDATE_TEXTURE:

    if(last_frame){
        if(twoD_painting_mode && panel_displaying_modes.selectedElement != 2){
            LGDLOG::start << "ERROR : Painting : Invalid displaying mode for the 2D painting" << LGDLOG::end;
            RETURN_FROM_PAINTING;
        }

        if(button_mesh_selection.selectedMeshI >= getScene()->model->meshes.size()){
            LGDLOG::start << "ERROR : Painting : Invalid selected mesh" << LGDLOG::end;
            RETURN_FROM_PAINTING;
        }
        
        if(settings.color_buffer.use_custom_material){
            update_custom_material_mesh(settings.color_buffer, settings.vertex_buffer.model_mesh, (settings.painted_buffers.material_painting) ? settings.painted_buffers.material_channel_albedo.getResolution() : settings.painted_buffers.solid_painted_texture.getResolution()); 
        }

        if(settings.painting_mode == 4){
            for (PaintedBufferData painted_buffer : get_painted_buffers(settings)){
                char clrPx[4] = {painted_buffer.clr.r * 127, painted_buffer.clr.g * 127, painted_buffer.clr.b * 127, 1.};
                one_px_txtr.update(clrPx, 1, 1);
                button_painting_filter_mode_filter.filter.applyFilter(painted_buffer.txtr, painting_projected_painting_FBO.colorBuffer, one_px_txtr);
            }
        }
        else{
            for (PaintedBufferData painted_buffer : get_painted_buffers(settings))
            {
                if(settings.color_buffer.use_custom_material){
                    painted_buffer.txtr.mix(painted_buffer.corresponding_custom_material_channel, painting_projected_painting_FBO.colorBuffer, true, false, false);
                    painted_buffer.txtr.removeSeams(*getScene()->get_selected_mesh());
                }
                else{
                    updateTheTexture(painted_buffer.txtr, painted_buffer.channel_index, settings, painting_projected_painting_FBO);

                    for (size_t i = 0; i < Library::getTextureArraySize(); i++)
                    {
                        if(painted_buffer.txtr.ID == Library::getTexture(i)->ID){
                            projectUpdatingThreadElements.updateTextures = true;
                        }
                    }
                }
            }
        }

        refresh_buffers(&painting_projected_painting_FBO);

        updateThePreRenderedPanels = true;

        if(settings.vertex_buffer.paint_model){
            settings.vertex_buffer.model_mesh->layerScene.update_result(std::stoi(comboBox_layers_resolution.texts[comboBox_layers_resolution.selectedIndex]), glm::vec3(0.f), *settings.vertex_buffer.model_mesh);
        }
    }

    RETURN_FROM_PAINTING;
}










bool painting_paintable_condition()
{
    return (!panels_any_hovered() || (panel_twoD_painting.hover && twoD_painting_mode)) && 
            (panel_displaying_modes.selectedElement == 1 || panel_displaying_modes.selectedElement == 2) && panel_painting_modes.selectedElement != 5 &&
            !checkComboList_painting_over.panel.sections[0].elements[1].checkBox.clickState1 &&
            !getScene()->get_selected_mesh()->face_selection_data.editMode &&
            !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) &&
            !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_ALT);
}

bool painting_paint_condition(){
    return painting_paintable_condition() && *Mouse::LPressed();
}

std::vector<MirrorSide*> painting_get_selected_mirror_sides(bool mirror_X, bool mirror_Y, bool mirror_Z){
    std::vector<MirrorSide*> mirrorSides;
    
    bool mirror_XY = mirror_X && mirror_Y;     
    bool mirror_XZ = mirror_X && mirror_Z; 
    bool mirror_YZ = mirror_Y && mirror_Z; 
    bool mirror_XYZ = mirror_X && mirror_Y && mirror_Z; 
    
    if(true)
        mirrorSides.push_back(&O_side); // 0
    if(mirror_X)
        mirrorSides.push_back(&X_side); // 1
    if(mirror_Y)
        mirrorSides.push_back(&Y_side); // 2
    if(mirror_XY)
        mirrorSides.push_back(&XY_side); // 3
    if(mirror_Z)
        mirrorSides.push_back(&Z_side); // 4
    if(mirror_XZ)
        mirrorSides.push_back(&XZ_side); // 5
    if(mirror_YZ)
        mirrorSides.push_back(&YZ_side); // 6
    if(mirror_XYZ)
        mirrorSides.push_back(&XYZ_side); // 7

    return mirrorSides;
}

#define INIT_MIRROR_SIDE(mirror_side, axis) mirror_side.paintingBuffers.depth_texture  = Texture((char*)nullptr, 1024, 1024, GL_LINEAR, GL_RGBA, GL_RGBA32F);\
                                            mirror_side.paintingBuffers.window_painting_texture  = Texture((char*)nullptr, 1024, 1024);\
                                            mirror_side.paintingBuffers.projected_painting_texture  = Texture((char*)nullptr, 1024, 1024, GL_LINEAR);\
                                            mirror_side.paintingBuffers.projected_painting_texture_low  = Texture((char*)nullptr, 512, 512, GL_LINEAR, GL_RGBA, GL_RGBA16F);\
                                            mirror_side.effectAxis = axis;

void painting_init_buffers(){
    one_px_txtr = Texture((char*)nullptr, 1, 1, GL_NEAREST);

    painting_projected_painting_FBO = Framebuffer(Texture((char*)nullptr, 1024, 1024), GL_TEXTURE_2D, "projected_painting_FBO");
    
    INIT_MIRROR_SIDE(O_side, glm::vec3(-1.f, -1.f, -1.f))
    INIT_MIRROR_SIDE(X_side, glm::vec3(1.f, -1.f, -1.f));
    INIT_MIRROR_SIDE(Y_side, glm::vec3(-1.f, 1.f, -1.f));
    INIT_MIRROR_SIDE(XY_side, glm::vec3(1.f, 1.f, -1.f));
    INIT_MIRROR_SIDE(Z_side, glm::vec3(-1.f, -1.f, 1.f));
    INIT_MIRROR_SIDE(XZ_side, glm::vec3(1.f, -1.f, 1.f));
    INIT_MIRROR_SIDE(YZ_side, glm::vec3(-1.f, 1.f, 1.f));
    INIT_MIRROR_SIDE(XYZ_side, glm::vec3(1.f, 1.f, 1.f));
}

#define UPDATE_MIRROR_SIDE(mirror_side, axis) mirror_side.paintingBuffers.depth_texture  = Texture((char*)nullptr, 1024, 1024, GL_LINEAR, GL_RGBA, GL_RGBA32F);\
                                            mirror_side.paintingBuffers.window_painting_texture  = Texture((char*)nullptr, 1024, 1024);\
                                            mirror_side.paintingBuffers.projected_painting_texture  = Texture((char*)nullptr, 1024, 1024, GL_LINEAR);\
                                            mirror_side.effectAxis = axis;

void painting_update_buffers(const unsigned int resolution){
    one_px_txtr = Texture((char*)nullptr, 1, 1, GL_NEAREST);

    if(painting_projected_painting_FBO.colorBuffer.getResolution().x != resolution)
        painting_projected_painting_FBO.colorBuffer.update((char*)nullptr, resolution, resolution);
    
    if(O_side.paintingBuffers.projected_painting_texture.getResolution().x != resolution){
        O_side.paintingBuffers.projected_painting_texture.update((char*)nullptr, resolution, resolution);
        X_side.paintingBuffers.projected_painting_texture.update((char*)nullptr, resolution, resolution);
        Y_side.paintingBuffers.projected_painting_texture.update((char*)nullptr, resolution, resolution);
        XY_side.paintingBuffers.projected_painting_texture.update((char*)nullptr, resolution, resolution);
        Z_side.paintingBuffers.projected_painting_texture.update((char*)nullptr, resolution, resolution);
        XZ_side.paintingBuffers.projected_painting_texture.update((char*)nullptr, resolution, resolution);
        YZ_side.paintingBuffers.projected_painting_texture.update((char*)nullptr, resolution, resolution);
        XYZ_side.paintingBuffers.projected_painting_texture.update((char*)nullptr, resolution, resolution);
    }
}

PaintSettings get_paint_settings_using_GUI_data(bool* success){
        
    Texture albedo = getScene()->get_selected_mesh()->layerScene.get_painting_channels(success).albedo;  
    Texture roughness = getScene()->get_selected_mesh()->layerScene.get_painting_channels(success).roughness;  
    Texture metallic = getScene()->get_selected_mesh()->layerScene.get_painting_channels(success).metallic;  
    Texture normalMap = getScene()->get_selected_mesh()->layerScene.get_painting_channels(success).normalMap;  
    Texture heightMap = getScene()->get_selected_mesh()->layerScene.get_painting_channels(success).heightMap;  
    Texture ambientOcclusion = getScene()->get_selected_mesh()->layerScene.get_painting_channels(success).ambientOcclusion;

    *success = *success || (panel_displaying_modes.selectedElement != 1 && panel_library_selected_texture.ID);  
    
    return //↓↓↓↓↓↓↓↓↓↓
    PaintSettings(
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
                    PaintSettings::DrawMode(),
                    PaintSettings::SoftenMode(1.f),
                    PaintSettings::SmearMode(panel_smear_painting_properties.sections[0].elements[1].rangeBar.value, panel_smear_painting_properties.sections[0].elements[0].rangeBar.value),
                    PaintSettings::NormalMode(1.f),
                    PaintSettings::FilterMode(button_painting_filter_mode_filter.filter),
                    PaintSettings::BucketMode(),
                    button_painting_brush.brush
                );
}