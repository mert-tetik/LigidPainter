/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <iostream>
#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../../thirdparty/include/glm/glm.hpp"
#include "../../thirdparty/include/glm/gtc/matrix_transform.hpp"
#include "../../thirdparty/include/glm/gtc/type_ptr.hpp"
#include "../../thirdparty/include/glm/gtx/string_cast.hpp"

#include "GUI/Elements.hpp"
#include "GUI/GUI.hpp"
#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"
#include "Renderer.h"

void Renderer::render(){
    
    //Handle user input and interact with the windowing system
    glfwPollEvents();
    
    //Update local timer data
    if(timer.runTimer(1.f))
        std::cout << timer.FPS << std::endl; //Print the fps every second
    
    //Update OpenGL viewport every frame
    updateViewport();

    //VSync
    if(VSync)
        glfwSwapInterval(1); //Enable VSync
    else
        glfwSwapInterval(0); //Disable VSync

    //Default blending settings
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);
    
    //Refresh the default framebuffer    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glDisable(GL_CULL_FACE);

    //Render skybox
    renderSkyBox();

    //Set the uniforms regarding 3D models (mostly vertex shader) 
    set3DUniforms();

    //Update the depth texture if necessary
    if(painter.updateTheDepthTexture && !mouse.RPressed){ //Last frame camera changed position
        //Update the depth texture
        painter.updateDepthTexture(model,context.windowScale);

        painter.updateTheDepthTexture = false;
    }

    if(backfaceCulling)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);

    //3D Model    
    shaders.tdModelShader.use();

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

    //Get the nodes connected to the mesh node (output node)
    Util util;
    std::vector<Material> nodeMaterials = util.getTheMaterialsConnectedToTheMeshNode(nodeScene,library,0); 

//  ////Render each mesh
    for (size_t i = 0; i < model.meshes.size(); i++)
    {
        if(library.materials.size()){
            //Bind the material
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D,nodeMaterials[i].albedo.ID);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D,nodeMaterials[i].roughness.ID);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D,nodeMaterials[i].metallic.ID);
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D,nodeMaterials[i].normalMap.ID);
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_2D,nodeMaterials[i].heightMap.ID);
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D,nodeMaterials[i].ambientOcclusion.ID);
        }
        //Draw the mesh
        model.meshes[i].Draw();
    }

    //Clear the depth buffer before rendering the UI elements (prevent coliding)
    glClear(GL_DEPTH_BUFFER_BIT);

    //Bind 2D square vertex buffers
    box.bindBuffers();
    
    //Update the UI projection using window size
    userInterface.projection = glm::ortho(0.f,(float)context.windowScale.x,(float)context.windowScale.y,0.f);
    
    //Render the UI
    userInterface.render(scene.videoScale,mouse,timer,textRenderer,context,box,library,appNodes,nodeScene,contextMenus,textureRes,project,painter,VSync,backfaceCulling,skybox,model);//Render the UI

    //Painting
    if(mouse.LPressed && !userInterface.anyPanelHover && !userInterface.anyDialogActive){ //If mouse hover 3D viewport and left mouse button pressed
        //Paint
        painter.doPaint(    
                            mouse,
                            userInterface.paintingPanel.sections[1].elements[0].rangeBar.value,
                            userInterface.paintingPanel.sections[1].elements[2].rangeBar.value/10.f,
                            userInterface.paintingPanel.sections[1].elements[1].rangeBar.value/100.f,
                            userInterface.paintingPanel.sections[1].elements[3].rangeBar.value,
                            1.f - userInterface.paintingPanel.sections[2].elements[0].rangeBar.value/100.f,
                            1.f - userInterface.paintingPanel.sections[2].elements[3].rangeBar.value/100.f,
                            1.f - userInterface.paintingPanel.sections[2].elements[1].rangeBar.value/100.f,
                            userInterface.paintingPanel.sections[2].elements[7].rangeBar.value,
                            1.f - userInterface.paintingPanel.sections[2].elements[8].rangeBar.value/100.f,
                            1.f - userInterface.paintingPanel.sections[2].elements[9].rangeBar.value/100.f,
                            userInterface.paintingPanel.sections[2].elements[6].checkBox.clickState1,
                            userInterface.paintingPanel.sections[2].elements[2].checkBox.clickState1,
                            library.textures
                        );
    }

    //Painting done (refresh)
    if((painter.refreshable && !mouse.LPressed) || mouse.RClick || mouse.MClick){ //Last frame painting done or once mouse right click or mouse wheel click
        //TODO Prevent updating all the materials
        for (size_t i = 0; i < library.materials.size(); i++)
        {   
            //Update the material after painting
            userInterface.materialEditorDialog.updateMaterial(library.materials[i],textureRes,box,context);
        }
        //Update the selected texture after painting
        painter.updateTexture(library.textures,model,textureRes);
        //Refresh the 2D painting texture
        painter.refreshPainting();

        painter.refreshable = false;
    }


    box.unbindBuffers(); //Finish rendering the UI


    //Set mouse states to default
    mouse.LClick = false;
    mouse.RClick = false;
    mouse.MClick = false;
    mouse.LDoubleClick = false;
    mouse.mouseOffset = glm::vec2(0);
    mouse.mods = 0;
    mouse.mouseScroll = 0;

    //Set keyboard states to default
    textRenderer.keyInput = false;
    textRenderer.mods = 0;

    //Let the model.newModelAdded be true for an another cycle
    if(previousModelNewModelAdded == true)
        model.newModelAdded = false;

    previousModelNewModelAdded = model.newModelAdded; 

    //Cursor is changing there
    //Sets the active cursor (mouse.activeCursor) as the cursor
    //Than changes the active cursor as default cursor
    mouse.updateCursor();  

    //Swap the front and back buffers of the window
    glfwSwapBuffers(context.window);
}




