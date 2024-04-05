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

#include "3D/ThreeD.hpp"
#include "3D/Camera/Camera.hpp"
#include "3D/Scene/Scene.hpp"

#include "GUI/Panels.hpp"

#include "Toolkit/VectorScene/VectorScene.hpp"

static Camera prevCam;
bool face_selection_interaction(Timer& timer, Model* model, int meshI, bool registerHistory);

void Scene::render_model(Timer& timer){
    
    //3D Model Shader
    ShaderSystem::tdModelShader().use();
    //ShaderSystem::tdModelShader().setInt("render2D", 0);
    ShaderSystem::tdModelShader().setInt("skybox",0);
    ShaderSystem::tdModelShader().setInt("prefilterMap",1);
    ShaderSystem::tdModelShader().setInt("albedoTxtr",2);
    ShaderSystem::tdModelShader().setInt("roughnessTxtr",3);
    ShaderSystem::tdModelShader().setInt("metallicTxtr",4);
    ShaderSystem::tdModelShader().setInt("normalMapTxtr",5);
    ShaderSystem::tdModelShader().setInt("heightMapTxtr",6);
    ShaderSystem::tdModelShader().setInt("ambientOcclusionTxtr",7);
    ShaderSystem::tdModelShader().setInt("paintingTexture",8);
    ShaderSystem::tdModelShader().setInt("selectedPrimitiveIDS", 11);
    ShaderSystem::tdModelShader().setInt("meshMask", 12);
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

    ShaderSystem::tdModelShader().setFloat("smearTransformStrength", panel_smear_painting_properties.sections[0].elements[0].rangeBar.value);
    ShaderSystem::tdModelShader().setFloat("smearBlurStrength", panel_smear_painting_properties.sections[0].elements[1].rangeBar.value);
    
    ShaderSystem::sceneTilesShader().use();
    ShaderSystem::sceneTilesShader().setMat4("view", this->camera.viewMatrix);
    ShaderSystem::sceneTilesShader().setMat4("projection", this->projectionMatrix);
    ShaderSystem::sceneTilesShader().setMat4("modelMatrix",glm::mat4(1));
    ShaderSystem::sceneTilesShader().setVec3("camPos", this->camera.cameraPos);

    ShaderSystem::sceneAxisDisplayerShader().use();
    ShaderSystem::sceneAxisDisplayerShader().setMat4("view", this->camera.viewMatrix);
    ShaderSystem::sceneAxisDisplayerShader().setMat4("projection", this->projectionMatrix);
    ShaderSystem::sceneAxisDisplayerShader().setMat4("modelMatrix",glm::mat4(1));
    
    //Skybox ball shader 
    ShaderSystem::skyboxBall().use();
    ShaderSystem::skyboxBall().setMat4("view", this->camera.viewMatrix);
    ShaderSystem::skyboxBall().setMat4("projection", this->projectionMatrix);
    
    // Set backface culling property
    if(this->backfaceCulling)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);

    ShaderSystem::tdModelShader().use();

    for (size_t i = 0; i < this->model->meshes.size(); i++)
    {   
        /* Albedo */
        glActiveTexture(GL_TEXTURE2);
        if(this->model->meshes[i].albedo.ID && glIsTexture(this->model->meshes[i].albedo.ID) == GL_TRUE)
            glBindTexture(GL_TEXTURE_2D, this->model->meshes[i].albedo.ID);
        else
            glBindTexture(GL_TEXTURE_2D, appTextures.materialChannelMissingTexture.ID);

        /* Roughness */
        glActiveTexture(GL_TEXTURE3);
        if(this->model->meshes[i].roughness.ID && glIsTexture(this->model->meshes[i].roughness.ID) == GL_TRUE)
            glBindTexture(GL_TEXTURE_2D, this->model->meshes[i].roughness.ID);
        else
            glBindTexture(GL_TEXTURE_2D, appTextures.materialChannelMissingTexture.ID);

        /* Metallic*/
        glActiveTexture(GL_TEXTURE4);
        if(this->model->meshes[i].metallic.ID && glIsTexture(this->model->meshes[i].metallic.ID) == GL_TRUE)
            glBindTexture(GL_TEXTURE_2D, this->model->meshes[i].metallic.ID);
        else
            glBindTexture(GL_TEXTURE_2D, appTextures.materialChannelMissingTexture.ID);

        /* Normal Map*/
        glActiveTexture(GL_TEXTURE5);
        if(this->model->meshes[i].normalMap.ID && glIsTexture(this->model->meshes[i].normalMap.ID) == GL_TRUE)
            glBindTexture(GL_TEXTURE_2D, this->model->meshes[i].normalMap.ID);
        else
            glBindTexture(GL_TEXTURE_2D, appTextures.materialChannelMissingTexture.ID);

        /* Height Map*/
        glActiveTexture(GL_TEXTURE6);
        if(this->model->meshes[i].heightMap.ID && glIsTexture(this->model->meshes[i].heightMap.ID) == GL_TRUE)
            glBindTexture(GL_TEXTURE_2D, this->model->meshes[i].heightMap.ID);
        else
            glBindTexture(GL_TEXTURE_2D, appTextures.materialChannelMissingTexture.ID);

        /* Ambient Occlusion*/
        glActiveTexture(GL_TEXTURE7);
        if(this->model->meshes[i].ambientOcclusion.ID && glIsTexture(this->model->meshes[i].ambientOcclusion.ID) == GL_TRUE)
            glBindTexture(GL_TEXTURE_2D, this->model->meshes[i].ambientOcclusion.ID);
        else
            glBindTexture(GL_TEXTURE_2D, appTextures.materialChannelMissingTexture.ID);

        
        if(panel_displaying_modes.selectedElement == 2){
            ShaderSystem::tdModelShader().setInt("albedo_only", 1);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, panel_library_selected_texture.ID);
        }
        else{
            ShaderSystem::tdModelShader().setInt("albedo_only", 0);
        }
        

        if(i == button_mesh_selection.selectedMeshI){
            ShaderSystem::tdModelShader().setInt("usingMeshSelection", this->model->meshes[i].face_selection_data.activated);
            ShaderSystem::tdModelShader().setInt("meshSelectionEditing", this->model->meshes[i].face_selection_data.editMode);
            ShaderSystem::tdModelShader().setInt("hideUnselected", this->model->meshes[i].face_selection_data.hideUnselected);
        }
        else{
            ShaderSystem::tdModelShader().setInt("usingMeshSelection", false);
            ShaderSystem::tdModelShader().setInt("meshSelectionEditing", false);
            ShaderSystem::tdModelShader().setInt("hideUnselected", false);

            this->model->meshes[i].face_selection_data.activated = false;
            this->model->meshes[i].face_selection_data.editMode = false;
        }
        
        if(button_mesh_selection.selectedMeshI == i){
            ShaderSystem::tdModelShader().setFloat("opacity", 1.f);
        }
        else{
            if(!this->model->meshes[i].face_selection_data.hideUnselected)
                ShaderSystem::tdModelShader().setFloat("opacity", 0.2f);
            else
                ShaderSystem::tdModelShader().setFloat("opacity", 0.0f);
        }
        
        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_2D, this->model->meshes[i].face_selection_data.selectedFaces.ID);
    
        glActiveTexture(GL_TEXTURE12);
        if(this->model->meshes[i].face_selection_data.editMode)
            glBindTexture(GL_TEXTURE_2D, appTextures.white.ID);
        else
            glBindTexture(GL_TEXTURE_2D, this->model->meshes[i].face_selection_data.meshMask.ID);
        
        
        
        ShaderSystem::tdModelShader().setInt("enableAlbedoChannel", checkComboList_painting_color.panel.sections[0].elements[1].checkBox.clickState1);
        
        ShaderSystem::tdModelShader().setInt("enableRoughnessChannel", checkComboList_painting_color.panel.sections[0].elements[3].checkBox.clickState1);
        ShaderSystem::tdModelShader().setFloat("roughnessVal", checkComboList_painting_color.panel.sections[0].elements[4].rangeBar.value);
        
        ShaderSystem::tdModelShader().setInt("enableMetallicChannel", checkComboList_painting_color.panel.sections[0].elements[5].checkBox.clickState1);
        ShaderSystem::tdModelShader().setFloat("metallicVal", checkComboList_painting_color.panel.sections[0].elements[6].rangeBar.value);
        
        ShaderSystem::tdModelShader().setInt("enableNormalMapChannel", checkComboList_painting_color.panel.sections[0].elements[7].checkBox.clickState1);
        ShaderSystem::tdModelShader().setFloat("normalMapStrengthVal", checkComboList_painting_color.panel.sections[0].elements[8].rangeBar.value);
        
        ShaderSystem::tdModelShader().setInt("enableHeightMapChannel", checkComboList_painting_color.panel.sections[0].elements[9].checkBox.clickState1);
        ShaderSystem::tdModelShader().setFloat("heightMapVal", checkComboList_painting_color.panel.sections[0].elements[10].rangeBar.value);
        
        ShaderSystem::tdModelShader().setInt("enableAOChannel", checkComboList_painting_color.panel.sections[0].elements[11].checkBox.clickState1);
        ShaderSystem::tdModelShader().setFloat("ambientOcclusionVal", checkComboList_painting_color.panel.sections[0].elements[12].rangeBar.value);

        ShaderSystem::tdModelShader().setInt("paintingMode", painting_paint_condition());

        //Bind the skybox
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.ID);
        
        //Bind the prefiltered skybox
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.IDPrefiltered);
        
        //Bind the painting texture
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, painting_projected_painting_FBO.colorBuffer.ID);
        
        if(!(i != button_mesh_selection.selectedMeshI && this->model->meshes[i].face_selection_data.hideUnselected)){
            ShaderSystem::tdModelShader().setInt("primitiveCount", this->model->meshes[i].indices.size() / 3);
            this->model->meshes[i].Draw(this->model->meshes[i].face_selection_data.editMode && i == button_mesh_selection.selectedMeshI);
        }
    }   

    if(this->get_selected_mesh()->face_selection_data.editMode && !panels_any_hovered())    
        face_selection_interaction(timer, this->model, button_mesh_selection.selectedMeshI, true);    
    
    ShaderSystem::tdModelShader().setFloat("opacity", 1.f);
    ShaderSystem::tdModelShader().setInt("usingMeshSelection", false);
    ShaderSystem::tdModelShader().setInt("meshSelectionEditing", false);

    // Update the 3D model depth texture if necessary last frame camera changed position
    if(
            (prevCam != this->camera) && !*Mouse::RPressed()
        )
    {
        getScene()->get_selected_mesh()->updatePosNormalTexture();
    }
    
    prevCam = this->camera;

    glDisable(GL_CULL_FACE);
}