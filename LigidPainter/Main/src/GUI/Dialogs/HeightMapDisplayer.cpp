
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 (c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
 Official Web Page : https:ligidtools.com/ligidpainter

 ---------------------------------------------------------------------------
 */

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "3D/ThreeD.hpp" 

#include "GUI/GUI.hpp" 

#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "UTIL/Threads/Threads.hpp"
#include "UTIL/GL/GL.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

static void set_cam_to_default(Camera* cam){
    cam->setCameraPosition(glm::vec3(5.f, 1.f, 0.f));
}

HeightMapDisplayerDialog::HeightMapDisplayerDialog(int){
    //Create the panel
    this->display1 = Button(ELEMENT_STYLE_SOLID, glm::vec2(20, 40), "", Texture((char*)nullptr, 1024, 1024), 0.f, false);
    this->display2 = Button(ELEMENT_STYLE_SOLID, glm::vec2(20, 40), "", Texture((char*)nullptr, 1024, 1024), 0.f, false);
    
    this->exit_btn = Button(ELEMENT_STYLE_SOLID, glm::vec2(1.5f), "", appTextures.X, 0.f, false);

    this->strength_rangebar = RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(6.f, 1.5f), "Strength", Texture(), 0.f, 0.f, 1.f, 0.5f);
}

float prev_strength = 0.5f;