void Renderer::updateViewMatrix(){
    scene.viewMatrix = glm::lookAt(scene.camera.cameraPos, 
                                    scene.camera.originPos, 
                                    glm::vec3(0.0, 1.0, 0.0));
}

void Renderer::updateProjectionMatrix(){
    if(context.windowScale.x){
        scene.projectionMatrix = glm::perspective(glm::radians(scene.fov), 
                                            (float)context.windowScale.x / (float)context.windowScale.y, //Since the ratio is determined by the window scale, 3D Model won't be stretched by window resizing.
                                            scene.near, 
                                            scene.far);
    }
}

void Renderer::updateViewport(){
    glViewport(0, 
                0, 
                context.windowScale.x, 
                context.windowScale.y);
}

void Renderer::set3DUniforms(){
    
    //3D Model Shader
    shaders.tdModelShader.use();
    shaders.tdModelShader.setInt("useTransformUniforms",0);
    shaders.tdModelShader.setInt("render2D", 0);
    shaders.tdModelShader.setInt("skybox",0);
    shaders.tdModelShader.setInt("prefilterMap",1);
    shaders.tdModelShader.setInt("albedoTxtr",2);
    shaders.tdModelShader.setInt("roughnessTxtr",3);
    shaders.tdModelShader.setInt("metallicTxtr",4);
    shaders.tdModelShader.setInt("normalMapTxtr",5);
    shaders.tdModelShader.setInt("heightMapTxtr",6);
    shaders.tdModelShader.setInt("ambientOcclusionTxtr",7);
    shaders.tdModelShader.setInt("paintingTexture",8);
    shaders.tdModelShader.setInt("depthTexture",9);
    shaders.tdModelShader.setVec3("viewPos",scene.camera.cameraPos);
    shaders.tdModelShader.setMat4("view",scene.viewMatrix);
    shaders.tdModelShader.setMat4("projection",scene.projectionMatrix);
    glm::mat4 modelMatrix = glm::mat4(1);
    shaders.tdModelShader.setMat4("modelMatrix",modelMatrix);

    //Shader (used to render the model with depth)
    shaders.depth3D.use();
    shaders.depth3D.setMat4("view",scene.viewMatrix);
    shaders.depth3D.setMat4("projection",scene.projectionMatrix);

    //Skybox ball shader 
    shaders.skyboxBall.use();
    shaders.skyboxBall.setMat4("view",scene.viewMatrix);
    shaders.skyboxBall.setMat4("projection",scene.projectionMatrix);

}





//UTILITY FUNCTIONS


void Renderer::renderSkyBox(){
    
    //Skybox shader
    shaders.skyboxShader.use();
    shaders.skyboxShader.setMat4("view",scene.viewMatrix);
    shaders.skyboxShader.setMat4("projection",scene.projectionMatrix);
    shaders.skyboxShader.setMat4("transformMatrix",skybox.transformMatrix);
    shaders.skyboxShader.setFloat("lod",skybox.lod);
    shaders.skyboxShader.setVec3("bgColor",skybox.bgColor);
    shaders.skyboxShader.setFloat("opacity",skybox.opacity);
    shaders.skyboxShader.setInt("skybox",0);
    
    //Render the skybox
    skybox.draw(true);
}