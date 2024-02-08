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

#include "GUI/Elements/Elements.hpp"
#include "GUI/GUI.hpp"
#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"
#include "Renderer.h"
#include "ShaderSystem/Shader.hpp"
#include "NodeSystem/Node/Node.hpp"
#include "MouseSystem/Mouse.hpp"
#include "SettingsSystem/Settings.hpp"

bool _ligid_renderer_render_first_frame = true;


void Renderer::render(){

    Debugger::block("Started"); // Start
    Debugger::block("Started"); // Start

    Debugger::block("Updating video scale & pollEvents"); // Start
    glm::ivec2 maxWindowSize;
    getContext()->window.getMaximizedScreenSize(maxWindowSize.x, maxWindowSize.y);
    *Settings::videoScale() = maxWindowSize; 

    //Handle user input and interact with the windowing system
    getContext()->window.pollEvents();

    // Prevent rendering the application if the window is minimized
    while (getContext()->window.isMinimized()){
        getContext()->window.pollEvents();
    }
    Debugger::block("Updating video scale & pollEvents"); // End

    Debugger::block("Complete rendering"); // Start
    
    Debugger::block("Prep"); // Start
    //Update local timer data
    timer.tick = false;
    if(timer.runTimer(1.f)){
        timer.tick = true;
        std::cout << timer.FPS << std::endl;
    }
    
    if(_ligid_renderer_render_first_frame)
        getContext()->window.setWindowSize(Settings::videoScale()->x, Settings::videoScale()->y);

    //Update OpenGL viewport every frame
        Settings::defaultFramebuffer()->setViewport();

    Texture paintingTxtrObj = painter.paintingTexture;
    glm::ivec2 resTest = paintingTxtrObj.getResolution();
    if(resTest != glm::ivec2(*Settings::videoScale() / Settings::properties()->paintingResolutionDivier)){
        // Vide scale is changed
    }

    //VSync
    if(Settings::properties()->VSync)
        LigidGL::setSwapInterval(1); //Enable VSync
    else
        LigidGL::setSwapInterval(0); //Disable VSync

    Settings::defaultFramebuffer()->FBO.bind();

    //Default blending settings
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    
    //Refresh the default framebuffer    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Debugger::block("Prep"); // End


    Debugger::block("Skybox Rendering"); // Start

    //Render skybox
    glDisable(GL_CULL_FACE);
    renderSkyBox();

    Debugger::block("Skybox Rendering"); // End

    Debugger::block("Prep 3D Rendering"); // Start
    //Set the uniforms regarding 3D models (mostly vertex shader) 
    set3DUniforms();

    // Update the 3D model depth texture if necessary last frame camera changed position
    if(painter.updateTheDepthTexture && !*Mouse::RPressed()){
        
        painter.updatePosNormalTexture();
        
        //Update the depth texture
        painter.updateDepthTexture();
        painter.updateTheDepthTexture = false;
        
        // Update the model's object id texture
        getModel()->updateObjectIDsTexture();
        
    }

    // Set backface culling property
    if(Settings::properties()->backfaceCulling)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);
    Debugger::block("Prep 3D Rendering"); // End

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
    
    // Process the shortcut inputs & move the camera gradually if necessary
    getScene()->camera.posShortcutInteraction(!userInterface.anyContextMenuActive && !userInterface.anyDialogActive, userInterface.sceneGizmo);

    //Render each mesh
    this->renderMainModel();

    Debugger::block("3D Model"); // End

    Debugger::block("3D Brush Cursor"); // Start
    if(            
            !userInterface.anyPanelHover && 
            !userInterface.anyDialogActive && 
            !userInterface.anyContextMenuActive && 
            (painter.selectedDisplayingModeIndex == 1 || painter.selectedDisplayingModeIndex == 2) && painter.selectedPaintingModeIndex != 5 && painter.selectedPaintingModeIndex != 6 &&
            !painter.paintingoverTextureEditorMode &&
            !painter.faceSelection.editMode &&
            !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) &&
            !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_ALT) && 
            !userInterface.dropper.active &&
            painter.wrapMode &&
            !*Mouse::RPressed()
        )
    {
        render3DBrushCursor();
    }
    Debugger::block("3D Brush Cursor"); // End


    Debugger::block("3D Model Object Selection"); // Start

    // Check if an object is selected after rendering the mesh
    if(painter.selectedDisplayingModeIndex == 0 && ((!userInterface.anyPanelHover || userInterface.objectsPanel.hover) && !userInterface.anyDialogActive && !*Mouse::RPressed() && !*Mouse::MPressed()) || userInterface.logDialog.unded) 
        getModel()->selectObject(this->userInterface.objectsPanel);

    Debugger::block("3D Model Object Selection"); // End
    
    //Clear the depth buffer before rendering the UI elements (prevent coliding)
    glClear(GL_DEPTH_BUFFER_BIT);

    //Bind 2D square vertex buffers
    getBox()->bindBuffers();
    
    //Update the UI projection using window size
    userInterface.projection = glm::ortho(0.f,(float)getContext()->windowScale.x,(float)getContext()->windowScale.y,0.f);
    
    Debugger::block("Complete user interface"); // Start
    
    //Render the UI
    userInterface.render(   //Params
                            timer,
                            project,
                            painter,
                            skybox
                        );
    
    Debugger::block("Complete user interface"); // End

    Debugger::block("Painting"); // Start
    //Painting
    if(
            *Mouse::LPressed() && 
            !userInterface.anyContextMenuActive && 
            !userInterface.anyPanelHover && 
            !userInterface.anyDialogActive && 
            (painter.selectedDisplayingModeIndex == 1 || painter.selectedDisplayingModeIndex == 2) && painter.selectedPaintingModeIndex != 5 &&
            !painter.paintingoverTextureEditorMode &&
            !painter.faceSelection.editMode &&
            !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) &&
            !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_ALT)
        )
    {
        //Paint
        painter.doPaint(    
                            userInterface.projection,
                            {},
                            painter.selectedPaintingModeIndex,
                            userInterface.twoDPaintingPanel,
                            userInterface.twoDPaintingBox,
                            false || painter.wrapMode,
                            userInterface.paintingOverTextureFields
                        );
    }

    //Painting done (refresh)
    if(((painter.refreshable && !*Mouse::LPressed()) || (painter.refreshable && (*Mouse::RClick() || *Mouse::MClick()))) && painter.selectedPaintingModeIndex != 5){ //Last frame painting done or once mouse right click or mouse wheel click
        /*//TODO Prevent updating all the materials
        for (size_t i = 0; i < library.materials.size(); i++)
        {   
            //Update the material after painting
            //TODO : Do smt after painting
            // library.materials[i].updateMaterial(this->settings.textureRes, context, shaders.buttonShader, shaders.tdModelShader);
        }
        */

        //Paint
        painter.doPaint(    
                            userInterface.projection,
                            {},
                            painter.selectedPaintingModeIndex,
                            userInterface.twoDPaintingPanel,
                            userInterface.twoDPaintingBox,
                            true,
                            userInterface.paintingOverTextureFields
                        );

        //Update the selected texture after painting
        painter.updateTexture(userInterface.twoDPaintingPanel, userInterface.projection, painter.selectedPaintingModeIndex, userInterface.filterPaintingModeFilterBtn.filter, userInterface.twoDPaintingBox, userInterface.paintingCustomMat);
        //Refresh the 2D painting texture
        painter.refreshPainting();

        painter.refreshable = false;
    }
    Debugger::block("Painting"); // End


    Debugger::block("Ending"); // Start
    //Set mouse states to default
    *Mouse::LClick() = false;
    *Mouse::RClick() = false;
    *Mouse::MClick() = false;
    
    if(!getContext()->window.isMouseButtonPressed(LIGIDGL_MOUSE_BUTTON_LEFT))
        *Mouse::LPressed() = false;
    if(!getContext()->window.isMouseButtonPressed(LIGIDGL_MOUSE_BUTTON_RIGHT))
        *Mouse::RPressed() = false;
    if(!getContext()->window.isMouseButtonPressed(LIGIDGL_MOUSE_BUTTON_MIDDLE))
        *Mouse::MPressed() = false;

    *Mouse::LDoubleClick() = false;
    *Mouse::mouseOffset() = glm::vec2(0);
    *Mouse::mods() = 0;
    *Mouse::mouseScroll() = 0;

    //Set keyboard states to default
    textRenderer.keyInput = false;
    textRenderer.mods = 0;
    textRenderer.action = 0;

    //Let the getModel()->newModelAdded be true for an another cycle
    if(previousModelNewModelAdded == true)
        getModel()->newModelAdded = false;

    previousModelNewModelAdded = getModel()->newModelAdded; 

    //Cursor is changing there
    //Sets the active cursor (mouse.activeCursor) as the cursor
    //Than changes the active cursor as default cursor
    Mouse::updateCursor();  
    Debugger::block("Ending"); // End

    // ------- Rendering the framebuffer result ------- 
    if(true){

        Debugger::block("Low resolution framebuffer result"); // Start

        if(Settings::defaultFramebuffer()->FBO.ID != 0)
            Settings::defaultFramebuffer()->render();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Create a framebuffer object (FBO)
        GLuint framebuffer;
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Settings::defaultFramebuffer()->bgTxtr.ID, 0);

        // Set up the blit
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); // Bind the default framebuffer as the source
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer); // Bind the FBO as the destination
        glBlitFramebuffer(0, 0, getContext()->windowScale.x, getContext()->windowScale.y, 0, 0, Settings::defaultFramebuffer()->resolution.x, Settings::defaultFramebuffer()->resolution.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        // Unbind the FBO
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Clean up resources (delete framebuffer and unneeded textures if necessary)
        glDeleteFramebuffers(1, &framebuffer);

        Debugger::block("Low resolution framebuffer result"); // End
    }





    //Swap the front and back buffers of the window
    getContext()->window.swapBuffers();

    getBox()->unbindBuffers(); //Finish rendering the UI

    _ligid_renderer_render_first_frame = false;

    Debugger::block("Complete rendering"); // End
}

