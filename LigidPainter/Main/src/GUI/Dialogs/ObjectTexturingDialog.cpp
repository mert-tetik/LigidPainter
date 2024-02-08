
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
#include "LibrarySystem/Library.hpp"
#include "MouseSystem/Mouse.hpp" 
#include "ColorPaletteSystem/ColorPalette.hpp"

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

ObjectTexturingDialog::ObjectTexturingDialog(){
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

    this->maskViaFaceSelection = Button(ELEMENT_STYLE_BASIC, glm::vec2(6, 2.f), "Mask via face selection", Texture(), 1.f, false);
    this->maskViaTexture = Button(ELEMENT_STYLE_BASIC, glm::vec2(6, 2.f), "Mask via texture", Texture(), 1.f, false);
    this->cancelMasks = Button(ELEMENT_STYLE_BASIC, glm::vec2(6, 2.f), "Cancel masks", Texture(), 1.f, false);
    
    this->maskMaterialBtn = Button(ELEMENT_STYLE_BASIC, glm::vec2(6, 2.f), "Material mask", Texture(), 1.f, false);
    this->maskMaterialBtn.textureSelection3D = true;

    this->ctrlInfoBtn = Button(ELEMENT_STYLE_BASIC, glm::vec2(7, 4.f), "CTRL - Unselect the selected faces", Texture(), 1.f, false);
    this->shiftInfoBtn = Button(ELEMENT_STYLE_BASIC, glm::vec2(7, 4.f), "SHIFT - Add to selected faces", Texture(), 1.f, false);
    this->ctrlInfoBtn.color = glm::vec4(0.);
    this->ctrlInfoBtn.color2 = glm::vec4(0.);
    this->ctrlInfoBtn.textScale = 0.6f;
    this->shiftInfoBtn.color = glm::vec4(0.);
    this->shiftInfoBtn.color2 = glm::vec4(0.);
    this->shiftInfoBtn.textScale = 0.6f;
    
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

    this->displayingTexture = Texture(nullptr, DISPLAY_RESOLUTION.x, DISPLAY_RESOLUTION.y);

    this->displayingFBO = Framebuffer(this->displayingTexture, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, DISPLAY_RESOLUTION), "Object texturing dialog - displaying");

    this->sceneCam.setCameraPosition(glm::vec3(0,0,-3.5f));
    this->sceneCam.radius = 3.5f;

    this->material = Material("ObjectTexturingMaterial", 0);
    this->material.materialModifiers.push_back(MaterialModifier(SOLID_MATERIAL_MODIFIER));
    char whitePixel[] = { 127, 127, 127, 127 }; // 1 pixel, RGBA format (white)
    material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
    material.materialModifiers[0].maskTexture.proceduralProps.proceduralID = 24;
    this->material.updateMaterialDisplayingTexture(256, true, Camera(), 0, false);
    this->material.displayingTexture.title = "ObjectTexturingMaterial_DisplayingTexture";
}

static bool __materialEditBtn = false;
static bool __materialSelectBtn = false;

