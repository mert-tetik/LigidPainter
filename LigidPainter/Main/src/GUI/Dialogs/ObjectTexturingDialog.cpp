
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

    this->elementSelectionPanel = Panel(
        {
            {
                Section()
            }
        },
        glm::vec2(15.f, 35.f),
        glm::vec3(50.f,50.f,0.8f),
        ColorPalette::secondColor,
        ColorPalette::thirdColor,
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

    this->assignRelatedTexturesButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(6, 2.f), "Assign to related textures", Texture(), 1.f, false);
    this->undoButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(3, 2.f), "Undo", Texture(), 1.f, false);

    this->materialDisplayerButton = Button(ELEMENT_STYLE_SOLID, glm::vec2(6, 4.f), "", Texture(), 1.f, false);
    this->editMaterialButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(6, 3.f), "Edit material", Texture(), 1.f, false);
    this->selectMaterialButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(2.f, 4.f), "", appTextures.arrowB, 1.f, false);
    this->selectMaterialButton.infoText = "Select a custom material from the library";
    this->defaultMaterialButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(2.f, 3.f), "", appTextures.X, 1.f, false);
    this->defaultMaterialButton.infoText = "Switch to default (solid) material";
    
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
    this->mixOptionsComboBox = ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(6.f, 1.5f), {"Opaque", "Black Only", "White Only", "Roughness Mask", "Metallic Mask", "AO Mask", "Height Map Mask", "Normal Map Mask", "Normal Map Mask Inverted"}, "Mix",  1.f);
    this->displayingOptionsComboBox = ComboBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f, 2.f), {"PBR", "Albedo", "Roughness", "Metallic", "Normal Map", "Heigth Map", "Ambient Occlusion"}, "Displaying",  1.f);

    this->displayingTexture = Texture((char*)nullptr, DISPLAY_RESOLUTION.x, DISPLAY_RESOLUTION.y);

    this->sceneCam.setCameraPosition(glm::vec3(0,0,-3.5f));
    this->sceneCam.radius = 3.5f;

    this->material = Material("ObjectTexturingMaterial_DisplayingTexture", {MaterialModifier(SOLID_MATERIAL_MODIFIER)});
}

static bool __materialEditBtn = false;
static bool __materialSelectBtn = false;