void Renderer::set3DUniforms(){
    
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
    ShaderSystem::tdModelShader().setMat4("view", getScene()->viewMatrix);
    ShaderSystem::tdModelShader().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::tdModelShader().setMat4("modelMatrix", getScene()->transformMatrix);
    ShaderSystem::tdModelShader().setVec3("mirrorState", glm::vec3(this->painter.oXSide.active, this->painter.oYSide.active, this->painter.oZSide.active));
    ShaderSystem::tdModelShader().setVec3("mirrorOffsets", glm::vec3(this->painter.mirrorXOffset, this->painter.mirrorYOffset, this->painter.mirrorZOffset));
    ShaderSystem::tdModelShader().setFloat("smearTransformStrength", this->painter.smearTransformStrength);
    ShaderSystem::tdModelShader().setFloat("smearBlurStrength", this->painter.smearBlurStrength);
    
    ShaderSystem::sceneTilesShader().use();
    ShaderSystem::sceneTilesShader().setMat4("view", getScene()->viewMatrix);
    ShaderSystem::sceneTilesShader().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::sceneTilesShader().setMat4("modelMatrix",glm::mat4(1));
    ShaderSystem::sceneTilesShader().setVec3("camPos", getScene()->camera.cameraPos);

    ShaderSystem::sceneAxisDisplayerShader().use();
    ShaderSystem::sceneAxisDisplayerShader().setMat4("view", getScene()->viewMatrix);
    ShaderSystem::sceneAxisDisplayerShader().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::sceneAxisDisplayerShader().setMat4("modelMatrix",glm::mat4(1));
    
    //Skybox ball shader 
    ShaderSystem::skyboxBall().use();
    ShaderSystem::skyboxBall().setMat4("view", getScene()->viewMatrix);
    ShaderSystem::skyboxBall().setMat4("projection", getScene()->projectionMatrix);
}





