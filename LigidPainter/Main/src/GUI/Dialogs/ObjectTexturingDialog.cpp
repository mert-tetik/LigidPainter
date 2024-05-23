
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 (c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
 Official Web Page : https:ligidtools.com/ligidpainter

 ---------------------------------------------------------------------------
 */
 
#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "GUI/GUI.hpp"

#include "UTIL/Library/Library.hpp"
#include "UTIL/Mouse/Mouse.hpp" 
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "UTIL/GL/GL.hpp"
#include "UTIL/Threads/Threads.hpp"

#include <string>
#include <iostream>
#include <vector>

#include "tinyfiledialogs.h"

#define DISPLAY_RESOLUTION glm::vec2(this->panel.scale * 30.f)

static std::vector<Mesh> meshTxtrHistory;
static bool albedoHistoryUsed = false;
static bool roughnessHistoryUsed = false;
static bool metallicHistoryUsed = false;
static bool normalMapHistoryUsed = false;
static bool heightMapHistoryUsed = false;
static bool aoHistoryUsed = false;

ObjectTexturingDialog::ObjectTexturingDialog(int){
    //Create the panel
    this->panel = Panel(
        {
            {
                Section(
                    Element(Button()),
                    {
                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,40.f), ""  , Texture(), 1.f, false)),
                    }
                )
            }
        },
        glm::vec2(40.f),
        glm::vec3(50.f,50.f,0.8f),
        glm::vec4(0.),
        glm::vec4(0.),
        true,
        true,
        true,
        true,
        true,
        1.f,
        1.f,
        {},
        0.25f,
        false
    );

    this->maskMaterialBtn = Button(ELEMENT_STYLE_BASIC, glm::vec2(6, 2.f), "Material mask", Texture(), 1.f, false);
    this->maskMaterialBtn.textureSelection3D = true;
    this->maskMaterialBtn.texture.proceduralProps.proceduralID = 24;
    
    this->displayMaskCheckBox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(6.f, 2.f), "Display Mask", 0.f);
    this->displayMaskCheckBox.clickState1 = true;

    this->assignRelatedTexturesButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(6, 2.f), "Assign to related textures", Texture(), 1.f, false);
    
    this->albedoChannelCheckBox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(6.f, 2.f), "Albedo", 0.f);
    this->albedoChannelCheckBox.clickState1 = true;
    this->albedoChannelCheckBox.infoText = "Enable / Disable modifying the Albedo channel";
    this->roughnessChannelCheckBox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(6.f, 2.f), "Roughness", 0.f);
    this->roughnessChannelCheckBox.clickState1 = true;
    this->roughnessChannelCheckBox.infoText = "Enable / Disable modifying the Roughness channel";
    this->metallicChannelCheckBox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(6.f, 2.f), "Metallic", 0.f);
    this->metallicChannelCheckBox.clickState1 = true;
    this->metallicChannelCheckBox.infoText = "Enable / Disable modifying the Metallic channel";
    this->normalMapChannelCheckBox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(6.f, 2.f), "Normal Map", 0.f);
    this->normalMapChannelCheckBox.clickState1 = true;
    this->normalMapChannelCheckBox.infoText = "Enable / Disable modifying the Normal Map channel";
    this->heightMapChannelCheckBox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(6.f, 2.f), "Height Map", 0.f);
    this->heightMapChannelCheckBox.clickState1 = true;
    this->heightMapChannelCheckBox.infoText = "Enable / Disable modifying the Height Map channel";
    this->aoChannelCheckBox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(6.f, 2.f), "Ambient Occlusion", 0.f);
    this->aoChannelCheckBox.clickState1 = true;
    this->aoChannelCheckBox.infoText = "Enable / Disable modifying the Ambient Occlusion channel";
    
    this->displayingOptionsComboBox = ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), {"PBR", "Albedo", "Roughness", "Metallic", "Normal Map", "Heigth Map", "Ambient Occlusion"}, "Displaying Mode",  1.f);
    this->displayBuffersComboBox = ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), {"Complete Layer Result", "Selected Layer Only"}, "Displaying Buffers",  1.f);

    this->displayingTexture = Texture((char*)nullptr, DISPLAY_RESOLUTION.x, DISPLAY_RESOLUTION.y);

    this->sceneCam.setCameraPosition(glm::vec3(0,0,-3.5f));
    this->sceneCam.radius = 3.5f;

    this->material = Material("ObjectTexturingMaterial_DisplayingTexture", {MaterialModifier(SOLID_MATERIAL_MODIFIER)});

}