void ObjectTexturingDialog::show(Timer& timer){
        
    dialogControl.activate();
    
    while(!getContext()->window.shouldClose()){

        dialogControl.updateStart();

        this->sceneCam.interaction(*Mouse::mouseScroll(), *Mouse::mouseOffset());

        if(getScene()->model->meshes.size() != result_channels.size()){
            for (size_t i = 0; i < result_channels.size(); i++)
            {
                glDeleteTextures(1, &result_channels[i].albedo.ID);
                glDeleteTextures(1, &result_channels[i].roughness.ID);
                glDeleteTextures(1, &result_channels[i].metallic.ID);
                glDeleteTextures(1, &result_channels[i].normalMap.ID);
                glDeleteTextures(1, &result_channels[i].heightMap.ID);
                glDeleteTextures(1, &result_channels[i].ambientOcclusion.ID);
            }

            result_channels.clear();

            for (size_t i = 0; i < getScene()->model->meshes.size(); i++)
            {
                MaterialChannels channel;
                channel.albedo = Texture((char*)nullptr, 1024, 1024);
                channel.roughness = Texture((char*)nullptr, 1024, 1024);
                channel.metallic = Texture((char*)nullptr, 1024, 1024);
                channel.normalMap = Texture((char*)nullptr, 1024, 1024);
                channel.heightMap = Texture((char*)nullptr, 1024, 1024);
                channel.ambientOcclusion = Texture((char*)nullptr, 1024, 1024);
            
                result_channels.push_back(channel);
            }
        }

        if(dialogControl.firstFrameActivated){
            this->material.updateMaterialDisplayingTexture(256, true, Camera(), 0, false);
            
            this->sceneCam.setCameraPosition(glm::vec3(0,0,-3.5f));
            this->sceneCam.radius = 3.5f;
            this->updateMeshTextures();
        }

        // Update the scene texture
        updateDisplayingTexture();

        // Modifying the elements    
        this->panel.sections[0].elements[0].button.scale = this->panel.scale;

        this->materialDisplayerButton.texture = this->material.displayingTexture;
        
        this->maskMaterialBtn.pos = this->panel.pos;
        this->maskMaterialBtn.pos.x -= this->panel.scale.x - this->maskMaterialBtn.scale.x - 2.f;
        this->maskMaterialBtn.pos.y += 15.f;
        
        this->assignRelatedTexturesButton.pos = maskMaterialBtn.pos;
        this->assignRelatedTexturesButton.pos.y += assignRelatedTexturesButton.scale.y + maskMaterialBtn.scale.y * 2.f;
        
        this->undoButton.pos = assignRelatedTexturesButton.pos;
        this->undoButton.pos.x += assignRelatedTexturesButton.scale.x + undoButton.scale.x * 1.5f;

        this->materialDisplayerButton.pos = this->panel.pos;
        this->materialDisplayerButton.pos.x += this->panel.scale.x - this->materialDisplayerButton.scale.x - 6.f;
        this->materialDisplayerButton.pos.y -= 25.f;
        this->editMaterialButton.pos = this->materialDisplayerButton.pos;
        this->editMaterialButton.pos.y += this->editMaterialButton.scale.y + this->materialDisplayerButton.scale.y;
        this->selectMaterialButton.pos = this->materialDisplayerButton.pos;
        this->selectMaterialButton.pos.x += this->selectMaterialButton.scale.x + this->materialDisplayerButton.scale.x;
        this->defaultMaterialButton.pos = this->selectMaterialButton.pos;
        this->defaultMaterialButton.pos.y += this->selectMaterialButton.scale.y + this->defaultMaterialButton.scale.y;

        this->albedoChannelCheckBox.pos = this->editMaterialButton.pos;
        this->albedoChannelCheckBox.pos.y += this->albedoChannelCheckBox.scale.y + this->editMaterialButton.scale.y * 2.f;
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
        this->mixOptionsComboBox.pos = this->aoChannelCheckBox.pos;
        this->mixOptionsComboBox.pos.y += this->mixOptionsComboBox.scale.y + this->aoChannelCheckBox.scale.y * 2.f;

        this->displayingOptionsComboBox.pos = this->panel.pos;
        this->displayingOptionsComboBox.pos.y -= this->panel.scale.y - this->displayingOptionsComboBox.scale.y * 2.f;


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

        ShaderSystem::buttonShader().use();

        // Render the elements
        this->maskMaterialBtn.render(timer, !this->materialSelection && !this->textureSelection);
        this->assignRelatedTexturesButton.render(timer, !this->materialSelection && !this->textureSelection);
        if(albedoHistoryUsed || roughnessHistoryUsed || metallicHistoryUsed || normalMapHistoryUsed || heightMapHistoryUsed || aoHistoryUsed)
            this->undoButton.render(timer, !this->materialSelection && !this->textureSelection);
        this->materialDisplayerButton.render(timer, false);
        this->editMaterialButton.render(timer, !this->materialSelection && !this->textureSelection);
        this->selectMaterialButton.render(timer, !this->materialSelection && !this->textureSelection);
        this->defaultMaterialButton.render(timer, !this->materialSelection && !this->textureSelection);
        this->albedoChannelCheckBox.render(timer, !this->materialSelection && !this->textureSelection);
        this->roughnessChannelCheckBox.render(timer, !this->materialSelection && !this->textureSelection);
        this->metallicChannelCheckBox.render(timer, !this->materialSelection && !this->textureSelection);
        this->normalMapChannelCheckBox.render(timer, !this->materialSelection && !this->textureSelection);
        this->heightMapChannelCheckBox.render(timer, !this->materialSelection && !this->textureSelection);
        this->aoChannelCheckBox.render(timer, !this->materialSelection && !this->textureSelection);
        this->mixOptionsComboBox.render(timer, !this->materialSelection && !this->textureSelection);

        this->displayingOptionsComboBox.render(timer, !this->materialSelection && !this->textureSelection);

        if(this->maskMaterialBtn.clicked || mixOptionsComboBox.selectionDone || ((albedoChannelCheckBox.hover || roughnessChannelCheckBox.hover || metallicChannelCheckBox.hover || normalMapChannelCheckBox.hover || heightMapChannelCheckBox.hover || aoChannelCheckBox.hover) && *Mouse::LClick())){
            this->updateMeshTextures();
        }

        // Element interactions
        if(this->selectMaterialButton.clicked){
            dialog_materialSelection.show(timer, &this->material);
            __materialSelectBtn = true;
        }
        else if(this->defaultMaterialButton.clicked){
            this->material.deleteBuffers();
            this->material = Material("ObjectTexturingMaterial_DisplayingTexture", {MaterialModifier(SOLID_MATERIAL_MODIFIER)});
            this->updateMeshTextures();
        }
        else if(this->editMaterialButton.clicked){
            dialog_materialEditor.show(timer, &this->material);
            __materialEditBtn = true;
        }
        else if(assignRelatedTexturesButton.clicked){
            for (size_t i = 0; i < getScene()->model->meshes.size(); i++)
            {
                for (size_t channelI = 0; channelI < 6; channelI++)
                {
                    skipChannel:
                    
                    glDisable(GL_DEPTH_TEST);
                    Texture colorBuffer;
                    std::string colorBufferChannelTitle = ""; 
                    if(channelI == 0){
                        colorBuffer = getScene()->model->meshes[i].albedo.ID;
                        colorBufferChannelTitle = "albedo";
                    }
                    if(channelI == 1){
                        colorBuffer = getScene()->model->meshes[i].roughness.ID;
                        colorBufferChannelTitle = "roughness";
                    }
                    if(channelI == 2){
                        colorBuffer = getScene()->model->meshes[i].metallic.ID;
                        colorBufferChannelTitle = "metallic";
                    }
                    if(channelI == 3){
                        colorBuffer = getScene()->model->meshes[i].normalMap.ID;
                        colorBufferChannelTitle = "normalMap";
                    }
                    if(channelI == 4){
                        colorBuffer = getScene()->model->meshes[i].heightMap.ID;
                        colorBufferChannelTitle = "heightMap";
                    }
                    if(channelI == 5){
                        colorBuffer = getScene()->model->meshes[i].ambientOcclusion.ID;
                        colorBufferChannelTitle = "ambientOcclusion";
                    }

                    if(!colorBuffer.ID || glIsTexture(colorBuffer.ID) == GL_FALSE){
                        if(!colorBuffer.ID)
                            LGDLOG::start << "WARNING! : Texture is missing for the " << colorBufferChannelTitle << " channel!" << LGDLOG::end;
                        else
                            LGDLOG::start << "WARNING! : Texture is invalid (OpenGL couldn't detect) for the " << colorBufferChannelTitle << " channel!" << LGDLOG::end;
                        
                        channelI++;
                        
                        if(channelI == 6)
                            break;
                        
                        goto skipChannel;
                    }

                    glm::vec2 res = colorBuffer.getResolution();

                    bool prevAlbedoHistoryUsed = albedoHistoryUsed, prevRoughnessHistoryUsed = roughnessHistoryUsed, prevMetallicHistoryUsed = metallicHistoryUsed, prevNormalMapHistoryUsed = normalMapHistoryUsed, prevHeightMapHistoryUsed = heightMapHistoryUsed, prevAoHistoryUsed = aoHistoryUsed;

                    {
                        if(meshTxtrHistory.size() < getScene()->model->meshes.size()){
                            while (meshTxtrHistory.size() < getScene()->model->meshes.size())
                            {
                                Mesh msh;
                                msh.albedo = Texture((char*)nullptr, 1, 1);
                                msh.roughness = Texture((char*)nullptr, 1, 1);
                                msh.metallic = Texture((char*)nullptr, 1, 1);
                                msh.normalMap = Texture((char*)nullptr, 1, 1);
                                msh.heightMap = Texture((char*)nullptr, 1, 1);
                                msh.ambientOcclusion = Texture((char*)nullptr, 1, 1);
                                
                                meshTxtrHistory.push_back(msh);
                            }
                        }
                        
                        else if(meshTxtrHistory.size() > getScene()->model->meshes.size()){
                            while (meshTxtrHistory.size() > getScene()->model->meshes.size()){
                                glDeleteTextures(1, &meshTxtrHistory[meshTxtrHistory.size() - 1].albedo.ID);
                                glDeleteTextures(1, &meshTxtrHistory[meshTxtrHistory.size() - 1].roughness.ID);
                                glDeleteTextures(1, &meshTxtrHistory[meshTxtrHistory.size() - 1].metallic.ID);
                                glDeleteTextures(1, &meshTxtrHistory[meshTxtrHistory.size() - 1].normalMap.ID);
                                glDeleteTextures(1, &meshTxtrHistory[meshTxtrHistory.size() - 1].heightMap.ID);
                                glDeleteTextures(1, &meshTxtrHistory[meshTxtrHistory.size() - 1].ambientOcclusion.ID);
                                meshTxtrHistory.pop_back();
                            }
                        }
                    }

                    if(channelI == 0){
                        albedoHistoryUsed = albedoChannelCheckBox.clickState1;
                        if(albedoHistoryUsed){
                            glm::ivec2 hTxtrRes = meshTxtrHistory[i].albedo.getResolution();
                            glm::ivec2 oTxtrRes = colorBuffer.getResolution();
                            
                            meshTxtrHistory[i].albedo.update((char*)nullptr, oTxtrRes.x, oTxtrRes.y);
                            colorBuffer.duplicateTextureSub(meshTxtrHistory[i].albedo);
                        }
                    }
                    if(channelI == 1){
                        roughnessHistoryUsed = roughnessChannelCheckBox.clickState1;
                        if(roughnessHistoryUsed){
                            glm::ivec2 hTxtrRes = meshTxtrHistory[i].roughness.getResolution();
                            glm::ivec2 oTxtrRes = colorBuffer.getResolution();
                            
                            meshTxtrHistory[i].roughness.update((char*)nullptr, oTxtrRes.x, oTxtrRes.y);
                            colorBuffer.duplicateTextureSub(meshTxtrHistory[i].roughness);
                        }
                    }
                    if(channelI == 2){
                        metallicHistoryUsed = metallicChannelCheckBox.clickState1;
                        if(metallicHistoryUsed){
                            glm::ivec2 hTxtrRes = meshTxtrHistory[i].metallic.getResolution();
                            glm::ivec2 oTxtrRes = colorBuffer.getResolution();
                            
                            meshTxtrHistory[i].metallic.update((char*)nullptr, oTxtrRes.x, oTxtrRes.y);
                            colorBuffer.duplicateTextureSub(meshTxtrHistory[i].metallic);
                        }
                    }
                    if(channelI == 3){
                        normalMapHistoryUsed = normalMapChannelCheckBox.clickState1;
                        if(normalMapHistoryUsed){
                            glm::ivec2 hTxtrRes = meshTxtrHistory[i].normalMap.getResolution();
                            glm::ivec2 oTxtrRes = colorBuffer.getResolution();
                            
                            meshTxtrHistory[i].normalMap.update((char*)nullptr, oTxtrRes.x, oTxtrRes.y);
                            colorBuffer.duplicateTextureSub(meshTxtrHistory[i].normalMap);
                        }
                    }
                    if(channelI == 4){
                        heightMapHistoryUsed = heightMapChannelCheckBox.clickState1;
                        if(heightMapHistoryUsed){
                            glm::ivec2 hTxtrRes = meshTxtrHistory[i].heightMap.getResolution();
                            glm::ivec2 oTxtrRes = colorBuffer.getResolution();
                            
                            meshTxtrHistory[i].heightMap.update((char*)nullptr, oTxtrRes.x, oTxtrRes.y);
                            colorBuffer.duplicateTextureSub(meshTxtrHistory[i].heightMap);
                        }
                    }
                    if(channelI == 5){
                        aoHistoryUsed = aoChannelCheckBox.clickState1;
                        if(aoHistoryUsed){
                            glm::ivec2 hTxtrRes = meshTxtrHistory[i].ambientOcclusion.getResolution();
                            glm::ivec2 oTxtrRes = colorBuffer.getResolution();
                            
                            meshTxtrHistory[i].ambientOcclusion.update((char*)nullptr, oTxtrRes.x, oTxtrRes.y);
                            colorBuffer.duplicateTextureSub(meshTxtrHistory[i].ambientOcclusion);
                        }
                    }

                    if(!albedoHistoryUsed && !roughnessHistoryUsed && !metallicHistoryUsed && !normalMapHistoryUsed && !heightMapHistoryUsed && !aoHistoryUsed){
                        albedoHistoryUsed = prevAlbedoHistoryUsed;
                        roughnessHistoryUsed = prevRoughnessHistoryUsed;
                        metallicHistoryUsed = prevMetallicHistoryUsed;
                        normalMapHistoryUsed = prevNormalMapHistoryUsed;
                        heightMapHistoryUsed = prevHeightMapHistoryUsed;
                        aoHistoryUsed = prevAoHistoryUsed;
                    }

                    Framebuffer FBO = FBOPOOL::requestFBO(colorBuffer, "Object texturing dialog assigning");

                    ShaderSystem::objectTexturingAssign().use();
                    ShaderSystem::objectTexturingAssign().setMat4("orthoProjection", glm::ortho(0.f,1.f,0.f,1.f));
                    ShaderSystem::objectTexturingAssign().setMat4("perspectiveProjection", getScene()->projectionMatrix);
                    ShaderSystem::objectTexturingAssign().setMat4("view", getScene()->camera.viewMatrix);
                    ShaderSystem::objectTexturingAssign().setInt("txtr", 0);

                    ShaderUTIL::set_shader_struct_face_selection_data(ShaderSystem::objectTexturingAssign(), getScene()->model->meshes[i]);

                    getScene()->model->meshes[i].Draw();

                    GL::releaseBoundTextures("ObjectTexturingDialog : Updating displaying texture");
                    
                    glEnable(GL_DEPTH_TEST);

                    FBOPOOL::releaseFBO(FBO);
                }
            }    
        }
        else if(undoButton.clicked){
            for (size_t i = 0; i < getScene()->model->meshes.size(); i++)
            {
                if(i < meshTxtrHistory.size()){
                    if(albedoHistoryUsed)
                        meshTxtrHistory[i].albedo.duplicateTexture(getScene()->model->meshes[i].albedo);
                    if(roughnessHistoryUsed)
                        meshTxtrHistory[i].roughness.duplicateTexture(getScene()->model->meshes[i].roughness);
                    if(metallicHistoryUsed)
                        meshTxtrHistory[i].metallic.duplicateTexture(getScene()->model->meshes[i].metallic);
                    if(normalMapHistoryUsed)
                        meshTxtrHistory[i].normalMap.duplicateTexture(getScene()->model->meshes[i].normalMap);
                    if(heightMapHistoryUsed)
                        meshTxtrHistory[i].heightMap.duplicateTexture(getScene()->model->meshes[i].heightMap);
                    if(aoHistoryUsed)
                        meshTxtrHistory[i].ambientOcclusion.duplicateTexture(getScene()->model->meshes[i].ambientOcclusion);
                }
            }
            
            albedoHistoryUsed = false;
            roughnessHistoryUsed = false;
            metallicHistoryUsed = false;
            normalMapHistoryUsed = false;
            heightMapHistoryUsed = false;
            aoHistoryUsed = false;
        }
        
        if(!this->editMaterialButton.clicked && __materialEditBtn || assignRelatedTexturesButton.clicked){
            this->updateMeshTextures();
            __materialEditBtn = false;
        }
        if(!this->selectMaterialButton.clicked && __materialSelectBtn){
            this->updateMeshTextures();
            __materialSelectBtn = false;
        }

        if(!this->panel.hover && *Mouse::LClick() && !dialog_log.isHovered() || (getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) && textRenderer.keyInput) && !this->materialSelection && !this->textureSelection){
            this->dialogControl.unActivate();
        }
        
        if(this->materialSelection || this->textureSelection){
            
            if(this->selectMaterialButton.clicked){
                this->elementSelectionPanel.sections[0].elements.clear();
                
                if(this->materialSelection){
                    for (size_t i = 0; i < Library::getMaterialArraySize(); i++)
                    {
                        this->elementSelectionPanel.sections[0].elements.push_back(Button(ELEMENT_STYLE_BASIC, glm::vec2(6, 2.f), Library::getMaterialObj(i).title, Library::getMaterialObj(i).displayingTexture, 0.f, false));
                    }
                    
                    this->elementSelectionPanel.sections[0].elements.push_back(Button(ELEMENT_STYLE_STYLIZED, glm::vec2(6, 2.f), "Default", Texture(), 0.f, false));
                }
                
                if(this->textureSelection){
                    for (size_t i = 0; i < Library::getTextureArraySize(); i++)
                    {
                        this->elementSelectionPanel.sections[0].elements.push_back(Button(ELEMENT_STYLE_BASIC, glm::vec2(6, 2.f), Library::getTextureObj(i).title, Library::getTextureObj(i), 0.f, false));
                    }
                }
            }
            
            this->elementSelectionPanel.render(timer, true);

            if(!this->elementSelectionPanel.hover && *Mouse::LClick() || getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE)){
                this->materialSelection = false; 
                this->textureSelection = false;
            }
        }

        dialogControl.updateEnd(timer,0.15f);
        if(dialogControl.mixVal == 0.f)
            break;
    }
}