//UTILITY FUNCTIONS


void Renderer::renderSkyBox(){
    
    //Skybox shader
    ShaderSystem::skyboxShader().use();
    ShaderSystem::skyboxShader().setMat4("view", getScene()->viewMatrix);
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
    glBindTexture(GL_TEXTURE_2D, userInterface.displayerDialog.panel.sections[0].elements[5].button.texture.ID);
    ShaderSystem::buttonShader().use();
    ShaderSystem::buttonShader().setVec3("pos", glm::vec3(getContext()->windowScale / glm::ivec2(2), 0.1));
    ShaderSystem::buttonShader().setVec2("scale", getContext()->windowScale / glm::ivec2(2));
    ShaderSystem::buttonShader().setFloat("properties.colorMixVal", 0.f);
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", userInterface.displayerDialog.panel.sections[0].elements[6].rangeBar.value);
    ShaderSystem::buttonShader().setInt("states.renderTexture",     1    );
    ShaderSystem::buttonShader().setInt("properties.txtr",     0    );
    LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Renderer::renderSkybox");
    glBindTexture(GL_TEXTURE_2D, 0);
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);
    ShaderSystem::buttonShader().setInt("states.renderTexture"  ,     0    );
    glClear(GL_DEPTH_BUFFER_BIT);
    getBox()->unbindBuffers();
}