void ObjectTexturingDialog::render(Timer timer, glm::mat4 projection, MaterialEditorDialog& materialEditorDialog, LogDialog& logDialog, MaterialSelectionDialog& materialSelectionDialog){
    dialogControl.updateStart();


    
    if(getModel()->meshes.size() != texturesMesh.size()){
        for (size_t i = 0; i < texturesMesh.size(); i++)
        {
            glDeleteTextures(1, &texturesMesh[i].albedo.ID);
            glDeleteTextures(1, &texturesMesh[i].roughness.ID);
            glDeleteTextures(1, &texturesMesh[i].metallic.ID);
            glDeleteTextures(1, &texturesMesh[i].normalMap.ID);
            glDeleteTextures(1, &texturesMesh[i].heightMap.ID);
            glDeleteTextures(1, &texturesMesh[i].ambientOcclusion.ID);
        }

        texturesMesh.clear();

        for (size_t i = 0; i < getModel()->meshes.size(); i++)
        {
            Mesh mesh;
            mesh.albedo = Texture(nullptr, 1024, 1024);
            mesh.roughness = Texture(nullptr, 1024, 1024);
            mesh.metallic = Texture(nullptr, 1024, 1024);
            mesh.normalMap = Texture(nullptr, 1024, 1024);
            mesh.heightMap = Texture(nullptr, 1024, 1024);
            mesh.ambientOcclusion = Texture(nullptr, 1024, 1024);
        
            texturesMesh.push_back(mesh);
        }
    }

    if(dialogControl.firstFrameActivated){
        this->material.updateMaterialDisplayingTexture(256, true, Camera(), 0, false);
        
        this->sceneCam.setCameraPosition(glm::vec3(0,0,-3.5f));
        this->sceneCam.radius = 3.5f;
        this->updateMeshTextures();
    }


    if(dialogControl.firstFrameActivated || this->cancelMasks.clicked || (getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) && textRenderer.keyInput && faceSelectionMode)){
        glDeleteTextures(1, &this->meshMask.ID);
        this->meshMask.ID = 0;
        this->meshMask.title = "";

        for (size_t i = 0; i < faceSelection.size(); i++)
        {
            glDeleteTextures(1, &faceSelection[i].selectedFaces.ID);
            glDeleteTextures(1, &faceSelection[i].modelPrimitives.ID);
            glDeleteTextures(1, &faceSelection[i].meshMask.ID);
        }
        
        faceSelection.clear();

        for (size_t meshI = 0; meshI < getModel()->meshes.size(); meshI++)
        {
            faceSelection.push_back(FaceSelection());
            faceSelection[faceSelection.size()-1].selectedFaces = getModel()->meshes[meshI].selectedObjectPrimitivesTxtr.duplicateTexture();
            faceSelection[faceSelection.size()-1].meshMask = appTextures.white.duplicateTexture();
        
            faceSelection[faceSelection.size()-1].selectedPrimitiveIDs.resize(getModel()->meshes[meshI].indices.size() / 3);
            for (size_t oI = 0; oI < getModel()->meshes[meshI].objects.size(); oI++)
            {
                bool contains = false;
                    for (size_t i = 0; i < getModel()->meshes[meshI].selectedObjectIndices.size(); i++)
                    {
                        if(getModel()->meshes[meshI].selectedObjectIndices[i] == oI)
                            contains = true;
                    }
                
                if(contains){
                    for (size_t i = getModel()->meshes[meshI].objects[oI].vertIndices.x / 3; i < getModel()->meshes[meshI].objects[oI].vertIndices.y / 3; i++)
                    {
                        if(i < faceSelection[faceSelection.size()-1].selectedPrimitiveIDs.size()){
                            faceSelection[faceSelection.size()-1].changedIndices.push_back(i);
                            faceSelection[faceSelection.size()-1].selectedPrimitiveIDs[i] = 2;
                        }
                    }
                }
            }
        }

        this->updateMeshTextures();
    }

    // Update the scene texture
    updateDisplayingTexture(logDialog);

    // Modifying the elements    
    this->panel.sections[0].elements[0].button.scale = this->panel.scale;

    this->materialDisplayerButton.texture = this->material.displayingTexture;
    this->maskViaTexture.texture = this->meshMask;

    this->maskViaFaceSelection.pos = this->panel.pos;
    this->maskViaFaceSelection.pos.x -= this->panel.scale.x - this->maskViaFaceSelection.scale.x - 2.f;
    this->maskViaFaceSelection.pos.y += 15.f;
    this->maskViaTexture.pos = this->maskViaFaceSelection.pos;
    this->maskViaTexture.pos.y += this->maskViaTexture.scale.y + this->maskViaFaceSelection.scale.y;
    this->cancelMasks.pos = this->maskViaTexture.pos;
    this->cancelMasks.pos.y += this->cancelMasks.scale.y + this->maskViaTexture.scale.y;
    
    this->maskMaterialBtn.pos = cancelMasks.pos;
    this->maskMaterialBtn.pos.y += this->maskMaterialBtn.scale.y + cancelMasks.scale.y * 2.f;

    this->ctrlInfoBtn.pos = this->panel.pos;
    this->ctrlInfoBtn.pos.y += this->panel.scale.y - this->ctrlInfoBtn.scale.y;
    this->ctrlInfoBtn.pos.x -= this->ctrlInfoBtn.scale.x + 2;
    this->shiftInfoBtn.pos = this->panel.pos;
    this->shiftInfoBtn.pos.y += this->panel.scale.y - this->ctrlInfoBtn.scale.y;
    this->shiftInfoBtn.pos.x += this->shiftInfoBtn.scale.x + 2;

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

    if(faceSelectionMode)
        maskViaFaceSelection.text = "Cancel face selection";
    else
        maskViaFaceSelection.text = "Mask via face selection";

    //Render the panel
    this->panel.render(timer, false);

    ShaderSystem::textureRenderingShader().use();
    ShaderSystem::textureRenderingShader().setMat4("projection", projection);
    ShaderSystem::textureRenderingShader().setVec2("scale", this->panel.resultScale);
    ShaderSystem::textureRenderingShader().setVec3("pos", this->panel.resultPos);
    ShaderSystem::textureRenderingShader().setInt("txtr", 0);
    ShaderSystem::textureRenderingShader().setFloat("opacity", this->dialogControl.mixVal);
    ShaderSystem::textureRenderingShader().setFloat("rotation", 0.f);
    ShaderSystem::textureRenderingShader().setInt("depthToleranceMode", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->displayingTexture.ID);

    LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Object texturing dialog : Rendering displaying texture");

    ShaderSystem::buttonShader().use();

    // Render the elements
    this->maskViaFaceSelection.render(timer, !this->materialSelection && !this->textureSelection);
    this->maskViaTexture.render(timer, !this->materialSelection && !this->textureSelection);
    this->cancelMasks.render(timer, !this->materialSelection && !this->textureSelection);
    this->maskMaterialBtn.render(timer, !this->materialSelection && !this->textureSelection);
    this->assignRelatedTexturesButton.render(timer, !this->materialSelection && !this->textureSelection);
    if(albedoHistoryUsed || roughnessHistoryUsed || metallicHistoryUsed || normalMapHistoryUsed || heightMapHistoryUsed || aoHistoryUsed)
        this->undoButton.render(timer, !this->materialSelection && !this->textureSelection);
    this->materialDisplayerButton.render(timer, false);
    this->editMaterialButton.render(timer, !this->materialSelection && !this->textureSelection);
    this->selectMaterialButton.render(timer, !this->materialSelection && !this->textureSelection);
    this->defaultMaterialButton.render(timer, !this->materialSelection && !this->textureSelection);
    if(this->faceSelectionMode){
        this->ctrlInfoBtn.render(timer, false);
        this->shiftInfoBtn.render(timer, false);
    }
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
        materialSelectionDialog.dialogControl.activate();
        materialSelectionDialog.material = &this->material;
        __materialSelectBtn = true;

        this->faceSelectionMode = false;
    }
    else if(this->defaultMaterialButton.clicked){
        this->material.deleteBuffers();
        this->material = Material("ObjectTexturingMaterial", 0);
        this->material.materialModifiers.push_back(MaterialModifier(SOLID_MATERIAL_MODIFIER));
        char whitePixel[] = { 127, 127, 127, 127 }; // 1 pixel, RGBA format (white)
        material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
        material.materialModifiers[0].maskTexture.proceduralProps.proceduralID = 24;
        this->material.updateMaterialDisplayingTexture(256, true, Camera(), 0, false);
        this->material.displayingTexture.title = "ObjectTexturingMaterial_DisplayingTexture";
        this->updateMeshTextures();

        this->faceSelectionMode = false;
    }
    else if(this->editMaterialButton.clicked){
        materialEditorDialog.material = &this->material;
        materialEditorDialog.activate();
        __materialEditBtn = true;

        this->faceSelectionMode = false;
    }
    else if(assignRelatedTexturesButton.clicked){
        for (size_t i = 0; i < getModel()->meshes.size(); i++)
        {
            for (size_t channelI = 0; channelI < 6; channelI++)
            {
                skipChannel:
                
                glDisable(GL_DEPTH_TEST);
                Texture colorBuffer;
                std::string colorBufferChannelTitle = ""; 
                if(channelI == 0){
                    colorBuffer = getModel()->meshes[i].albedo.ID;
                    colorBufferChannelTitle = "albedo";
                }
                if(channelI == 1){
                    colorBuffer = getModel()->meshes[i].roughness.ID;
                    colorBufferChannelTitle = "roughness";
                }
                if(channelI == 2){
                    colorBuffer = getModel()->meshes[i].metallic.ID;
                    colorBufferChannelTitle = "metallic";
                }
                if(channelI == 3){
                    colorBuffer = getModel()->meshes[i].normalMap.ID;
                    colorBufferChannelTitle = "normalMap";
                }
                if(channelI == 4){
                    colorBuffer = getModel()->meshes[i].heightMap.ID;
                    colorBufferChannelTitle = "heightMap";
                }
                if(channelI == 5){
                    colorBuffer = getModel()->meshes[i].ambientOcclusion.ID;
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
                    if(meshTxtrHistory.size() < getModel()->meshes.size()){
                        while (meshTxtrHistory.size() < getModel()->meshes.size())
                        {
                            Mesh msh;
                            msh.albedo = Texture(nullptr, 1, 1);
                            msh.roughness = Texture(nullptr, 1, 1);
                            msh.metallic = Texture(nullptr, 1, 1);
                            msh.normalMap = Texture(nullptr, 1, 1);
                            msh.heightMap = Texture(nullptr, 1, 1);
                            msh.ambientOcclusion = Texture(nullptr, 1, 1);
                            
                            meshTxtrHistory.push_back(msh);
                        }
                    }
                    
                    else if(meshTxtrHistory.size() > getModel()->meshes.size()){
                        while (meshTxtrHistory.size() > getModel()->meshes.size()){
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
                        
                        meshTxtrHistory[i].albedo.update(nullptr, oTxtrRes.x, oTxtrRes.y);
                        colorBuffer.duplicateTextureSub(meshTxtrHistory[i].albedo);
                    }
                }
                if(channelI == 1){
                    roughnessHistoryUsed = roughnessChannelCheckBox.clickState1;
                    if(roughnessHistoryUsed){
                        glm::ivec2 hTxtrRes = meshTxtrHistory[i].roughness.getResolution();
                        glm::ivec2 oTxtrRes = colorBuffer.getResolution();
                        
                        meshTxtrHistory[i].roughness.update(nullptr, oTxtrRes.x, oTxtrRes.y);
                        colorBuffer.duplicateTextureSub(meshTxtrHistory[i].roughness);
                    }
                }
                if(channelI == 2){
                    metallicHistoryUsed = metallicChannelCheckBox.clickState1;
                    if(metallicHistoryUsed){
                        glm::ivec2 hTxtrRes = meshTxtrHistory[i].metallic.getResolution();
                        glm::ivec2 oTxtrRes = colorBuffer.getResolution();
                        
                        meshTxtrHistory[i].metallic.update(nullptr, oTxtrRes.x, oTxtrRes.y);
                        colorBuffer.duplicateTextureSub(meshTxtrHistory[i].metallic);
                    }
                }
                if(channelI == 3){
                    normalMapHistoryUsed = normalMapChannelCheckBox.clickState1;
                    if(normalMapHistoryUsed){
                        glm::ivec2 hTxtrRes = meshTxtrHistory[i].normalMap.getResolution();
                        glm::ivec2 oTxtrRes = colorBuffer.getResolution();
                        
                        meshTxtrHistory[i].normalMap.update(nullptr, oTxtrRes.x, oTxtrRes.y);
                        colorBuffer.duplicateTextureSub(meshTxtrHistory[i].normalMap);
                    }
                }
                if(channelI == 4){
                    heightMapHistoryUsed = heightMapChannelCheckBox.clickState1;
                    if(heightMapHistoryUsed){
                        glm::ivec2 hTxtrRes = meshTxtrHistory[i].heightMap.getResolution();
                        glm::ivec2 oTxtrRes = colorBuffer.getResolution();
                        
                        meshTxtrHistory[i].heightMap.update(nullptr, oTxtrRes.x, oTxtrRes.y);
                        colorBuffer.duplicateTextureSub(meshTxtrHistory[i].heightMap);
                    }
                }
                if(channelI == 5){
                    aoHistoryUsed = aoChannelCheckBox.clickState1;
                    if(aoHistoryUsed){
                        glm::ivec2 hTxtrRes = meshTxtrHistory[i].ambientOcclusion.getResolution();
                        glm::ivec2 oTxtrRes = colorBuffer.getResolution();
                        
                        meshTxtrHistory[i].ambientOcclusion.update(nullptr, oTxtrRes.x, oTxtrRes.y);
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

                Framebuffer FBO = Framebuffer(colorBuffer, GL_TEXTURE_2D, "Object texturing dialog assigning");
                FBO.bind();

                glViewport(0,0,res.x,res.y);

                ShaderSystem::objectTexturingAssign().use();
                ShaderSystem::objectTexturingAssign().setMat4("orthoProjection", glm::ortho(0.f,1.f,0.f,1.f));
                ShaderSystem::objectTexturingAssign().setMat4("perspectiveProjection", getScene()->projectionMatrix);
                ShaderSystem::objectTexturingAssign().setMat4("view", getScene()->viewMatrix);
                ShaderSystem::objectTexturingAssign().setInt("txtr", 0);
                ShaderSystem::objectTexturingAssign().setInt("selectedPrimitiveIDS", 1);
                ShaderSystem::objectTexturingAssign().setInt("meshMask", 2);
                ShaderSystem::objectTexturingAssign().setInt("primitiveCount", getModel()->meshes[i].indices.size() / 3);

                glActiveTexture(GL_TEXTURE0);
                if(channelI == 0)
                    glBindTexture(GL_TEXTURE_2D, texturesMesh[i].albedo.ID);
                if(channelI == 1)
                    glBindTexture(GL_TEXTURE_2D, texturesMesh[i].roughness.ID);
                if(channelI == 2)
                    glBindTexture(GL_TEXTURE_2D, texturesMesh[i].metallic.ID);
                if(channelI == 3)
                    glBindTexture(GL_TEXTURE_2D, texturesMesh[i].normalMap.ID);
                if(channelI == 4)
                    glBindTexture(GL_TEXTURE_2D, texturesMesh[i].heightMap.ID);
                if(channelI == 5)
                    glBindTexture(GL_TEXTURE_2D, texturesMesh[i].ambientOcclusion.ID);
                
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, faceSelection[i].selectedFaces.ID);
                
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, faceSelection[i].meshMask.ID);

                getModel()->meshes[i].Draw(false);
                
                glEnable(GL_DEPTH_TEST);

                FBO.deleteBuffers(false, false);
                Settings::defaultFramebuffer()->FBO.bind();
                Settings::defaultFramebuffer()->setViewport();
            }
        }    
    }
    else if(maskViaFaceSelection.clicked){
        faceSelectionMode = !faceSelectionMode;
    }
    else if(maskViaTexture.clicked){
        
        showTextureSelectionDialog(this->meshMask, this->getResolution(), false);
        
        if(this->meshMask.ID){
            for (size_t i = 0; i < getModel()->meshes.size(); i++)
            {
                if(i < this->faceSelection.size()){
                    if(!this->faceSelection[i].meshMask.ID)
                        this->faceSelection[i].meshMask = Texture(nullptr, this->getResolution(), this->getResolution());
                    this->faceSelection[i].meshMask.update(nullptr, this->getResolution(), this->getResolution());
                    this->meshMask.generateProceduralTexture(getModel()->meshes[i], this->faceSelection[i].meshMask, this->getResolution());
                }
            }
        }
    
        this->faceSelectionMode = false;
    }
    else if(undoButton.clicked){
        for (size_t i = 0; i < getModel()->meshes.size(); i++)
        {
            if(i < meshTxtrHistory.size()){
                if(albedoHistoryUsed)
                    meshTxtrHistory[i].albedo.duplicateTexture(getModel()->meshes[i].albedo);
                if(roughnessHistoryUsed)
                    meshTxtrHistory[i].roughness.duplicateTexture(getModel()->meshes[i].roughness);
                if(metallicHistoryUsed)
                    meshTxtrHistory[i].metallic.duplicateTexture(getModel()->meshes[i].metallic);
                if(normalMapHistoryUsed)
                    meshTxtrHistory[i].normalMap.duplicateTexture(getModel()->meshes[i].normalMap);
                if(heightMapHistoryUsed)
                    meshTxtrHistory[i].heightMap.duplicateTexture(getModel()->meshes[i].heightMap);
                if(aoHistoryUsed)
                    meshTxtrHistory[i].ambientOcclusion.duplicateTexture(getModel()->meshes[i].ambientOcclusion);
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

    if(!this->panel.hover && *Mouse::LClick() && !logDialog.isHovered() || (getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) && textRenderer.keyInput) && !this->materialSelection && !this->textureSelection){
        if(!faceSelectionMode)
            this->dialogControl.unActivate();
        else
            faceSelectionMode = false;
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

    glClear(GL_DEPTH_BUFFER_BIT);
}



void ObjectTexturingDialog::updateDisplayingTexture(LogDialog& logDialog){
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
    
    this->displayingFBO.bind();
    
    //Set the OpenGL viewport to the resolution of the material displaying texture
    glViewport(0,0, DISPLAY_RESOLUTION.x, DISPLAY_RESOLUTION.y);

    //Clear the capture framebuffer (displaying texture) with alpha zero color
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Use the 3D model rendering shader
    ShaderSystem::tdModelShader().use();

    //Throw the camera data to the shader
    ShaderSystem::tdModelShader().setVec3("viewPos", this->sceneCam.cameraPos);
    ShaderSystem::tdModelShader().setMat4("view",view);
    ShaderSystem::tdModelShader().setMat4("projection",projectionMatrix);
    ShaderSystem::tdModelShader().setMat4("modelMatrix", glm::mat4(1));

    ShaderSystem::tdModelShader().setInt("displayingMode", this->displayingOptionsComboBox.selectedIndex);
    
    for (size_t i = 0; i < getModel()->meshes.size(); i++)
    {
        //Bind the channels of the material
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texturesMesh[i].albedo.ID);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texturesMesh[i].roughness.ID);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, texturesMesh[i].metallic.ID);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, texturesMesh[i].normalMap.ID);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, texturesMesh[i].heightMap.ID);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, texturesMesh[i].ambientOcclusion.ID);
        
        ShaderSystem::tdModelShader().setInt("usingMeshSelection", !this->faceSelectionMode);
        ShaderSystem::tdModelShader().setInt("meshSelectionEditing", this->faceSelectionMode);
        ShaderSystem::tdModelShader().setInt("hideUnselected", !this->faceSelectionMode);
        ShaderSystem::tdModelShader().setInt("primitiveCount", getModel()->meshes[i].indices.size() / 3);

        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_2D, faceSelection[i].selectedFaces.ID);
        
        glActiveTexture(GL_TEXTURE12);
        glBindTexture(GL_TEXTURE_2D, faceSelection[i].meshMask.ID);
    
        getModel()->meshes[i].Draw(faceSelectionMode);
    }
    
    if((*Mouse::LPressed() || shortcuts_CTRL_A()) && this->faceSelectionMode && !this->anyElementHover()){
        glm::vec2 cursorPos = *Mouse::cursorPos();
        cursorPos -= glm::vec2(*Settings::videoScale() * glm::vec2(0.1f));    
        cursorPos *= glm::vec2(glm::vec2(1.25f));   
        
        if((*Mouse::LClick() && !logDialog.isHovered()) || shortcuts_CTRL_A()){
            std::vector<std::vector<byte>> primitivesArray, prevPrimArray;

            for (size_t i = 0; i < faceSelection.size(); i++)
            {
                primitivesArray.push_back(faceSelection[i].selectedPrimitiveIDs);
                prevPrimArray.push_back(faceSelection[i].prevPrimArray);
            }
            
            registerFaceSelectionActionObjectTexturingDialog("Face selection - Object Texturing Dialog", primitivesArray, prevPrimArray);
        } 

        for (size_t i = 0; i < this->faceSelection.size(); i++)
        {
            this->faceSelection[i].interaction(getModel()->meshes[i], i, true, view, projectionMatrix, glm::mat4(1), cursorPos, true, false);
        }
    }

    ShaderSystem::tdModelShader().setInt("usingMeshSelection", false);
    ShaderSystem::tdModelShader().setInt("meshSelectionEditing", false);
    ShaderSystem::tdModelShader().setInt("hideUnselected", false);

    // 190 - 1727
    // 114 - 967
    
    //!Finish (prepare rendering the GUI)

    //Use the button shader (Is necessary since that process is done in the middle of GUI rendering) 
    ShaderSystem::buttonShader().use();

    //Bind the default framebuffer
    Settings::defaultFramebuffer()->FBO.bind();
    
    //Set the OpenGL viewport to default
    Settings::defaultFramebuffer()->setViewport();

    getBox()->bindBuffers();
}