void HeightMapDisplayerDialog::show(Timer& timer, Model* model){
    
    this->dialogControl.activate();
    
    set_cam_to_default(&this->display1_cam);
    set_cam_to_default(&this->display2_cam);

    for (Mesh& mesh : model->meshes)
    {
        mesh.processHeightMap(this->strength_rangebar.value);
    }
    
    while (!getContext()->window.shouldClose())
    {
        this->dialogControl.updateStart(true);

        if(this->display1.hover)
            this->display1_cam.interaction(*Mouse::mouseScroll(), *Mouse::mouseOffset(), true);
        if(this->display2.hover)
            this->display2_cam.interaction(*Mouse::mouseScroll(), *Mouse::mouseOffset(), true);

        this->display1.pos = glm::vec3(50.f - this->display1.scale.x, 50.f, 0.7f);
        this->display2.pos = glm::vec3(50.f + this->display2.scale.x, 50.f, 0.7f);

        this->display1.render(timer, !dialog_log.isHovered());
        this->display2.render(timer, !dialog_log.isHovered());
        
        glClear(GL_DEPTH_BUFFER_BIT);

        this->exit_btn.pos = glm::vec3(this->display2.pos.x + this->display2.scale.x - this->exit_btn.scale.x, this->display2.pos.y - this->display2.scale.y + this->exit_btn.scale.y, this->display2.pos.z);
        this->exit_btn.render(timer, !dialog_log.isHovered());

        this->strength_rangebar.pos = glm::vec3(this->display1.pos.x, this->display1.pos.y + this->display1.scale.y - this->strength_rangebar.scale.y - 2.f, this->display1.pos.z + 0.01f);
        this->strength_rangebar.render(timer, !dialog_log.isHovered());


        if(!strength_rangebar.pointerPressed && prev_strength != this->strength_rangebar.value){
            for (Mesh& mesh : model->meshes)
            {
                mesh.processHeightMap(this->strength_rangebar.value);
            }
        }

        prev_strength = this->strength_rangebar.value;
        
        glm::mat4 projectionMatrix = glm::perspective(
                                                        glm::radians(35.f), //Fov  
                                                        1.f,  //
                                                        0.1f, //Near
                                                        100.f //Far
                                                    );

        {
            Framebuffer FBO = FBOPOOL::requestFBO_with_RBO(this->display1.texture, this->display1.texture.getResolution(), "HeightMapDisplayerDialog display 1");
            ShaderSystem::PBRDisplayOnly().use();

            //Clear the capture framebuffer (displaying texture) with alpha zero color
            glClearColor(0,0,0,0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            //Use the 3D model rendering shader
            ShaderSystem::PBRDisplayOnly().use();
            ShaderSystem::PBRDisplayOnly().setMat4("view", this->display1_cam.viewMatrix);
            ShaderSystem::PBRDisplayOnly().setMat4("projection", projectionMatrix);
            ShaderSystem::PBRDisplayOnly().setMat4("modelMatrix", glm::mat4(1.f));
            ShaderSystem::PBRDisplayOnly().setVec3("viewPos", this->display1_cam.cameraPos);
            
            for (size_t i = 0; i < model->meshes.size(); i++)
            {
                ShaderSystem::PBRDisplayOnly().setInt("skybox", GL::bindTexture_CubeMap(getScene()->skybox.ID, "HeightMapDisplayerDialog display 1")); 
                ShaderSystem::PBRDisplayOnly().setInt("prefilterMap", GL::bindTexture_CubeMap(getScene()->skybox.IDPrefiltered, "HeightMapDisplayerDialog display 1")); 
                ShaderSystem::PBRDisplayOnly().setInt("albedoTxtr", GL::bindTexture_2D((model->meshes[i].layerScene.layers.size()) ? model->meshes[i].material_channels.albedo.ID : appTextures.noLayersWarningTexture.ID, "HeightMapDisplayerDialog display 1"));
                ShaderSystem::PBRDisplayOnly().setInt("roughnessTxtr", GL::bindTexture_2D((model->meshes[i].layerScene.layers.size()) ? model->meshes[i].material_channels.roughness.ID : appTextures.white.ID, "HeightMapDisplayerDialog display 1"));
                ShaderSystem::PBRDisplayOnly().setInt("metallicTxtr", GL::bindTexture_2D((model->meshes[i].layerScene.layers.size()) ? model->meshes[i].material_channels.metallic.ID : appTextures.black.ID, "HeightMapDisplayerDialog display 1"));
                ShaderSystem::PBRDisplayOnly().setInt("normalMapTxtr", GL::bindTexture_2D((model->meshes[i].layerScene.layers.size()) ? model->meshes[i].material_channels.normalMap.ID : appTextures.noLayersWarningTexture.ID, "HeightMapDisplayerDialog display 1"));
                ShaderSystem::PBRDisplayOnly().setInt("heightMapTxtr", GL::bindTexture_2D((model->meshes[i].layerScene.layers.size()) ? model->meshes[i].material_channels.heightMap.ID : appTextures.white.ID, "HeightMapDisplayerDialog display 1"));
                ShaderSystem::PBRDisplayOnly().setInt("ambientOcclusionTxtr", GL::bindTexture_2D((model->meshes[i].layerScene.layers.size()) ? model->meshes[i].material_channels.ambientOcclusion.ID : appTextures.white.ID, "HeightMapDisplayerDialog display 1"));
                ShaderSystem::PBRDisplayOnly().setInt("displayingMode", 0);
                
                //Draw the sphere
                model->meshes[i].draw_height_map_processed_vertices("");
                
                GL::releaseBoundTextures("HeightMapDisplayerDialog display 1");
            }
            
            ShaderSystem::PBRDisplayOnly().setInt("displayingMode", 0);
            ShaderUTIL::release_bound_shader();

            FBOPOOL::releaseFBO(FBO);
        }
        
        {
            Framebuffer FBO = FBOPOOL::requestFBO_with_RBO(this->display2.texture, this->display2.texture.getResolution(), "HeightMapDisplayerDialog display 2");
            ShaderSystem::PBRDisplayOnly().use();

            //Clear the capture framebuffer (displaying texture) with alpha zero color
            glClearColor(0,0,0,0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            //Use the 3D model rendering shader
            ShaderSystem::solidShadingShader().use();
            ShaderSystem::solidShadingShader().setMat4("view", this->display2_cam.viewMatrix);
            ShaderSystem::solidShadingShader().setMat4("projection", projectionMatrix);
            ShaderSystem::solidShadingShader().setMat4("modelMatrix", glm::mat4(1.f));
            ShaderSystem::solidShadingShader().setVec3("viewPos", this->display2_cam.cameraPos);
            
            for (size_t i = 0; i < model->meshes.size(); i++)
            {
                ShaderSystem::solidShadingShader().setInt("use_txtr", true);
                ShaderSystem::solidShadingShader().setInt("txtr", model->meshes[i].material_channels.heightMap.ID);

                //Draw the sphere
                model->meshes[i].Draw("HeightMapDisplayerDialog display 2");
                
                GL::releaseBoundTextures("HeightMapDisplayerDialog display 2");
            }
            
            ShaderUTIL::release_bound_shader();
            FBOPOOL::releaseFBO(FBO);
        }


        //Close the dialog
        if(
                getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || 
                (!display1.hover && !display2.hover && !dialog_log.isHovered() && *Mouse::LClick()) ||
                this->exit_btn.clicked
            )
        {
            if(!dialogControl.firstFrameActivated){
                this->dialogControl.unActivate();
            }
        }
        
        dialogControl.updateEnd(timer, 0.15f);

        if(dialogControl.mixVal == 0.f)
            break;
    }
}