void Renderer::renderMainModel(){
    
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
        
        ShaderSystem::tdModelShader().setInt("paintingOverTexture", 13);
        glActiveTexture(GL_TEXTURE13);
        glBindTexture(GL_TEXTURE_2D, painter.paintingOverTexture.ID);
        
        if(!(i != painter.selectedMeshIndex && painter.faceSelection.hideUnselected)){
            ShaderSystem::tdModelShader().setInt("primitiveCount", getModel()->meshes[i].indices.size() / 3);
            getModel()->meshes[i].Draw(painter.faceSelection.editMode && i == painter.selectedMeshIndex && painter.selectedDisplayingModeIndex != 0);
        }
    }
    
    ShaderSystem::tdModelShader().setFloat("opacity", 1.f);
    ShaderSystem::tdModelShader().setInt("usingMeshSelection", false);
    ShaderSystem::tdModelShader().setInt("meshSelectionEditing", false);
}


void render3DPoints(){
    /*if(!threeDPointsStencilTexture.ID){
        threeDPointsStencilTexture = Texture(nullptr, 256, 256);
        threeDPointsStencilFBO = Framebuffer(threeDPointsStencilTexture, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, glm::ivec2(256)), "threeDPointsStencilFBO");
    }

    threeDPointsStencilFBO.bind();
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    

    glViewport(0, 0, 256, 256);

    ShaderSystem::color3d().use();
    ShaderSystem::color3d().setMat4("view", getScene()->viewMatrix);
    ShaderSystem::color3d().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::color3d().setMat4("modelMatrix", getScene()->transformMatrix);
    ShaderSystem::color3d().setVec4("color", glm::vec4(0.f));

    if(painter.selectedMeshIndex < getModel()->meshes.size())
        getModel()->meshes[painter.selectedMeshIndex].Draw(false);

    for (size_t i = 0; i < threeDPointsPipeline.size(); i++)
    {
        threeDPointsPipeline[i]->render(timer, false, this->painter, Framebuffer(), true);
    }
    
    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();
    
    for (size_t i = 0; i < threeDPointsPipeline.size(); i++)
    {
        threeDPointsPipeline[i]->render(timer, false, this->painter, threeDPointsStencilFBO, false);
    }
    */
}

void Renderer::render3DBrushCursor(){
    glClear(GL_DEPTH_BUFFER_BIT);
    ShaderSystem::color3d().use();
    ShaderSystem::color3d().setMat4("view", getScene()->viewMatrix);
    ShaderSystem::color3d().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::color3d().setVec4("color", glm::vec4(1.f));
    ShaderSystem::color3d().setInt("depthToleranceMode", 0);
    
    float* posData = new float[4]; 
    float* normalData = new float[4]; 

    painter.getPosNormalValOverPoint(
                                        glm::vec2(
                                                    Mouse::cursorPos()->x, 
                                                    getContext()->windowScale.y - Mouse::cursorPos()->y
                                                ),
                                        posData,
                                        normalData,
                                        true
                                    );

    Settings::defaultFramebuffer()->FBO.bind();

    for (size_t i = 0; i < getTDBrushCursorModel()->meshes.size(); i++)
    {
        glm::mat4 transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(posData[0], posData[1], posData[2]));
        
        glm::vec3 initialForward(0.0f, 0.0f, 1.0f);

        // Desired normal vector
        glm::vec3 normalVector(normalData[0], normalData[1], normalData[2]);
        normalVector = glm::normalize(normalVector); // Ensure it's a unit vector

        // Compute the rotation axis using cross product
        glm::vec3 rotationAxis = glm::cross(initialForward, normalVector);
        float rotationAngle = glm::acos(glm::dot(initialForward, normalVector));

        // Create a rotation matrix to align the model's initial forward direction with the normal vector
        transform = glm::rotate(transform, rotationAngle, rotationAxis);
        
        if(i == 1)
            transform = glm::scale(transform, glm::vec3(0.05f));
        else if(i == 0)
            transform = glm::scale(transform, glm::vec3(0.01f + painter.brushProperties.radius * 4.f));
                    
        ShaderSystem::color3d().setMat4("modelMatrix", transform);

        if(posData[3] != 0.f)
            getTDBrushCursorModel()->meshes[i].Draw(false);
    }

    delete[] posData;
    delete[] normalData;
}