void ObjectTexturingDialog::updateDisplayingTexture(){
    Shader already_bound_shader = ShaderUTIL::get_bound_shader();

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
    glViewport(0,0, DISPLAY_RESOLUTION.x, DISPLAY_RESOLUTION.y);

    //Clear the capture framebuffer (displaying texture) with alpha zero color
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    /*
    if(channelI == 0)
        GL::bindTexture_2D(result_channels[i].albedo.ID, 0, "ObjectTexturingDialog::show : Updating displaying texture");
    if(channelI == 1)
        GL::bindTexture_2D(result_channels[i].roughness.ID, 0, "ObjectTexturingDialog::show : Updating displaying texture");
    if(channelI == 2)
        GL::bindTexture_2D(result_channels[i].metallic.ID, 0, "ObjectTexturingDialog::show : Updating displaying texture");
    if(channelI == 3)
        GL::bindTexture_2D(result_channels[i].normalMap.ID, 0, "ObjectTexturingDialog::show : Updating displaying texture");
    if(channelI == 4)
        GL::bindTexture_2D(result_channels[i].heightMap.ID, 0, "ObjectTexturingDialog::show : Updating displaying texture");
    if(channelI == 5)
        GL::bindTexture_2D(result_channels[i].ambientOcclusion.ID, 0, "ObjectTexturingDialog::show : Updating displaying texture");
    */

    getScene()->render_scene(Timer()); //result_channels

    // 190 - 1727
    // 114 - 967
    
    //!Finish (prepare rendering the GUI)
    already_bound_shader.use();
    FBOPOOL::releaseFBO(FBO);

}

