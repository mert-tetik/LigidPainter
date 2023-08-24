/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

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
    
    //Handle user input and interact with the windowing system
    getContext()->window.pollEvents();
    //Update local timer data
    if(timer.runTimer(1.f)){
        std::cout << timer.FPS << std::endl;
    }
    
    if(_ligid_renderer_render_first_frame)
        getContext()->window.setWindowSize(Settings::videoScale()->x, Settings::videoScale()->y);

    //Update OpenGL viewport every frame
    glViewport(0, 0, getContext()->windowScale.x, getContext()->windowScale.y);
    //VSync
    if(Settings::properties()->VSync)
        LigidGL::setSwapInterval(1); //Enable VSync
    else
        LigidGL::setSwapInterval(0); //Disable VSync

    //Default blending settings
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);
    
    //Refresh the default framebuffer    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glDisable(GL_CULL_FACE);

    //Render skybox
    renderSkyBox();

    box.bindBuffers();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userInterface.displayerDialog.panel.sections[0].elements[5].button.texture.ID);
    ShaderSystem::buttonShader().use();
    ShaderSystem::buttonShader().setVec3("pos", glm::vec3(getContext()->windowScale / glm::ivec2(2), 0.1));
    ShaderSystem::buttonShader().setVec2("scale", getContext()->windowScale / glm::ivec2(2));
    ShaderSystem::buttonShader().setFloat("properties.colorMixVal", 0.f);
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", userInterface.displayerDialog.panel.sections[0].elements[6].rangeBar.value);
    ShaderSystem::buttonShader().setInt("states.renderTexture",     1    );
    ShaderSystem::buttonShader().setInt("properties.txtr",     0    );
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);
    ShaderSystem::buttonShader().setInt("states.renderTexture"  ,     0    );
    glClear(GL_DEPTH_BUFFER_BIT);
    box.unbindBuffers();

    //Set the uniforms regarding 3D models (mostly vertex shader) 
    set3DUniforms();

    //Update the depth texture if necessary
    if(painter.updateTheDepthTexture && !*Mouse::RPressed()){ //Last frame camera changed position
        //Update the depth texture
        painter.updateDepthTexture();

        painter.updateTheDepthTexture = false;
    }

    if(Settings::properties()->backfaceCulling)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);

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

    //3D Model    
    ShaderSystem::tdModelShader().use();

    //Bind the skybox
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP,skybox.ID);
    
    //Bind the prefiltered skybox
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP,skybox.IDPrefiltered);
    
    //Bind the painting texture
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D,painter.paintingTexture);
    
    //Bind the depth texture
    glActiveTexture(GL_TEXTURE9);
    glBindTexture(GL_TEXTURE_2D,painter.depthTexture);

    if(!userInterface.anyContextMenuActive && !userInterface.anyDialogActive){
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_0) || getContext()->window.isKeyPressed(LIGIDGL_KEY_NUMPAD_0)){
            getScene()->camera.originLocked = true;
            
            getScene()->camera.XPLocked = false;
            getScene()->camera.XNLocked = false;
            getScene()->camera.YPLocked = false;
            getScene()->camera.YNLocked = false;
            getScene()->camera.ZPLocked = false;
            getScene()->camera.ZNLocked = false;
        }

        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_1) || getContext()->window.isKeyPressed(LIGIDGL_KEY_NUMPAD_1)){
            getScene()->camera.XPLocked = true;
            getScene()->camera.XNLocked = false;
            getScene()->camera.YPLocked = false;
            getScene()->camera.YNLocked = false;
            getScene()->camera.ZPLocked = false;
            getScene()->camera.ZNLocked = false;
        }
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_2) || getContext()->window.isKeyPressed(LIGIDGL_KEY_NUMPAD_2)){
            getScene()->camera.XNLocked = true;
            getScene()->camera.XPLocked = false;
            getScene()->camera.YPLocked = false;
            getScene()->camera.YNLocked = false;
            getScene()->camera.ZPLocked = false;
            getScene()->camera.ZNLocked = false;
            
        }
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_3) || getContext()->window.isKeyPressed(LIGIDGL_KEY_NUMPAD_3)){
            getScene()->camera.YPLocked = true;
            getScene()->camera.XNLocked = false;
            getScene()->camera.XPLocked = false;
            getScene()->camera.YNLocked = false;
            getScene()->camera.ZPLocked = false;
            getScene()->camera.ZNLocked = false;
        }
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_4) || getContext()->window.isKeyPressed(LIGIDGL_KEY_NUMPAD_4)){
            getScene()->camera.YNLocked = true;
            getScene()->camera.XNLocked = false;
            getScene()->camera.YPLocked = false;
            getScene()->camera.XPLocked = false;
            getScene()->camera.ZPLocked = false;
            getScene()->camera.ZNLocked = false;
        }
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_5) || getContext()->window.isKeyPressed(LIGIDGL_KEY_NUMPAD_5)){
            getScene()->camera.ZPLocked = true;
            getScene()->camera.XNLocked = false;
            getScene()->camera.YPLocked = false;
            getScene()->camera.YNLocked = false;
            getScene()->camera.XPLocked = false;
            getScene()->camera.ZNLocked = false;
        }
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_6) || getContext()->window.isKeyPressed(LIGIDGL_KEY_NUMPAD_6)){
            getScene()->camera.ZNLocked = true;
            getScene()->camera.XNLocked = false;
            getScene()->camera.YPLocked = false;
            getScene()->camera.YNLocked = false;
            getScene()->camera.ZPLocked = false;
            getScene()->camera.XPLocked = false;
        }
    }
    

    if(getScene()->camera.originLocked){        
        getScene()->camera.transition(glm::vec3(10.f, 0.f, 0.f), glm::vec3(0.f));
        getScene()->updateViewMatrix();
        getScene()->updateProjectionMatrix();

    }
    if(getScene()->camera.XPLocked){        
        getScene()->camera.transition(glm::vec3(10.f, 0.f, 0.f) + getScene()->camera.originPos);
        getScene()->updateViewMatrix();
        getScene()->updateProjectionMatrix();

    }
    if(getScene()->camera.XNLocked){        
        getScene()->camera.transition(glm::vec3(-10.f, 0.f, 0.f) + getScene()->camera.originPos);
        getScene()->updateViewMatrix();
        getScene()->updateProjectionMatrix();

    }
    if(getScene()->camera.YPLocked){        
        getScene()->camera.transition(glm::vec3(0.f, 10.f, 0.f) + getScene()->camera.originPos);
        getScene()->updateViewMatrix();
        getScene()->updateProjectionMatrix();

    }
    if(getScene()->camera.YNLocked){        
        getScene()->camera.transition(glm::vec3(0.f, -10.f, 0.f) + getScene()->camera.originPos);
        getScene()->updateViewMatrix();
        getScene()->updateProjectionMatrix();

    }
    if(getScene()->camera.ZPLocked){        
        getScene()->camera.transition(glm::vec3(0.f, 0.f, 10.f) + getScene()->camera.originPos);
        getScene()->updateViewMatrix();
        getScene()->updateProjectionMatrix();

    }
    if(getScene()->camera.ZNLocked){        
        getScene()->camera.transition(glm::vec3(0.f, 0.f, -10.f) + getScene()->camera.originPos);
        getScene()->updateViewMatrix();
        getScene()->updateProjectionMatrix();

    }

    //Render each mesh
    for (size_t i = 0; i < getModel()->meshes.size(); i++)
    {   
        /* Albedo */
        glActiveTexture(GL_TEXTURE2);
        if(painter.selectedDisplayingModeIndex == 0){
            if(NodeScene::getNode(0)->IOs[i].connections.size())
                glBindTexture(GL_TEXTURE_2D, getModel()->meshes[i].albedo.ID);
            else
                glBindTexture(GL_TEXTURE_2D, Settings::appTextures().noMaterialConnectedToTheMeshWarningImage.ID);
            }
        else if(painter.selectedDisplayingModeIndex == 1)
            glBindTexture(GL_TEXTURE_2D, userInterface.paintingPanel.sections[4].elements[0].button.texture.ID);

        /* Roughness */
        glActiveTexture(GL_TEXTURE3);
        if(painter.selectedDisplayingModeIndex == 0){
            if(NodeScene::getNode(0)->IOs[i].connections.size())
                glBindTexture(GL_TEXTURE_2D, getModel()->meshes[i].roughness.ID);
            else
                glBindTexture(GL_TEXTURE_2D, Settings::appTextures().noMaterialConnectedToTheMeshWarningImage.ID);
        }
        else if(painter.selectedDisplayingModeIndex == 1)
            glBindTexture(GL_TEXTURE_2D, userInterface.paintingPanel.sections[4].elements[1].button.texture.ID);

        /* Metallic*/
        glActiveTexture(GL_TEXTURE4);
        if(painter.selectedDisplayingModeIndex == 0){
            if(NodeScene::getNode(0)->IOs[i].connections.size())
                glBindTexture(GL_TEXTURE_2D, getModel()->meshes[i].metallic.ID);
            else
                glBindTexture(GL_TEXTURE_2D, Settings::appTextures().noMaterialConnectedToTheMeshWarningImage.ID);
        }
        else if(painter.selectedDisplayingModeIndex == 1)
            glBindTexture(GL_TEXTURE_2D, userInterface.paintingPanel.sections[4].elements[2].button.texture.ID);

        /* Normal Map*/
        glActiveTexture(GL_TEXTURE5);
        if(painter.selectedDisplayingModeIndex == 0){
            if(NodeScene::getNode(0)->IOs[i].connections.size())
                glBindTexture(GL_TEXTURE_2D, getModel()->meshes[i].normalMap.ID);
            else
                glBindTexture(GL_TEXTURE_2D, Settings::appTextures().noMaterialConnectedToTheMeshWarningImage.ID);
        }
        else if(painter.selectedDisplayingModeIndex == 1)
            glBindTexture(GL_TEXTURE_2D, userInterface.paintingPanel.sections[4].elements[3].button.texture.ID);

        /* Height Map*/
        glActiveTexture(GL_TEXTURE6);
        if(painter.selectedDisplayingModeIndex == 0){
            if(NodeScene::getNode(0)->IOs[i].connections.size())
                glBindTexture(GL_TEXTURE_2D, getModel()->meshes[i].heightMap.ID);
            else
                glBindTexture(GL_TEXTURE_2D, Settings::appTextures().noMaterialConnectedToTheMeshWarningImage.ID);
        }
        else if(painter.selectedDisplayingModeIndex == 1)
            glBindTexture(GL_TEXTURE_2D, userInterface.paintingPanel.sections[4].elements[4].button.texture.ID);

        /* Ambient Occlusion*/
        glActiveTexture(GL_TEXTURE7);
        if(painter.selectedDisplayingModeIndex == 0){
            if(NodeScene::getNode(0)->IOs[i].connections.size())
                glBindTexture(GL_TEXTURE_2D, getModel()->meshes[i].ambientOcclusion.ID);
            else
                glBindTexture(GL_TEXTURE_2D, Settings::appTextures().noMaterialConnectedToTheMeshWarningImage.ID);
        }
        else if(painter.selectedDisplayingModeIndex == 1)
            glBindTexture(GL_TEXTURE_2D, userInterface.paintingPanel.sections[4].elements[5].button.texture.ID);

        ShaderSystem::tdModelShader().setInt("paintedTxtrStateIndex", painter.selectedPaintingChannelIndex);
        
        // Painting over texture
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, painter.paintingOverTexture);
        
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
            else
                ShaderSystem::tdModelShader().setFloat("opacity", 0.2f);
        }
        else
            ShaderSystem::tdModelShader().setFloat("opacity", 1.f);

        //Draw the mesh
        getModel()->meshes[i].Draw();
    }
    
    ShaderSystem::tdModelShader().setFloat("opacity", 1.f);
    ShaderSystem::tdModelShader().setInt("paintingOverDisplayinMode", 0);

    //Clear the depth buffer before rendering the UI elements (prevent coliding)
    glClear(GL_DEPTH_BUFFER_BIT);

    //Bind 2D square vertex buffers
    box.bindBuffers();
    
    //Update the UI projection using window size
    userInterface.projection = glm::ortho(0.f,(float)getContext()->windowScale.x,(float)getContext()->windowScale.y,0.f);
    
    //Render the UI
    userInterface.render(   //Params
                            timer,
                            box,
                            project,
                            painter,
                            skybox
                        );

    //Painting
    if(*Mouse::LPressed() && !userInterface.anyContextMenuActive && !userInterface.anyPanelHover && !userInterface.anyDialogActive && (painter.selectedDisplayingModeIndex == 1 || painter.selectedDisplayingModeIndex == 2)){ //If mouse hover 3D viewport and left mouse button pressed
        //Paint
        painter.doPaint(    
                           
                            userInterface.projection
                        );

    }

    //Painting done (refresh)
    if((painter.refreshable && !*Mouse::LPressed()) || (painter.refreshable && (*Mouse::RClick() || *Mouse::MClick()))){ //Last frame painting done or once mouse right click or mouse wheel click
        /*//TODO Prevent updating all the materials
        for (size_t i = 0; i < library.materials.size(); i++)
        {   
            //Update the material after painting
            //TODO : Do smt after painting
            // library.materials[i].updateMaterial(this->settings.textureRes, box, context, shaders.buttonShader, shaders.tdModelShader);
        }
        */

        //Update the selected texture after painting
        painter.updateTexture(userInterface.twoDPaintingPanel, userInterface.projection, userInterface.twoDPaintingSceneScroll, userInterface.twoDPaintingScenePos);
        //Refresh the 2D painting texture
        painter.refreshPainting();

        painter.refreshable = false;
    }


    box.unbindBuffers(); //Finish rendering the UI


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

    //Swap the front and back buffers of the window
    getContext()->window.swapBuffers();

    _ligid_renderer_render_first_frame = false;
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
    ShaderSystem::tdModelShader().setInt("depthTexture",9);
    ShaderSystem::tdModelShader().setInt("paintingOverTexture",10);
    ShaderSystem::tdModelShader().setVec3("viewPos", getScene()->camera.cameraPos);
    ShaderSystem::tdModelShader().setMat4("view", getScene()->viewMatrix);
    ShaderSystem::tdModelShader().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::tdModelShader().setMat4("modelMatrix", getScene()->transformMatrix);

    //Shader (used to render the model with depth)
    ShaderSystem::depth3D().use();
    ShaderSystem::depth3D().setMat4("view", getScene()->viewMatrix);
    ShaderSystem::depth3D().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::depth3D().setMat4("modelMatrix",getScene()->transformMatrix);
    
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
    
    //Render the skybox
    skybox.draw(true);
}