void ObjectTexturingDialog::show(Timer& timer, Mesh& mesh, PaintingLayer* painting_layer, const unsigned int resolution){
        
    dialogControl.activate();
    
    while(!getContext()->window.shouldClose()){

        dialogControl.updateStart(true);

        this->sceneCam.interaction(*Mouse::mouseScroll(), *Mouse::mouseOffset(), false);

        if(dialogControl.firstFrameActivated){
            material_thread.update_material_displaying_texture(&this->material, getMaterialDisplayerModel(), &getMaterialDisplayerModel()->meshes[0], &getMaterialDisplayerModel()->meshes[0].material_channels);

            this->sceneCam.setCameraPosition(glm::vec3(0,0,-3.5f));
            this->sceneCam.radius = 3.5f;
        }

        // Update the scene texture
        this->updateDisplayingTexture(mesh, painting_layer);

        // Modifying the elements    
        this->panel.sections[0].elements[0].button.scale = this->panel.scale;

        this->maskMaterialBtn.pos = this->panel.pos;
        this->maskMaterialBtn.pos.x -= this->panel.scale.x - this->maskMaterialBtn.scale.x - 2.f;
        this->maskMaterialBtn.pos.y -= 6.f;
        
        this->displayMaskCheckBox.pos = maskMaterialBtn.pos;
        this->displayMaskCheckBox.pos.y += displayMaskCheckBox.scale.y + maskMaterialBtn.scale.y * 1.f;

        this->assignRelatedTexturesButton.pos = displayMaskCheckBox.pos;
        this->assignRelatedTexturesButton.pos.y += assignRelatedTexturesButton.scale.y + displayMaskCheckBox.scale.y * 2.f;
        
        this->albedoChannelCheckBox.pos = this->assignRelatedTexturesButton.pos;
        this->albedoChannelCheckBox.pos.y += this->albedoChannelCheckBox.scale.y + this->assignRelatedTexturesButton.scale.y * 2.f;
        this->roughnessChannelCheckBox.pos = this->albedoChannelCheckBox.pos;
        this->roughnessChannelCheckBox.pos.y += this->roughnessChannelCheckBox.scale.y + this->albedoChannelCheckBox.scale.y;
        this->metallicChannelCheckBox.pos = this->roughnessChannelCheckBox.pos;
        this->metallicChannelCheckBox.pos.y += this->metallicChannelCheckBox.scale.y + this->roughnessChannelCheckBox.scale.y;
        this->normalMapChannelCheckBox.pos = this->metallicChannelCheckBox.pos;
        this->normalMapChannelCheckBox.pos.y += this->normalMapChannelCheckBox.scale.y + this->metallicChannelCheckBox.scale.y;
        this->heightMapChannelCheckBox.pos = this->normalMapChannelCheckBox.pos;
        this->heightMapChannelCheckBox.pos.y += this->heightMapChannelCheckBox.scale.y + this->normalMapChannelCheckBox.scale.y;
        this->aoChannelCheckBox.pos = this->heightMapChannelCheckBox.pos;
        this->aoChannelCheckBox.pos.y += this->aoChannelCheckBox.scale.y + this->heightMapChannelCheckBox.scale.y;

        this->displayingOptionsComboBox.pos = this->panel.pos;
        this->displayingOptionsComboBox.pos.y -= this->panel.scale.y - this->displayingOptionsComboBox.scale.y * 2.f;
        this->displayingOptionsComboBox.pos.x += this->displayingOptionsComboBox.scale.x + 2.f / 2.f;
        this->displayBuffersComboBox.pos = this->displayingOptionsComboBox.pos;
        this->displayBuffersComboBox.pos.x -= this->displayingOptionsComboBox.scale.x + this->displayBuffersComboBox.scale.x + 2.f;


        //Render the panel
        this->panel.render(timer, false);

        ShaderSystem::textureRenderingShader().use();
        ShaderSystem::textureRenderingShader().setMat4("projection", getContext()->ortho_projection);
        ShaderSystem::textureRenderingShader().setVec2("scale", this->panel.resultScale);
        ShaderSystem::textureRenderingShader().setVec3("pos", this->panel.resultPos);
        ShaderSystem::textureRenderingShader().setInt("txtr", GL::bindTexture_2D(this->displayingTexture.ID, "ObjectTexturingDialog::show : Rendering displaying texture"));
        ShaderSystem::textureRenderingShader().setFloat("opacity", this->dialogControl.mixVal);
        ShaderSystem::textureRenderingShader().setFloat("rotation", 0.f);
        ShaderSystem::textureRenderingShader().setFloat("depthToleranceValue", 0);

        getBox()->draw("Object texturing dialog : Rendering displaying texture");

        GL::releaseBoundTextures("ObjectTexturingDialog::show : Rendering displaying texture");
        ShaderUTIL::release_bound_shader();

        // Render the elements
        this->maskMaterialBtn.render(timer, true);
        this->displayMaskCheckBox.render(timer, true);
        this->assignRelatedTexturesButton.render(timer, true);

        this->albedoChannelCheckBox.render(timer, true);
        this->roughnessChannelCheckBox.render(timer, true);
        this->metallicChannelCheckBox.render(timer, true);
        this->normalMapChannelCheckBox.render(timer, true);
        this->heightMapChannelCheckBox.render(timer, true);
        this->aoChannelCheckBox.render(timer, true);

        this->displayingOptionsComboBox.render(timer, true);
        this->displayBuffersComboBox.render(timer, true);

        if(this->maskMaterialBtn.clicked || !this->meshMask.ID || resolution != this->meshMask.getResolution().x || this->dialogControl.firstFrameActivated){
            // Create the mask texture
            if(!this->meshMask.ID){
                this->meshMask = Texture((char*)nullptr, 1, 1);
            }

            this->meshMask.update((char*)nullptr, resolution, resolution);
            maskMaterialBtn.texture.generateProceduralTexture(*getScene()->get_selected_mesh(), this->meshMask, resolution);
        }

        if(assignRelatedTexturesButton.clicked){
            dialog_materialSelection.show(timer, &this->material);    
         
            material_thread.update_object_texturing_dialog_result(&this->material, getScene()->model, &mesh, &painting_layer->result, this->meshMask);
        }

        if(!this->panel.hover && *Mouse::LClick() && !dialog_log.isHovered() || (getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) && textRenderer.keyInput)){
            this->dialogControl.unActivate();
        }
        
        dialogControl.updateEnd(timer,0.15f);
        if(dialogControl.mixVal == 0.f)
            break;
    }
}