#define MIX_CHANNELS(mask, normalMapMode, invert)   getModel()->meshes[meshI].albedo.mix(albedo, mask, false, normalMapMode, invert);\
                                                    getModel()->meshes[meshI].roughness.mix(roughness, mask, false, normalMapMode, invert);\
                                                    getModel()->meshes[meshI].metallic.mix(metallic, mask, false, normalMapMode, invert);\
                                                    getModel()->meshes[meshI].normalMap.mix(normalMap, mask, false, normalMapMode, invert);\
                                                    getModel()->meshes[meshI].heightMap.mix(heightMap, mask, false, normalMapMode, invert);\
                                                    getModel()->meshes[meshI].ambientOcclusion.mix(ambientOcclusion, mask, false, normalMapMode, invert)

static Texture maskDuplicated;

void ObjectTexturingDialog::updateMeshTextures(){
    for (size_t meshI = 0; meshI < getModel()->meshes.size(); meshI++)
    {
        Texture albedo = getModel()->meshes[meshI].albedo; 
        Texture roughness = getModel()->meshes[meshI].roughness; 
        Texture metallic = getModel()->meshes[meshI].metallic; 
        Texture normalMap = getModel()->meshes[meshI].normalMap; 
        Texture heightMap = getModel()->meshes[meshI].heightMap; 
        Texture ambientOcclusion = getModel()->meshes[meshI].ambientOcclusion; 
        
        getModel()->meshes[meshI].albedo = texturesMesh[meshI].albedo;
        getModel()->meshes[meshI].roughness = texturesMesh[meshI].roughness;
        getModel()->meshes[meshI].metallic = texturesMesh[meshI].metallic;
        getModel()->meshes[meshI].normalMap = texturesMesh[meshI].normalMap;
        getModel()->meshes[meshI].heightMap = texturesMesh[meshI].heightMap;
        getModel()->meshes[meshI].ambientOcclusion = texturesMesh[meshI].ambientOcclusion;
        
        for (int i = this->material.materialModifiers.size() - 1; i >= 0; --i)    
        {
            if(meshI < this->faceSelection.size())
                this->material.materialModifiers[i].updateMaterialChannels(this->material, getModel()->meshes[meshI], getResolution(), i, this->faceSelection[meshI].meshMask, this->faceSelection[meshI].selectedFaces, false, *getModel());
        }
        
        Texture maskMat = maskMaterialBtn.texture.generateProceduralTexture(getModel()->meshes[meshI], this->getResolution());

        // Mask channels
        MIX_CHANNELS(maskMat, false, false);
        
        Texture maskTxtr;

        // Mask channels via channel textures
        if(mixOptionsComboBox.selectedIndex == 3){ // Roughness
            maskTxtr = getModel()->meshes[meshI].roughness; 
        }
        if(mixOptionsComboBox.selectedIndex == 4){ // Metallic
            maskTxtr = getModel()->meshes[meshI].metallic; 
        }
        if(mixOptionsComboBox.selectedIndex == 5){ // AO
            maskTxtr = getModel()->meshes[meshI].ambientOcclusion; 
        }
        if(mixOptionsComboBox.selectedIndex == 6){ // Height Map
            maskTxtr = getModel()->meshes[meshI].heightMap; 
        }
        if(mixOptionsComboBox.selectedIndex == 7 || mixOptionsComboBox.selectedIndex == 8){ // Normal Map
            maskTxtr = getModel()->meshes[meshI].normalMap; 
        }
        
        if(mixOptionsComboBox.selectedIndex){

            if(mixOptionsComboBox.selectedIndex == 1 || mixOptionsComboBox.selectedIndex == 2){
                std::vector<Texture> orgTxtrs = {albedo, roughness, metallic, normalMap, heightMap, ambientOcclusion};
                std::vector<Texture> modifiedTxtrs = {getModel()->meshes[meshI].albedo, getModel()->meshes[meshI].roughness, getModel()->meshes[meshI].metallic, getModel()->meshes[meshI].normalMap, getModel()->meshes[meshI].heightMap, getModel()->meshes[meshI].ambientOcclusion};
            
                for (size_t i = 0; i < orgTxtrs.size(); i++)
                {
                    maskTxtr = modifiedTxtrs[i];
                    glm::ivec2 maskRes = maskTxtr.getResolution();
                    if(maskDuplicated.ID == 0){
                        maskDuplicated = Texture(nullptr, maskRes.x, maskRes.y);
                    }
                    else{
                        maskDuplicated.update(nullptr, maskRes.x, maskRes.y);
                    }
                    
                    maskTxtr.duplicateTextureSub(maskDuplicated);
                
                    modifiedTxtrs[i].mix(orgTxtrs[i], maskDuplicated, false, i == 3, mixOptionsComboBox.selectedIndex == 2);
                }
                
            }
            else{
                glm::ivec2 maskRes = maskTxtr.getResolution();
                if(maskDuplicated.ID == 0){
                    maskDuplicated = Texture(nullptr, maskRes.x, maskRes.y);
                }
                else{
                    maskDuplicated.update(nullptr, maskRes.x, maskRes.y);
                }
                
                maskTxtr.duplicateTextureSub(maskDuplicated);

                MIX_CHANNELS(maskDuplicated, mixOptionsComboBox.selectedIndex == 7 || mixOptionsComboBox.selectedIndex == 8, mixOptionsComboBox.selectedIndex != 8);
            }
        }

        if(!albedoChannelCheckBox.clickState1)
            getModel()->meshes[meshI].albedo.mix(albedo, appTextures.white, false, false, false);
        if(!roughnessChannelCheckBox.clickState1)
            getModel()->meshes[meshI].roughness.mix(roughness, appTextures.white, false, false, false);
        if(!metallicChannelCheckBox.clickState1)
            getModel()->meshes[meshI].metallic.mix(metallic, appTextures.white, false, false, false);
        if(!normalMapChannelCheckBox.clickState1)
            getModel()->meshes[meshI].normalMap.mix(normalMap, appTextures.white, false, false, false);
        if(!heightMapChannelCheckBox.clickState1)
            getModel()->meshes[meshI].heightMap.mix(heightMap, appTextures.white, false, false, false);
        if(!aoChannelCheckBox.clickState1)
            getModel()->meshes[meshI].ambientOcclusion.mix(ambientOcclusion, appTextures.white, false, false, false);

        getModel()->meshes[meshI].albedo = albedo; 
        getModel()->meshes[meshI].roughness = roughness; 
        getModel()->meshes[meshI].metallic = metallic; 
        getModel()->meshes[meshI].normalMap = normalMap; 
        getModel()->meshes[meshI].heightMap = heightMap; 
        getModel()->meshes[meshI].ambientOcclusion = ambientOcclusion; 
    }
}

bool ObjectTexturingDialog::anyElementHover(){
    return maskViaFaceSelection.hover || maskViaTexture.hover || cancelMasks.hover || assignRelatedTexturesButton.hover || materialDisplayerButton.hover || editMaterialButton.hover || selectMaterialButton.hover;
}

int ObjectTexturingDialog::getResolution(){
    for (size_t i = 0; i < getModel()->meshes.size(); i++)
    {
        if(getModel()->meshes[i].albedo.ID)
            return getModel()->meshes[i].albedo.getResolution().x;

        else if(getModel()->meshes[i].roughness.ID)
            return getModel()->meshes[i].roughness.getResolution().x;

        else if(getModel()->meshes[i].metallic.ID)
            return getModel()->meshes[i].metallic.getResolution().x;

        else if(getModel()->meshes[i].normalMap.ID)
            return getModel()->meshes[i].normalMap.getResolution().x;

        else if(getModel()->meshes[i].heightMap.ID)
            return getModel()->meshes[i].heightMap.getResolution().x;

        else if(getModel()->meshes[i].ambientOcclusion.ID)
            return getModel()->meshes[i].ambientOcclusion.getResolution().x;
    
        else
            return 1024;
    }

    Settings::defaultFramebuffer()->FBO.bind();
    
}