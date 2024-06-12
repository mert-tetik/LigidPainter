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

#include <string>
#include <iostream>
#include <vector>

#include "UTIL/Util.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/Threads/Threads.hpp"
#include "UTIL/Painting/Painter.hpp"

#include "GUI/Elements.hpp"
#include "GUI/Panels.hpp"
#include "GUI/Dialogs.hpp"

#include "Toolkit/Layers/Layers.hpp"

VectorLayer::VectorLayer(const unsigned int resolution){
    this->title = "Vector Layer";
    this->layerType = "vector";
    this->layerIcon = appTextures.inkPenIcon;
    this->generate_layer_buffers(resolution);

    this->vector_scene = VectorScene({}, {});

    this->color_checkComboList = checkComboList_painting_color;
    this->mirror_checkComboList = checkComboList_painting_mirror;

    this->smear_properties_panel = panel_smear_painting_properties;
    this->filter_button = button_painting_filter_mode_filter;
    this->brush_properties_button = button_painting_brush;
    this->brush_properties_button.brush = Brush(BrushProperties(appTextures.white.duplicateTexture("button_painting_brush.brush texture")), "Vector layer brush");

}

static std::vector<VectorStroke3D> last_strokes;
static PaintSettings last_paint_settings;

static Button quit_modifying_button = Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), "Quit Modifying", Texture(), 0.f, false);

static bool firstFrameActivated = true;
void VectorLayer::type_specific_modification(Timer& timer, bool doMouseTracking, const unsigned int resolution, Mesh& mesh){
    
    panel_displaying_modes.sections[0].elements[0].button.clickState1 = true;

    this->vector_scene.render_scene(timer, doMouseTracking && !panels_any_hovered() && !quit_modifying_button.hover, true, false);
    
    glClear(GL_DEPTH_BUFFER_BIT);

    Button vector_layer_specific_info_btn = Button(ELEMENT_STYLE_SOLID, glm::vec2(this->color_checkComboList.scale.x + this->mirror_checkComboList.scale.x + this->brush_properties_button.scale.x, 1.f), "Vector Layer Specific", Texture(), 0.f, false);
    vector_layer_specific_info_btn.pos = glm::vec3(panel_library.pos.x + panel_library.scale.x + vector_layer_specific_info_btn.scale.x + 1.f, panel_library.pos.y - panel_library.scale.y + vector_layer_specific_info_btn.scale.y + 1.f, 0.9f);
    vector_layer_specific_info_btn.render(timer, false);
    
    quit_modifying_button.scale.x = vector_layer_specific_info_btn.scale.x;
    quit_modifying_button.pos = glm::vec3(vector_layer_specific_info_btn.pos.x - vector_layer_specific_info_btn.scale.x + quit_modifying_button.scale.x, vector_layer_specific_info_btn.pos.y + vector_layer_specific_info_btn.scale.y + quit_modifying_button.scale.y, 0.7f);
    quit_modifying_button.render(timer, doMouseTracking);

    if(quit_modifying_button.clicked){
        this->type_specific_modification_enabled = false;
    }

    this->color_checkComboList.pos = glm::vec3(quit_modifying_button.pos.x - quit_modifying_button.scale.x + this->color_checkComboList.scale.x, quit_modifying_button.pos.y + quit_modifying_button.scale.y + this->color_checkComboList.scale.y, 0.7f);
    checkComboList_painting_color_render(timer, doMouseTracking, this->color_checkComboList);
    
    this->mirror_checkComboList.pos = glm::vec3(this->color_checkComboList.pos.x + this->color_checkComboList.scale.x + this->mirror_checkComboList.scale.x, this->color_checkComboList.pos.y, this->color_checkComboList.pos.z);
    checkComboList_painting_mirror_render(timer, doMouseTracking, this->mirror_checkComboList);
   
    this->brush_properties_button.scale.y = mirror_checkComboList.scale.y;
    this->brush_properties_button.pos = glm::vec3(this->mirror_checkComboList.pos.x + this->mirror_checkComboList.scale.x + this->brush_properties_button.scale.x, this->mirror_checkComboList.pos.y, this->color_checkComboList.pos.z);
    this->brush_properties_button.render(timer, doMouseTracking);
    
    bool same = true;
    if(this->vector_scene.strokes_3D.size() == last_strokes.size()){
        for (size_t i = 0; i < this->vector_scene.strokes_3D.size(); i++)
        {
            if(last_strokes[i].endPoint.pos != this->vector_scene.strokes_3D[i].endPoint.pos)
                same = false;
            if(last_strokes[i].startPoint.pos != this->vector_scene.strokes_3D[i].startPoint.pos)
                same = false;
        }
    }
    else
        same = false;

    if(last_paint_settings != this->get_painting_settings(resolution, mesh)){
        same = false;
    }

    if(!same){
        vector_thread.use_thread(resolution, &this->vector_scene, this, &mesh);
        last_strokes = this->vector_scene.strokes_3D;
        last_paint_settings = this->get_painting_settings(resolution, mesh);
    }
}