void ObjectTexturingDialog::updateDisplayingTexture(Mesh& mesh, PaintingLayer* painting_layer){
    
    //Move the camera to the side
    glm::mat4 view = glm::lookAt(this->sceneCam.cameraPos, 
                                 this->sceneCam.originPos, 
                                 glm::vec3(0.0, 1.0, 0.0));
    
    //The perspective projection matrix    
    glm::mat4 projectionMatrix = glm::perspective(
                                                    glm::radians(30.f), //Fov  
                                                    (float)getContext()->windowScale.x / (float)getContext()->windowScale.y,  //Ratio (is 1 since the width & the height is equal to DISPLAY_RESOLUTION)
                                                    0.1f,   //Near
                                                    1000.f  //Far (the material is pretty close to the camera actually  ) 
                                                );
    
    Framebuffer FBO = FBOPOOL::requestFBO_with_RBO(this->displayingTexture, this->displayingTexture.getResolution(), "ObjectTexturingDialog::updateDisplayingTexture");
    
    //Set the OpenGL viewport to the resolution of the material displaying texture
    glViewport(0,0, this->displayingTexture.getResolution().x, this->displayingTexture.getResolution().y);

    //Clear the capture framebuffer (displaying texture) with alpha zero color
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    MaterialChannels mat_channels;
    if(this->displayBuffersComboBox.selectedIndex == 0){
        mat_channels = mesh.material_channels;
    }
    else{
        mat_channels = painting_layer->result;
    }

    // Set shader
    ShaderSystem::objectTexturingDialogPBR().use();
    ShaderSystem::objectTexturingDialogPBR().setMat4("view", this->sceneCam.viewMatrix);
    ShaderSystem::objectTexturingDialogPBR().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::objectTexturingDialogPBR().setMat4("modelMatrix", glm::mat4(1.f));
    
    ShaderSystem::objectTexturingDialogPBR().setVec3("viewPos", this->sceneCam.cameraPos);
    ShaderSystem::objectTexturingDialogPBR().setInt("skybox", GL::bindTexture_CubeMap(getScene()->skybox.ID, "Texture::renderMesh"));
    ShaderSystem::objectTexturingDialogPBR().setInt("prefilterMap", GL::bindTexture_CubeMap(getScene()->skybox.IDPrefiltered, "Texture::renderMesh"));
    
    ShaderSystem::objectTexturingDialogPBR().setInt("albedoTxtr", GL::bindTexture_2D(mat_channels.albedo.ID, "Texture::renderMesh"));
    ShaderSystem::objectTexturingDialogPBR().setInt("roughnessTxtr", GL::bindTexture_2D(mat_channels.roughness.ID, "Texture::renderMesh"));
    ShaderSystem::objectTexturingDialogPBR().setInt("metallicTxtr", GL::bindTexture_2D(mat_channels.metallic.ID, "Texture::renderMesh"));
    ShaderSystem::objectTexturingDialogPBR().setInt("normalMapTxtr", GL::bindTexture_2D(mat_channels.normalMap.ID, "Texture::renderMesh"));
    ShaderSystem::objectTexturingDialogPBR().setInt("heightMapTxtr", GL::bindTexture_2D(mat_channels.heightMap.ID, "Texture::renderMesh"));
    ShaderSystem::objectTexturingDialogPBR().setInt("ambientOcclusionTxtr", GL::bindTexture_2D(mat_channels.ambientOcclusion.ID, "Texture::renderMesh"));

    ShaderSystem::objectTexturingDialogPBR().setInt("displayingMode", 0);
    
    ShaderSystem::objectTexturingDialogPBR().setInt("mask_texture", GL::bindTexture_2D((this->displayMaskCheckBox.clickState1) ? this->meshMask.ID : appTextures.white.ID, "Texture::renderMesh"));
    ShaderSystem::objectTexturingDialogPBR().setInt("channelI", this->displayingOptionsComboBox.selectedIndex);

    // Render result
    mesh.Draw("Texture::render_mesh");

    //!Finish (prepare rendering the GUI)
    GL::releaseBoundTextures("Texture::render_mesh"); // Release bound textures
    ShaderUTIL::release_bound_shader();
    FBOPOOL::releaseFBO(FBO);
}


int ObjectTexturingDialog::getResolution(){
    for (size_t i = 0; i < getScene()->model->meshes.size(); i++)
    {
        if(getScene()->model->meshes[i].material_channels.albedo.ID)
            return getScene()->model->meshes[i].material_channels.albedo.getResolution().x;

        else if(getScene()->model->meshes[i].material_channels.roughness.ID)
            return getScene()->model->meshes[i].material_channels.roughness.getResolution().x;

        else if(getScene()->model->meshes[i].material_channels.metallic.ID)
            return getScene()->model->meshes[i].material_channels.metallic.getResolution().x;

        else if(getScene()->model->meshes[i].material_channels.normalMap.ID)
            return getScene()->model->meshes[i].material_channels.normalMap.getResolution().x;

        else if(getScene()->model->meshes[i].material_channels.heightMap.ID)
            return getScene()->model->meshes[i].material_channels.heightMap.getResolution().x;

        else if(getScene()->model->meshes[i].material_channels.ambientOcclusion.ID)
            return getScene()->model->meshes[i].material_channels.ambientOcclusion.getResolution().x;
    
        else
            return 1024;
    }

    Settings::defaultFramebuffer()->FBO.bind();
    
}