#define MIX_CHANNELS(mask, normalMapMode, invert)   getScene()->model->meshes[meshI].albedo.mix(albedo, mask, false, normalMapMode, invert);\
                                                    getScene()->model->meshes[meshI].roughness.mix(roughness, mask, false, normalMapMode, invert);\
                                                    getScene()->model->meshes[meshI].metallic.mix(metallic, mask, false, normalMapMode, invert);\
                                                    getScene()->model->meshes[meshI].normalMap.mix(normalMap, mask, false, normalMapMode, invert);\
                                                    getScene()->model->meshes[meshI].heightMap.mix(heightMap, mask, false, normalMapMode, invert);\
                                                    getScene()->model->meshes[meshI].ambientOcclusion.mix(ambientOcclusion, mask, false, normalMapMode, invert)

static Texture maskDuplicated;

void ObjectTexturingDialog::updateMeshTextures(){
    for (size_t meshI = 0; meshI < getScene()->model->meshes.size(); meshI++)
    {
        Texture albedo = getScene()->model->meshes[meshI].albedo; 
        Texture roughness = getScene()->model->meshes[meshI].roughness; 
        Texture metallic = getScene()->model->meshes[meshI].metallic; 
        Texture normalMap = getScene()->model->meshes[meshI].normalMap; 
        Texture heightMap = getScene()->model->meshes[meshI].heightMap; 
        Texture ambientOcclusion = getScene()->model->meshes[meshI].ambientOcclusion; 
        
        getScene()->model->meshes[meshI].albedo = result_channels[meshI].albedo;
        getScene()->model->meshes[meshI].roughness = result_channels[meshI].roughness;
        getScene()->model->meshes[meshI].metallic = result_channels[meshI].metallic;
        getScene()->model->meshes[meshI].normalMap = result_channels[meshI].normalMap;
        getScene()->model->meshes[meshI].heightMap = result_channels[meshI].heightMap;
        getScene()->model->meshes[meshI].ambientOcclusion = result_channels[meshI].ambientOcclusion;
        
        //this->material.apply_material(*getScene()->model, getScene()->model->meshes[meshI], getResolution(), false);

        Texture maskMat = maskMaterialBtn.texture.generateProceduralTexture(getScene()->model->meshes[meshI], this->getResolution());

        // Mask channels
        MIX_CHANNELS(maskMat, false, false);
        
        Texture maskTxtr;

        // Mask channels via channel textures
        if(mixOptionsComboBox.selectedIndex == 3){ // Roughness
            maskTxtr = getScene()->model->meshes[meshI].roughness; 
        }
        if(mixOptionsComboBox.selectedIndex == 4){ // Metallic
            maskTxtr = getScene()->model->meshes[meshI].metallic; 
        }
        if(mixOptionsComboBox.selectedIndex == 5){ // AO
            maskTxtr = getScene()->model->meshes[meshI].ambientOcclusion; 
        }
        if(mixOptionsComboBox.selectedIndex == 6){ // Height Map
            maskTxtr = getScene()->model->meshes[meshI].heightMap; 
        }
        if(mixOptionsComboBox.selectedIndex == 7 || mixOptionsComboBox.selectedIndex == 8){ // Normal Map
            maskTxtr = getScene()->model->meshes[meshI].normalMap; 
        }
        
        if(mixOptionsComboBox.selectedIndex){

            if(mixOptionsComboBox.selectedIndex == 1 || mixOptionsComboBox.selectedIndex == 2){
                std::vector<Texture> orgTxtrs = {albedo, roughness, metallic, normalMap, heightMap, ambientOcclusion};
                std::vector<Texture> modifiedTxtrs = {getScene()->model->meshes[meshI].albedo, getScene()->model->meshes[meshI].roughness, getScene()->model->meshes[meshI].metallic, getScene()->model->meshes[meshI].normalMap, getScene()->model->meshes[meshI].heightMap, getScene()->model->meshes[meshI].ambientOcclusion};
            
                for (size_t i = 0; i < orgTxtrs.size(); i++)
                {
                    maskTxtr = modifiedTxtrs[i];
                    glm::ivec2 maskRes = maskTxtr.getResolution();
                    if(maskDuplicated.ID == 0){
                        maskDuplicated = Texture((char*)nullptr, maskRes.x, maskRes.y);
                    }
                    else{
                        maskDuplicated.update((char*)nullptr, maskRes.x, maskRes.y);
                    }
                    
                    maskTxtr.duplicateTextureSub(maskDuplicated);
                
                    modifiedTxtrs[i].mix(orgTxtrs[i], maskDuplicated, false, i == 3, mixOptionsComboBox.selectedIndex == 2);
                }
                
            }
            else{
                glm::ivec2 maskRes = maskTxtr.getResolution();
                if(maskDuplicated.ID == 0){
                    maskDuplicated = Texture((char*)nullptr, maskRes.x, maskRes.y);
                }
                else{
                    maskDuplicated.update((char*)nullptr, maskRes.x, maskRes.y);
                }
                
                maskTxtr.duplicateTextureSub(maskDuplicated);

                MIX_CHANNELS(maskDuplicated, mixOptionsComboBox.selectedIndex == 7 || mixOptionsComboBox.selectedIndex == 8, mixOptionsComboBox.selectedIndex != 8);
            }
        }

        if(!albedoChannelCheckBox.clickState1)
            getScene()->model->meshes[meshI].albedo.mix(albedo, appTextures.white, false, false, false);
        if(!roughnessChannelCheckBox.clickState1)
            getScene()->model->meshes[meshI].roughness.mix(roughness, appTextures.white, false, false, false);
        if(!metallicChannelCheckBox.clickState1)
            getScene()->model->meshes[meshI].metallic.mix(metallic, appTextures.white, false, false, false);
        if(!normalMapChannelCheckBox.clickState1)
            getScene()->model->meshes[meshI].normalMap.mix(normalMap, appTextures.white, false, false, false);
        if(!heightMapChannelCheckBox.clickState1)
            getScene()->model->meshes[meshI].heightMap.mix(heightMap, appTextures.white, false, false, false);
        if(!aoChannelCheckBox.clickState1)
            getScene()->model->meshes[meshI].ambientOcclusion.mix(ambientOcclusion, appTextures.white, false, false, false);

        getScene()->model->meshes[meshI].albedo = albedo; 
        getScene()->model->meshes[meshI].roughness = roughness; 
        getScene()->model->meshes[meshI].metallic = metallic; 
        getScene()->model->meshes[meshI].normalMap = normalMap; 
        getScene()->model->meshes[meshI].heightMap = heightMap; 
        getScene()->model->meshes[meshI].ambientOcclusion = ambientOcclusion; 
    }
}

bool ObjectTexturingDialog::anyElementHover(){
    return assignRelatedTexturesButton.hover || materialDisplayerButton.hover || editMaterialButton.hover || selectMaterialButton.hover;
}

int ObjectTexturingDialog::getResolution(){
    for (size_t i = 0; i < getScene()->model->meshes.size(); i++)
    {
        if(getScene()->model->meshes[i].albedo.ID)
            return getScene()->model->meshes[i].albedo.getResolution().x;

        else if(getScene()->model->meshes[i].roughness.ID)
            return getScene()->model->meshes[i].roughness.getResolution().x;

        else if(getScene()->model->meshes[i].metallic.ID)
            return getScene()->model->meshes[i].metallic.getResolution().x;

        else if(getScene()->model->meshes[i].normalMap.ID)
            return getScene()->model->meshes[i].normalMap.getResolution().x;

        else if(getScene()->model->meshes[i].heightMap.ID)
            return getScene()->model->meshes[i].heightMap.getResolution().x;

        else if(getScene()->model->meshes[i].ambientOcclusion.ID)
            return getScene()->model->meshes[i].ambientOcclusion.getResolution().x;
    
        else
            return 1024;
    }

    Settings::defaultFramebuffer()->FBO.bind();
    
}