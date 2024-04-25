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

}

static std::vector<VectorStroke3D> last_strokes;

static Button cancel_editing_vectors_btn = Button(ELEMENT_STYLE_BASIC, glm::vec2(7.f,2.f), "Quit Vector Editing", Texture(), 0.f, false);
static bool firstFrameActivated = true;
void VectorLayer::type_specific_modification(Timer& timer, bool doMouseTracking, const unsigned int resolution, Mesh& mesh){
    
    panel_displaying_modes.sections[0].elements[0].button.clickState1 = true;

    this->vector_scene.strokes_3D = this->strokes;
    this->vector_scene.render_scene(timer, true, true);
    this->strokes = vector_scene.strokes_3D;
    
    this->color_checkComboList.pos = glm::vec3(this->vector_scene.interaction_panel.pos.x - this->vector_scene.interaction_panel.scale.x + this->color_checkComboList.scale.x, this->vector_scene.interaction_panel.pos.y + this->vector_scene.interaction_panel.scale.y + this->color_checkComboList.scale.y, 0.7f);
    this->color_checkComboList.render(timer, true);
    
    this->mirror_checkComboList.pos = glm::vec3(this->color_checkComboList.pos.x + this->color_checkComboList.scale.x + this->mirror_checkComboList.scale.x, this->color_checkComboList.pos.y, this->color_checkComboList.pos.z);
    this->mirror_checkComboList.render(timer, true);
   
    this->brush_properties_button.scale.y = mirror_checkComboList.scale.y;
    this->brush_properties_button.pos = glm::vec3(this->mirror_checkComboList.pos.x + this->mirror_checkComboList.scale.x + this->brush_properties_button.scale.x, this->mirror_checkComboList.pos.y, this->color_checkComboList.pos.z);
    this->brush_properties_button.render(timer, true);
    
    if(this->vector_scene.interaction_panel.sections[0].elements[0].comboBox.selectedIndex == 2){
        this->smear_properties_panel.pos = glm::vec3(this->vector_scene.interaction_panel.pos.x - this->vector_scene.interaction_panel.scale.x + this->smear_properties_panel.scale.x, this->color_checkComboList.pos.y + this->color_checkComboList.scale.y + this->smear_properties_panel.scale.y, 0.7f);
        this->smear_properties_panel.render(timer, true);
    }

    if(this->vector_scene.interaction_panel.sections[0].elements[0].comboBox.selectedIndex == 4){
        this->filter_button.pos = glm::vec3(this->vector_scene.interaction_panel.pos.x - this->vector_scene.interaction_panel.scale.x + this->filter_button.scale.x, this->color_checkComboList.pos.y + this->color_checkComboList.scale.y + this->filter_button.scale.y, 0.7f);
        this->filter_button.render(timer, true);
    }
   
    bool same = true;

    if(this->strokes.size() == last_strokes.size()){
        for (size_t i = 0; i < this->strokes.size(); i++)
        {
            if(last_strokes[i].endPoint.pos != this->strokes[i].endPoint.pos)
                same = false;
            if(last_strokes[i].startPoint.pos != this->strokes[i].startPoint.pos)
                same = false;
        }
    }
    else
        same = false;

    if(!*Mouse::LPressed() && !same){
        this->update_result_buffers(resolution, mesh);
        last_strokes = this->strokes;
    }
}

void VectorLayer::type_specific_generate_result(const unsigned int resolution, Mesh& mesh){
    Box emptyBox;
    TextureFieldScene emptyTextureFieldScene;

    this->result.albedo.update(nullptr, this->result.albedo.getResolution().x, this->result.albedo.getResolution().y);
    this->result.roughness.update(nullptr, this->result.roughness.getResolution().x, this->result.roughness.getResolution().y);
    this->result.metallic.update(nullptr, this->result.metallic.getResolution().x, this->result.metallic.getResolution().y);
    this->result.normalMap.update(nullptr, this->result.normalMap.getResolution().x, this->result.normalMap.getResolution().y);
    this->result.heightMap.update(nullptr, this->result.heightMap.getResolution().x, this->result.heightMap.getResolution().y);
    this->result.ambientOcclusion.update(nullptr, this->result.ambientOcclusion.getResolution().x, this->result.ambientOcclusion.getResolution().y);

    vector_scene.apply_strokes(
                                true, 
                                false, 
                                PaintSettings(
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
                                                this->vector_scene.interaction_panel.sections[0].elements[0].comboBox.selectedIndex,
                                                PaintSettings::DrawMode(),
                                                PaintSettings::SoftenMode(1.f),
                                                PaintSettings::SmearMode(smear_properties_panel.sections[0].elements[1].rangeBar.value, smear_properties_panel.sections[0].elements[0].rangeBar.value),
                                                PaintSettings::NormalMode(1.f),
                                                PaintSettings::FilterMode(filter_button.filter),
                                                PaintSettings::BucketMode(),
                                                brush_properties_button.brush
                                            )
                                );

    mesh.layerScene.update_result(resolution, glm::vec3(0.f), mesh);
}