void VectorLayer::type_specific_generate_result(const unsigned int resolution, Mesh& mesh){
    this->result.albedo.update((char*)nullptr, this->result.albedo.getResolution().x, this->result.albedo.getResolution().y);
    this->result.roughness.update((char*)nullptr, this->result.roughness.getResolution().x, this->result.roughness.getResolution().y);
    this->result.metallic.update((char*)nullptr, this->result.metallic.getResolution().x, this->result.metallic.getResolution().y);
    this->result.normalMap.update((char*)nullptr, this->result.normalMap.getResolution().x, this->result.normalMap.getResolution().y);
    this->result.heightMap.update((char*)nullptr, this->result.heightMap.getResolution().x, this->result.heightMap.getResolution().y);
    this->result.ambientOcclusion.update((char*)nullptr, this->result.ambientOcclusion.getResolution().x, this->result.ambientOcclusion.getResolution().y);

    this->vector_scene.update3DVectorBuffers();

    vector_scene.apply_strokes(
                                    true, 
                                    false, 
                                    this->get_painting_settings(resolution, mesh)    
                                );

    mesh.layerScene.update_result(resolution, glm::vec3(0.f), mesh);
}

PaintSettings VectorLayer::get_painting_settings(const unsigned int resolution, Mesh& mesh){
    Box emptyBox;
    TextureFieldScene emptyTextureFieldScene;

    return PaintSettings(
                            PaintSettings::PaintingOverData(false, false, &emptyTextureFieldScene),
                            PaintSettings::ColorBuffer(
                                                            this->color_checkComboList.panel.sections[0].elements[2].painterColorSelection.getSelectedColor(),
                                                            this->color_checkComboList.panel.sections[0].elements[4].rangeBar.value,
                                                            this->color_checkComboList.panel.sections[0].elements[6].rangeBar.value,
                                                            this->color_checkComboList.panel.sections[0].elements[8].rangeBar.value,
                                                            this->color_checkComboList.panel.sections[0].elements[10].rangeBar.value,
                                                            this->color_checkComboList.panel.sections[0].elements[12].rangeBar.value,
                                                            this->color_checkComboList.panel.sections[0].elements[14].button.material,
                                                            this->color_checkComboList.panel.sections[0].elements[13].checkBox.clickState1
                                                        ),
                            PaintSettings::PaintedBuffers(true, Texture(), true, this->result.albedo, true, this->result.roughness, true, this->result.metallic, true, this->result.normalMap, true, this->result.heightMap, true, this->result.ambientOcclusion),
                            PaintSettings::MirrorSettings(
                                                            this->mirror_checkComboList.panel.sections[0].elements[0].checkBox.clickState1, 
                                                            this->mirror_checkComboList.panel.sections[0].elements[1].rangeBar.value, 
                                                            this->mirror_checkComboList.panel.sections[0].elements[2].checkBox.clickState1, 
                                                            this->mirror_checkComboList.panel.sections[0].elements[3].rangeBar.value, 
                                                            this->mirror_checkComboList.panel.sections[0].elements[4].checkBox.clickState1, 
                                                            this->mirror_checkComboList.panel.sections[0].elements[5].rangeBar.value
                                                        ),
                            PaintSettings::PaintVertexBuffer(&mesh, &emptyBox, true),
                            PaintSettings::PointData(), // Auto-calculates in the function
                            0,
                            PaintSettings::DrawMode(),
                            PaintSettings::SoftenMode(1.f),
                            PaintSettings::SmearMode(smear_properties_panel.sections[0].elements[1].rangeBar.value, smear_properties_panel.sections[0].elements[0].rangeBar.value),
                            PaintSettings::NormalMode(1.f),
                            PaintSettings::FilterMode(filter_button.filter),
                            PaintSettings::BucketMode(),
                            brush_properties_button.brush
                        );
}