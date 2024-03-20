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

#include "3D/ThreeD.hpp"
#include "3D/Camera/Camera.hpp"
#include "3D/Scene/Scene.hpp"

#include "GUI/Panels.hpp"

#include "Toolkit/VectorScene/VectorScene.hpp"

static Camera prevCam;

void Scene::render_model(Painter& painter){
    
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
    ShaderSystem::tdModelShader().setVec3("mirrorState", glm::vec3(painter.oXSide.active, painter.oYSide.active, painter.oZSide.active));
    ShaderSystem::tdModelShader().setVec3("mirrorOffsets", glm::vec3(painter.mirrorXOffset, painter.mirrorYOffset, painter.mirrorZOffset));
    ShaderSystem::tdModelShader().setFloat("smearTransformStrength", painter.smearTransformStrength);
    ShaderSystem::tdModelShader().setFloat("smearBlurStrength", painter.smearBlurStrength);
    
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

        ShaderSystem::tdModelShader().setInt("paintedTxtrStateIndex", painter.selectedPaintingChannelIndex);
        
        if(painter.selectedDisplayingModeIndex == 2){
            ShaderSystem::tdModelShader().setInt("paintedTxtrStateIndex", 0);
            ShaderSystem::tdModelShader().setInt("displayingMode", 1);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, painter.selectedTexture.ID);
        }
        else{
            ShaderSystem::tdModelShader().setInt("displayingMode", 0);
        }
        
        if(painter.selectedDisplayingModeIndex == 2 || painter.selectedDisplayingModeIndex == 1){
            if(painter.selectedMeshIndex == i)
                ShaderSystem::tdModelShader().setFloat("opacity", 1.f);
            else{
                if(!painter.faceSelection.hideUnselected)
                    ShaderSystem::tdModelShader().setFloat("opacity", 0.2f);
                else
                    ShaderSystem::tdModelShader().setFloat("opacity", 0.0f);
            }
        }
        else
            ShaderSystem::tdModelShader().setFloat("opacity", 1.f);

        if(painter.selectedDisplayingModeIndex == 1 || painter.selectedDisplayingModeIndex == 2){
            if(i == painter.selectedMeshIndex){
                ShaderSystem::tdModelShader().setInt("usingMeshSelection", painter.faceSelection.activated);
                ShaderSystem::tdModelShader().setInt("meshSelectionEditing", painter.faceSelection.editMode);
                ShaderSystem::tdModelShader().setInt("hideUnselected", painter.faceSelection.hideUnselected);
            }
            else{
                ShaderSystem::tdModelShader().setInt("usingMeshSelection", false);
                ShaderSystem::tdModelShader().setInt("meshSelectionEditing", false);
                ShaderSystem::tdModelShader().setInt("hideUnselected", false);
            }
            
            glActiveTexture(GL_TEXTURE11);
            glBindTexture(GL_TEXTURE_2D, painter.faceSelection.selectedFaces.ID);
        
            glActiveTexture(GL_TEXTURE12);
            glBindTexture(GL_TEXTURE_2D, painter.faceSelection.meshMask.ID);
        }
        else{
            ShaderSystem::tdModelShader().setInt("usingMeshSelection", false);
            ShaderSystem::tdModelShader().setInt("meshSelectionEditing", true);
            ShaderSystem::tdModelShader().setInt("hideUnselected", false);
        
            glActiveTexture(GL_TEXTURE11);
            glBindTexture(GL_TEXTURE_2D, this->model->meshes[i].selectedObjectPrimitivesTxtr.ID);
        
            glActiveTexture(GL_TEXTURE12);
            glBindTexture(GL_TEXTURE_2D, appTextures.white.ID);
        }
        
        
        ShaderSystem::tdModelShader().setInt("materialPainting", painter.materialPainting);
        ShaderSystem::tdModelShader().setInt("enableAlbedoChannel", painter.enableAlbedoChannel);
        ShaderSystem::tdModelShader().setInt("enableRoughnessChannel", painter.enableRoughnessChannel);
        ShaderSystem::tdModelShader().setFloat("roughnessVal", painter.roughnessVal);
        ShaderSystem::tdModelShader().setInt("enableMetallicChannel", painter.enableMetallicChannel);
        ShaderSystem::tdModelShader().setFloat("metallicVal", painter.metallicVal);
        ShaderSystem::tdModelShader().setInt("enableNormalMapChannel", painter.enableNormalMapChannel);
        ShaderSystem::tdModelShader().setFloat("normalMapStrengthVal", painter.normalMapStrengthVal);
        ShaderSystem::tdModelShader().setInt("enableHeightMapChannel", painter.enableHeightMapChannel);
        ShaderSystem::tdModelShader().setFloat("heightMapVal", painter.heightMapVal);
        ShaderSystem::tdModelShader().setInt("enableAOChannel", painter.enableAOChannel);
        ShaderSystem::tdModelShader().setFloat("ambientOcclusionVal", painter.ambientOcclusionVal);

        ShaderSystem::tdModelShader().setInt("paintingMode", painter.refreshable);

        // 3D Model    
        ShaderSystem::tdModelShader().use();

        //Bind the skybox
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.ID);
        
        //Bind the prefiltered skybox
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.IDPrefiltered);
        
        //Bind the painting texture
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, painter.projectedPaintingTexture.ID);
        
        if(!(i != painter.selectedMeshIndex && painter.faceSelection.hideUnselected)){
            ShaderSystem::tdModelShader().setInt("primitiveCount", this->model->meshes[i].indices.size() / 3);
            this->model->meshes[i].Draw(painter.faceSelection.editMode && i == painter.selectedMeshIndex && painter.selectedDisplayingModeIndex != 0);
        }
    }
    
    ShaderSystem::tdModelShader().setFloat("opacity", 1.f);
    ShaderSystem::tdModelShader().setInt("usingMeshSelection", false);
    ShaderSystem::tdModelShader().setInt("meshSelectionEditing", false);

    Debugger::block("3D Model Object Selection"); // Start
    // Check if an object is selected after rendering the mesh
    if(painter.selectedDisplayingModeIndex == 0 && ((!panels_any_hovered() || panel_objects.hover) && !*Mouse::RPressed() && !*Mouse::MPressed()) || dialog_log.unded) 
        this->model->selectObject();
    Debugger::block("3D Model Object Selection"); // End

    // Update the 3D model depth texture if necessary last frame camera changed position
    if(
            (prevCam.cameraPos != this->camera.cameraPos || 
            prevCam.originPos != this->camera.originPos || 
            prevCam.yaw != this->camera.yaw || 
            prevCam.pitch != this->camera.pitch) && !*Mouse::RPressed()
        )
    {
        painter.updatePosNormalTexture();
        
        //Update the depth texture
        painter.updateDepthTexture();
        
        // Update the model's object id texture
        this->model->updateObjectIDsTexture();
    
        prevCam = this->camera;
    }

    glDisable(GL_CULL_FACE);
}