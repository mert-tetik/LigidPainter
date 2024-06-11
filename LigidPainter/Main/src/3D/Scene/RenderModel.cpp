/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <iostream>

#include "UTIL/Util.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Painting/Painter.hpp"
#include "UTIL/GL/GL.hpp"

#include "3D/ThreeD.hpp"
#include "3D/Camera/Camera.hpp"
#include "3D/Scene/Scene.hpp"

#include "GUI/Panels.hpp"

#include "Toolkit/VectorScene/VectorScene.hpp"

static float calculateDepthToleranceValue(glm::vec3 campos,glm::vec3 origin){
    float distance = glm::distance(campos, origin);

    if(distance < 2.f)
        return 0.01f;
    else if(distance < 5.f)
        return 0.001f;
    else if(distance < 10.f)
        return 0.0001f;

    return 0.00001f;
}

static Camera prevCam;
bool face_selection_interaction(Timer& timer, Model* model, int meshI, bool registerHistory);

void render_wireframe_if_needed(Mesh& mesh, const Camera& camera, const glm::mat4& projectionMatrix, const glm::mat4& transformMatrix){
    // Render the wireframe if face selection edit mode is active
    if(mesh.face_selection_data.editMode){
        ShaderSystem::color3d().use();
        ShaderSystem::color3d().setMat4("view", camera.viewMatrix);
        ShaderSystem::color3d().setMat4("projection", projectionMatrix);
        ShaderSystem::color3d().setMat4("modelMatrix", transformMatrix);
        ShaderSystem::color3d().setVec4("color", glm::vec4(1.f));
        ShaderSystem::color3d().setFloat("depthToleranceValue", 0.0001f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        mesh.Draw("Scene::render_model wireframe");
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        ShaderUTIL::release_bound_shader();
    }
}

void Scene::render_model(Timer& timer){

    // Set backface culling property
    if(this->backfaceCulling)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);

    const bool multi_channel_mode = panel_displaying_modes.selectedElement != 2; 

    if(multi_channel_mode){

        for (size_t i = 0; i < this->model->meshes.size(); i++)
        {   
            if(i != button_mesh_selection.selectedMeshI){
                this->model->meshes[i].face_selection_data.activated = false;
                this->model->meshes[i].face_selection_data.editMode = false;
            }

            //3D Model Shader
            ShaderSystem::tdModelShader().use();
            
            ShaderUTIL::set_shader_struct_painting_data(
                                                            ShaderSystem::tdModelShader(),
                                                            ShaderUTIL::PaintingData(
                                                                                        ShaderUTIL::PaintingData::PaintingBuffers(
                                                                                                                                    (this->model->meshes[i].layerScene.layers.size()) ? this->model->meshes[i].material_channels.albedo : appTextures.noLayersWarningTexture,
                                                                                                                                    (this->model->meshes[i].layerScene.layers.size()) ? this->model->meshes[i].material_channels.roughness : appTextures.white,
                                                                                                                                    (this->model->meshes[i].layerScene.layers.size()) ? this->model->meshes[i].material_channels.metallic : appTextures.black,
                                                                                                                                    (this->model->meshes[i].layerScene.layers.size()) ? this->model->meshes[i].material_channels.normalMap : appTextures.noLayersWarningTexture,
                                                                                                                                    (this->model->meshes[i].layerScene.layers.size()) ? this->model->meshes[i].material_channels.heightMap : appTextures.white,
                                                                                                                                    (this->model->meshes[i].layerScene.layers.size()) ? this->model->meshes[i].material_channels.ambientOcclusion : appTextures.white,
                                                                                                                                    painting_projected_painting_txtr
                                                                                                                                ),   
                                                                                        ShaderUTIL::PaintingData::PaintingSmearData(
                                                                                                                                        panel_smear_painting_properties.sections[0].elements[0].rangeBar.value,
                                                                                                                                        panel_smear_painting_properties.sections[0].elements[1].rangeBar.value
                                                                                                                                    ),   
                                                                                        ShaderUTIL::PaintingData::ChannelData(
                                                                                                                                checkComboList_painting_color.panel.sections[0].elements[4].rangeBar.value,
                                                                                                                                checkComboList_painting_color.panel.sections[0].elements[6].rangeBar.value,
                                                                                                                                checkComboList_painting_color.panel.sections[0].elements[8].rangeBar.value,
                                                                                                                                checkComboList_painting_color.panel.sections[0].elements[10].rangeBar.value,
                                                                                                                                checkComboList_painting_color.panel.sections[0].elements[12].rangeBar.value
                                                                                                                            ),
                                                                                        panel_painting_modes.selectedElement,
                                                                                        checkComboList_painting_over.panel.sections[0].elements[0].checkBox.clickState1                            
                                                                                    )
                                                        );
            
            ShaderUTIL::set_shader_struct_face_selection_data(ShaderSystem::tdModelShader(), this->model->meshes[i]);

            ShaderSystem::tdModelShader().setInt("skybox", GL::bindTexture_CubeMap(skybox.ID, "Scene::render_model"));
            ShaderSystem::tdModelShader().setInt("prefilterMap", GL::bindTexture_CubeMap(skybox.IDPrefiltered, "Scene::render_model"));
            
            ShaderSystem::tdModelShader().setVec3("viewPos", this->camera.cameraPos);
            ShaderSystem::tdModelShader().setMat4("view", this->camera.viewMatrix);
            ShaderSystem::tdModelShader().setMat4("projection", this->projectionMatrix);
            ShaderSystem::tdModelShader().setMat4("modelMatrix", this->transformMatrix);
            ShaderSystem::tdModelShader().setVec3("mirrorState", glm::vec3(
                                                                                checkComboList_painting_mirror.panel.sections[0].elements[0].checkBox.clickState1, 
                                                                                checkComboList_painting_mirror.panel.sections[0].elements[2].checkBox.clickState1, 
                                                                                checkComboList_painting_mirror.panel.sections[0].elements[4].checkBox.clickState1
                                                                            ));
                                                                            
            ShaderSystem::tdModelShader().setVec3("mirrorOffsets", glm::vec3(
                                                                                checkComboList_painting_mirror.panel.sections[0].elements[1].rangeBar.value, 
                                                                                checkComboList_painting_mirror.panel.sections[0].elements[3].rangeBar.value, 
                                                                                checkComboList_painting_mirror.panel.sections[0].elements[5].rangeBar.value
                                                                            ));
            
            if(button_mesh_selection.selectedMeshI == i){
                ShaderSystem::tdModelShader().setFloat("opacity", 1.f);
            }
            else{
                if(!this->model->meshes[i].face_selection_data.hideUnselected)
                    ShaderSystem::tdModelShader().setFloat("opacity", 0.35f);
                else
                    ShaderSystem::tdModelShader().setFloat("opacity", 0.0f);
            }
            
            ShaderSystem::tdModelShader().setInt("enableAlbedoChannel", checkComboList_painting_color.panel.sections[0].elements[1].checkBox.clickState1);
            
            ShaderSystem::tdModelShader().setInt("enableRoughnessChannel", checkComboList_painting_color.panel.sections[0].elements[3].checkBox.clickState1);
            
            ShaderSystem::tdModelShader().setInt("enableMetallicChannel", checkComboList_painting_color.panel.sections[0].elements[5].checkBox.clickState1);
            
            ShaderSystem::tdModelShader().setInt("enableNormalMapChannel", checkComboList_painting_color.panel.sections[0].elements[7].checkBox.clickState1);
            
            ShaderSystem::tdModelShader().setInt("enableHeightMapChannel", checkComboList_painting_color.panel.sections[0].elements[9].checkBox.clickState1);
            
            ShaderSystem::tdModelShader().setInt("enableAOChannel", checkComboList_painting_color.panel.sections[0].elements[11].checkBox.clickState1);

            ShaderSystem::tdModelShader().setInt("paintingMode", painting_paint_condition());
            
            ShaderSystem::tdModelShader().setInt("displayingMode", comboBox_PBR_displaying_mode.selectedIndex);

            if(!(i != button_mesh_selection.selectedMeshI && this->model->meshes[i].face_selection_data.hideUnselected)){
                this->model->meshes[i].Draw("Scene::render_model : PBR");
            }

            if(i == button_mesh_selection.selectedMeshI){
                render_wireframe_if_needed(this->model->meshes[i], this->camera, this->projectionMatrix, this->transformMatrix);
            }

            GL::releaseBoundTextures("Scene::render_model");
            ShaderUTIL::release_bound_shader();
        }   
    }
    else{
        ShaderSystem::solidPaintingShader().use();

        ShaderUTIL::set_shader_struct_painting_data(
                                                        ShaderSystem::solidPaintingShader(),
                                                        ShaderUTIL::PaintingData(
                                                                                    ShaderUTIL::PaintingData::PaintingBuffers(
                                                                                                                                (panel_library_selected_texture.ID) ? panel_library_selected_texture.ID : appTextures.black,
                                                                                                                                0,
                                                                                                                                0,
                                                                                                                                0,
                                                                                                                                0,
                                                                                                                                0,
                                                                                                                                painting_projected_painting_txtr
                                                                                                                            ),   
                                                                                    ShaderUTIL::PaintingData::PaintingSmearData(
                                                                                                                                    panel_smear_painting_properties.sections[0].elements[0].rangeBar.value,
                                                                                                                                    panel_smear_painting_properties.sections[0].elements[1].rangeBar.value
                                                                                                                                ),   
                                                                                    ShaderUTIL::PaintingData::ChannelData(
                                                                                                                            0.f,
                                                                                                                            0.f,
                                                                                                                            0.f,
                                                                                                                            0.f,
                                                                                                                            0.f
                                                                                                                        ),
                                                                                    panel_painting_modes.selectedElement,
                                                                                    checkComboList_painting_over.panel.sections[0].elements[0].checkBox.clickState1                            
                                                                                )
                                                );

        ShaderSystem::solidPaintingShader().setVec3("viewPos", this->camera.cameraPos);
        
        ShaderSystem::solidPaintingShader().setMat4("view", this->camera.viewMatrix);
        ShaderSystem::solidPaintingShader().setMat4("projection", this->projectionMatrix);
        ShaderSystem::solidPaintingShader().setMat4("modelMatrix", this->transformMatrix);

        ShaderSystem::solidPaintingShader().setVec3("mirrorState", glm::vec3(
                                                                    checkComboList_painting_mirror.panel.sections[0].elements[0].checkBox.clickState1, 
                                                                    checkComboList_painting_mirror.panel.sections[0].elements[2].checkBox.clickState1, 
                                                                    checkComboList_painting_mirror.panel.sections[0].elements[4].checkBox.clickState1
                                                                ));
                                                                            
        ShaderSystem::solidPaintingShader().setVec3("mirrorOffsets", glm::vec3(
                                                                            checkComboList_painting_mirror.panel.sections[0].elements[1].rangeBar.value, 
                                                                            checkComboList_painting_mirror.panel.sections[0].elements[3].rangeBar.value, 
                                                                            checkComboList_painting_mirror.panel.sections[0].elements[5].rangeBar.value
                                                                        ));

        ShaderSystem::solidPaintingShader().setInt("paintingMode", painting_paint_condition());

        ShaderUTIL::set_shader_struct_face_selection_data(ShaderSystem::solidPaintingShader(), *this->get_selected_mesh());

        this->get_selected_mesh()->Draw("Scene::render_model : single texture");

        render_wireframe_if_needed(*this->get_selected_mesh(), this->camera, this->projectionMatrix, this->transformMatrix);
        
        GL::releaseBoundTextures("Scene::render_model");
        ShaderUTIL::release_bound_shader();
    }

    if(this->get_selected_mesh()->face_selection_data.editMode && !panels_any_hovered())    
        face_selection_interaction(timer, this->model, button_mesh_selection.selectedMeshI, true);    
    
    // Update the 3D model depth texture if necessary last frame camera changed position
    if(
            (prevCam != this->camera) && !*Mouse::RPressed() ||
            this->get_selected_mesh()->face_selection_data.editMode    
        )
    {
        this->get_selected_mesh()->updatePosNormalTexture();
        prevCam = this->camera;
    }

    glDisable(GL_CULL_FACE);
}