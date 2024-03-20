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

// 3D Point
extern bool aPointWasAlreadyActivated;

void Renderer::render(){

    Debugger::block("Started"); // Start
    Debugger::block("Started"); // Start

    Debugger::block("Prep"); // Start
    this->start_render();
    Debugger::block("Prep"); // End

    Debugger::block("Skybox Rendering"); // Start
    renderSkyBox();
    Debugger::block("Skybox Rendering"); // End

    Debugger::block("Rendering scene decorations"); // Start

    if(getScene()->renderTiles){
        ShaderSystem::sceneTilesShader().use();
        getScene()->tiles.draw();
        glClear(GL_DEPTH_BUFFER_BIT);
    }
    
    if(getScene()->renderAxisDisplayer){
        ShaderSystem::sceneAxisDisplayerShader().use();
        getScene()->axisDisplayer.draw();
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    Debugger::block("Rendering scene decorations"); // End

    Debugger::block("3D Model"); // Start    
    this->renderMainModel();
    Debugger::block("3D Model"); // End
    
    //Clear the depth buffer before rendering the UI elements (prevent coliding)
    glClear(GL_DEPTH_BUFFER_BIT);

    //Bind 2D square vertex buffers
    getBox()->bindBuffers();
    
    Debugger::block("Render : Toolkits"); // Start
    render_toolkits(timer, painter);
    Debugger::block("Render : Toolkits"); // End
    
    Debugger::block("Render : Panels"); // Start
    panels_render(timer, project, painter);
    Debugger::block("Render : Panels"); // End

    Debugger::block("Painting"); // Start
    //Painting
    if(
            *Mouse::LPressed() && 
            !panels_any_hovered() && 
            (painter.selectedDisplayingModeIndex == 1 || painter.selectedDisplayingModeIndex == 2) && painter.selectedPaintingModeIndex != 5 &&
            !painter.paintingoverTextureEditorMode &&
            !painter.faceSelection.editMode &&
            !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) &&
            !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_ALT)
        )
    {
        //Paint
        painter.doPaint(    
                            painter.wrapMode,
                            *Mouse::LClick(),
                            painter.selectedPaintingModeIndex,
                            false || painter.wrapMode
                        );
    }

    //Painting done (refresh)
    if(((painter.refreshable && !*Mouse::LPressed()) || (painter.refreshable && (*Mouse::RClick() || *Mouse::MClick()))) && painter.selectedPaintingModeIndex != 5){ //Last frame painting done or once mouse right click or mouse wheel click
        //Paint
        painter.doPaint(    
                            painter.wrapMode,
                            true,
                            painter.selectedPaintingModeIndex,
                            true
                        );

        //Update the selected texture after painting
        painter.updateTexture(painter.selectedPaintingModeIndex);
        
        //Refresh the 2D painting texture
        painter.refreshPainting();

        painter.refreshable = false;
    }

    Debugger::block("Painting"); // End
}































//UTILITY FUNCTIONS


void Renderer::renderSkyBox(){
    
    //Skybox shader
    ShaderSystem::skyboxShader().use();
    ShaderSystem::skyboxShader().setMat4("view", getScene()->camera.viewMatrix);
    ShaderSystem::skyboxShader().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::skyboxShader().setMat4("transformMatrix",skybox.transformMatrix);
    ShaderSystem::skyboxShader().setFloat("lod",skybox.lod);
    ShaderSystem::skyboxShader().setVec3("bgColor",skybox.bgColor);
    ShaderSystem::skyboxShader().setFloat("opacity",skybox.opacity);
    ShaderSystem::skyboxShader().setInt("skybox",0);
    
    ShaderSystem::skyboxShader().setInt("gradient", 0);
    ShaderSystem::skyboxShader().setFloat("gradientOffset", 0.f);
    
    //Render the skybox
    skybox.draw(true);

    // Render the background image
    getBox()->bindBuffers();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dialog_displayer.panel.sections[0].elements[5].button.texture.ID);
    ShaderSystem::buttonShader().use();
    ShaderSystem::buttonShader().setVec3("pos", glm::vec3(getContext()->windowScale / glm::ivec2(2), 0.1));
    ShaderSystem::buttonShader().setVec2("scale", getContext()->windowScale / glm::ivec2(2));
    ShaderSystem::buttonShader().setFloat("properties.colorMixVal", 0.f);
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", dialog_displayer.panel.sections[0].elements[6].rangeBar.value);
    ShaderSystem::buttonShader().setInt("states.renderTexture",     1    );
    ShaderSystem::buttonShader().setInt("properties.txtr",     0    );
    LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Renderer::renderSkybox");
    glBindTexture(GL_TEXTURE_2D, 0);
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);
    ShaderSystem::buttonShader().setInt("states.renderTexture"  ,     0    );
    glClear(GL_DEPTH_BUFFER_BIT);
    getBox()->unbindBuffers();
}

static Camera prevCam;

void Renderer::renderMainModel(){
    
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
    ShaderSystem::tdModelShader().setVec3("viewPos", getScene()->camera.cameraPos);
    ShaderSystem::tdModelShader().setMat4("view", getScene()->camera.viewMatrix);
    ShaderSystem::tdModelShader().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::tdModelShader().setMat4("modelMatrix", getScene()->transformMatrix);
    ShaderSystem::tdModelShader().setVec3("mirrorState", glm::vec3(this->painter.oXSide.active, this->painter.oYSide.active, this->painter.oZSide.active));
    ShaderSystem::tdModelShader().setVec3("mirrorOffsets", glm::vec3(this->painter.mirrorXOffset, this->painter.mirrorYOffset, this->painter.mirrorZOffset));
    ShaderSystem::tdModelShader().setFloat("smearTransformStrength", this->painter.smearTransformStrength);
    ShaderSystem::tdModelShader().setFloat("smearBlurStrength", this->painter.smearBlurStrength);
    
    ShaderSystem::sceneTilesShader().use();
    ShaderSystem::sceneTilesShader().setMat4("view", getScene()->camera.viewMatrix);
    ShaderSystem::sceneTilesShader().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::sceneTilesShader().setMat4("modelMatrix",glm::mat4(1));
    ShaderSystem::sceneTilesShader().setVec3("camPos", getScene()->camera.cameraPos);

    ShaderSystem::sceneAxisDisplayerShader().use();
    ShaderSystem::sceneAxisDisplayerShader().setMat4("view", getScene()->camera.viewMatrix);
    ShaderSystem::sceneAxisDisplayerShader().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::sceneAxisDisplayerShader().setMat4("modelMatrix",glm::mat4(1));
    
    //Skybox ball shader 
    ShaderSystem::skyboxBall().use();
    ShaderSystem::skyboxBall().setMat4("view", getScene()->camera.viewMatrix);
    ShaderSystem::skyboxBall().setMat4("projection", getScene()->projectionMatrix);
    
    // Set backface culling property
    if(getScene()->backfaceCulling)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);
        

    for (size_t i = 0; i < getModel()->meshes.size(); i++)
    {   
        /* Albedo */
        glActiveTexture(GL_TEXTURE2);
        if(getModel()->meshes[i].albedo.ID && glIsTexture(getModel()->meshes[i].albedo.ID) == GL_TRUE)
            glBindTexture(GL_TEXTURE_2D, getModel()->meshes[i].albedo.ID);
        else
            glBindTexture(GL_TEXTURE_2D, appTextures.materialChannelMissingTexture.ID);

        /* Roughness */
        glActiveTexture(GL_TEXTURE3);
        if(getModel()->meshes[i].roughness.ID && glIsTexture(getModel()->meshes[i].roughness.ID) == GL_TRUE)
            glBindTexture(GL_TEXTURE_2D, getModel()->meshes[i].roughness.ID);
        else
            glBindTexture(GL_TEXTURE_2D, appTextures.materialChannelMissingTexture.ID);

        /* Metallic*/
        glActiveTexture(GL_TEXTURE4);
        if(getModel()->meshes[i].metallic.ID && glIsTexture(getModel()->meshes[i].metallic.ID) == GL_TRUE)
            glBindTexture(GL_TEXTURE_2D, getModel()->meshes[i].metallic.ID);
        else
            glBindTexture(GL_TEXTURE_2D, appTextures.materialChannelMissingTexture.ID);

        /* Normal Map*/
        glActiveTexture(GL_TEXTURE5);
        if(getModel()->meshes[i].normalMap.ID && glIsTexture(getModel()->meshes[i].normalMap.ID) == GL_TRUE)
            glBindTexture(GL_TEXTURE_2D, getModel()->meshes[i].normalMap.ID);
        else
            glBindTexture(GL_TEXTURE_2D, appTextures.materialChannelMissingTexture.ID);

        /* Height Map*/
        glActiveTexture(GL_TEXTURE6);
        if(getModel()->meshes[i].heightMap.ID && glIsTexture(getModel()->meshes[i].heightMap.ID) == GL_TRUE)
            glBindTexture(GL_TEXTURE_2D, getModel()->meshes[i].heightMap.ID);
        else
            glBindTexture(GL_TEXTURE_2D, appTextures.materialChannelMissingTexture.ID);

        /* Ambient Occlusion*/
        glActiveTexture(GL_TEXTURE7);
        if(getModel()->meshes[i].ambientOcclusion.ID && glIsTexture(getModel()->meshes[i].ambientOcclusion.ID) == GL_TRUE)
            glBindTexture(GL_TEXTURE_2D, getModel()->meshes[i].ambientOcclusion.ID);
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
            glBindTexture(GL_TEXTURE_2D, getModel()->meshes[i].selectedObjectPrimitivesTxtr.ID);
        
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
            ShaderSystem::tdModelShader().setInt("primitiveCount", getModel()->meshes[i].indices.size() / 3);
            getModel()->meshes[i].Draw(painter.faceSelection.editMode && i == painter.selectedMeshIndex && painter.selectedDisplayingModeIndex != 0);
        }
    }
    
    ShaderSystem::tdModelShader().setFloat("opacity", 1.f);
    ShaderSystem::tdModelShader().setInt("usingMeshSelection", false);
    ShaderSystem::tdModelShader().setInt("meshSelectionEditing", false);

    Debugger::block("3D Model Object Selection"); // Start
    // Check if an object is selected after rendering the mesh
    if(painter.selectedDisplayingModeIndex == 0 && ((!panels_any_hovered() || panel_objects.hover) && !*Mouse::RPressed() && !*Mouse::MPressed()) || dialog_log.unded) 
        getModel()->selectObject();
    Debugger::block("3D Model Object Selection"); // End

    // Update the 3D model depth texture if necessary last frame camera changed position
    if(
            (prevCam.cameraPos != getScene()->camera.cameraPos || 
            prevCam.originPos != getScene()->camera.originPos || 
            prevCam.yaw != getScene()->camera.yaw || 
            prevCam.pitch != getScene()->camera.pitch) && !*Mouse::RPressed()
        )
    {
        painter.updatePosNormalTexture();
        
        //Update the depth texture
        painter.updateDepthTexture();
        
        // Update the model's object id texture
        getModel()->updateObjectIDsTexture();
    
        prevCam = getScene()->camera;
    }

    glDisable(GL_CULL_FACE);
}