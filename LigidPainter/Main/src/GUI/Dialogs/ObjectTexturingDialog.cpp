
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 Copyright (c) 2022-2023, Mert Tetik

 All rights reserved.

 Official GitHub Link : https:github.com/mert-tetik/LigidPainter
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

static AppMaterialModifiers appMatMods;

ObjectTexturingDialog::ObjectTexturingDialog(){}
ObjectTexturingDialog::ObjectTexturingDialog(AppMaterialModifiers appMaterialModifiers){
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
    
    this->ctrlInfoBtn = Button(ELEMENT_STYLE_BASIC, glm::vec2(7, 4.f), "CTRL - Unselect the selected faces", Texture(), 1.f, false);
    this->shiftInfoBtn = Button(ELEMENT_STYLE_BASIC, glm::vec2(7, 4.f), "SHIFT - Add to selected faces", Texture(), 1.f, false);
    this->ctrlInfoBtn.color = glm::vec4(0.);
    this->ctrlInfoBtn.color2 = glm::vec4(0.);
    this->ctrlInfoBtn.textScale = 0.6f;
    this->shiftInfoBtn.color = glm::vec4(0.);
    this->shiftInfoBtn.color2 = glm::vec4(0.);
    this->shiftInfoBtn.textScale = 0.6f;
    
    this->assignRelatedTexturesButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(6, 2.f), "Assign to related textures", Texture(), 1.f, false);

    this->materialDisplayerButton = Button(ELEMENT_STYLE_SOLID, glm::vec2(6, 4.f), "", Texture(), 1.f, false);
    this->editMaterialButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(6, 2.f), "Edit material", Texture(), 1.f, false);
    this->selectMaterialButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(2.f, 4.f), "", Settings::appTextures().arrowB, 1.f, false);

    this->displayingTexture = Texture(nullptr, DISPLAY_RESOLUTION.x, DISPLAY_RESOLUTION.y);

    this->displayingFBO = Framebuffer(this->displayingTexture, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, DISPLAY_RESOLUTION));

    this->sceneCam.setCameraPosition(glm::vec3(0,0,-3.5f));
    this->sceneCam.radius = 3.5f;

    appMatMods = appMaterialModifiers;
}

static bool __materialEditBtn = false;

void ObjectTexturingDialog::render(Timer timer, glm::mat4 projection, MaterialEditorDialog& materialEditorDialog, LogDialog& logDialog){
    dialogControl.updateStart();

    if(this->material.title == "" || materialSelection){

        int matI = -1;
        bool elementClicked = false;

        if(materialSelection && elementSelectionPanel.sections.size()){
            for (size_t i = 0; i < elementSelectionPanel.sections[0].elements.size(); i++)
            {
                if(elementSelectionPanel.sections[0].elements[i].button.hover && *Mouse::LClick()){
                    matI = i;
                    elementClicked = true;
                }
            }
        }

        if(matI >= Library::getMaterialArraySize())
            matI = -1;        

        if(materialSelection && elementClicked || this->material.title == ""){

            materialSelection = false;

            // Delete the buffers of the material
            this->material.deleteBuffers();

            if(matI == -1){
                this->material = Material("ObjectTexturingMaterial", 0);
                this->material.materialModifiers.push_back(appMatMods.solidModifier);
            }
            else{
                this->material = Library::getMaterialObj(matI).duplicateMaterial();
            }

            Camera cam;
            cam.cameraPos = glm::vec3(0,0,-4.f);
            cam.radius = 4.f;                            
            this->material.updateMaterialDisplayingTexture(256, true, cam, 0);
            this->material.displayingTexture.title = "ObjectTexturingMaterial_DisplayingTexture";
            
            if(!dialogControl.firstFrameActivated)
                this->updateMeshTextures();
        }
    }

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
            faceSelection[faceSelection.size()-1].meshMask = Settings::appTextures().white.duplicateTexture();
        
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

    this->ctrlInfoBtn.pos = this->panel.pos;
    this->ctrlInfoBtn.pos.y += this->panel.scale.y - this->ctrlInfoBtn.scale.y;
    this->ctrlInfoBtn.pos.x -= this->ctrlInfoBtn.scale.x + 2;
    this->shiftInfoBtn.pos = this->panel.pos;
    this->shiftInfoBtn.pos.y += this->panel.scale.y - this->ctrlInfoBtn.scale.y;
    this->shiftInfoBtn.pos.x += this->shiftInfoBtn.scale.x + 2;

    this->assignRelatedTexturesButton.pos = cancelMasks.pos;
    this->assignRelatedTexturesButton.pos.y += assignRelatedTexturesButton.scale.y + cancelMasks.scale.y * 2.f;

    this->materialDisplayerButton.pos = this->panel.pos;
    this->materialDisplayerButton.pos.x += this->panel.scale.x - this->materialDisplayerButton.scale.x - 6.f;
    this->materialDisplayerButton.pos.y -= 25.f;
    this->editMaterialButton.pos = this->materialDisplayerButton.pos;
    this->editMaterialButton.pos.y += this->editMaterialButton.scale.y + this->materialDisplayerButton.scale.y;
    this->selectMaterialButton.pos = this->materialDisplayerButton.pos;
    this->selectMaterialButton.pos.x += this->selectMaterialButton.scale.x + this->materialDisplayerButton.scale.x;

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

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->displayingTexture.ID);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    ShaderSystem::buttonShader().use();

    // Render the elements
    this->maskViaFaceSelection.render(timer, !this->materialSelection && !this->textureSelection);
    this->maskViaTexture.render(timer, !this->materialSelection && !this->textureSelection);
    this->cancelMasks.render(timer, !this->materialSelection && !this->textureSelection);
    this->assignRelatedTexturesButton.render(timer, !this->materialSelection && !this->textureSelection);
    this->materialDisplayerButton.render(timer, false);
    this->editMaterialButton.render(timer, !this->materialSelection && !this->textureSelection);
    this->selectMaterialButton.render(timer, !this->materialSelection && !this->textureSelection);
    if(this->faceSelectionMode){
        this->ctrlInfoBtn.render(timer, false);
        this->shiftInfoBtn.render(timer, false);
    }

    // Element interactions
    if(this->selectMaterialButton.clicked){
        this->materialSelection = true;

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

                if(!colorBuffer.ID){
                    LGDLOG::start << "WARNING! : Texture is missing for the " << colorBufferChannelTitle << " channel!" << LGDLOG::end;
                    channelI++;
                    if(channelI == 6)
                        break;
                    goto skipChannel;
                }

                glm::vec2 res = colorBuffer.getResolution();

                Framebuffer FBO = Framebuffer(colorBuffer, GL_TEXTURE_2D);
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

        this->dialogControl.unActivate();
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
                    glDeleteTextures(1, &this->faceSelection[i].meshMask.ID);
                    this->faceSelection[i].meshMask = this->meshMask.generateProceduralTexture(getModel()->meshes[i], this->getResolution());
                }
            }
        }
    
        this->faceSelectionMode = false;
    }
    
    if(!this->editMaterialButton.clicked && __materialEditBtn || assignRelatedTexturesButton.clicked){
        this->updateMeshTextures();
        __materialEditBtn = false;
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
    ShaderSystem::tdModelShader().setInt("displayingMode", 0);
    ShaderSystem::tdModelShader().setVec3("viewPos", this->sceneCam.cameraPos);
    ShaderSystem::tdModelShader().setMat4("view",view);
    ShaderSystem::tdModelShader().setMat4("projection",projectionMatrix);
    ShaderSystem::tdModelShader().setMat4("modelMatrix", glm::mat4(1));

    
    ShaderSystem::tdModelShader().setInt("displayingMode", 0);
    ShaderSystem::tdModelShader().setInt("paintingOverWrap", false);
    
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
    
    if((*Mouse::LPressed() || Shortcuts::CTRL_A()) && this->faceSelectionMode && !this->anyElementHover()){
        glm::vec2 cursorPos = *Mouse::cursorPos();
        cursorPos -= glm::vec2(*Settings::videoScale() * glm::vec2(0.1f));    
        cursorPos *= glm::vec2(glm::vec2(1.25f));   
        
        if((*Mouse::LClick() && !logDialog.isHovered()) || Shortcuts::CTRL_A()){
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
                this->material.materialModifiers[i].updateMaterialChannels(this->material, getModel()->meshes[meshI], this->getResolution(), i, this->faceSelection[meshI].meshMask, this->faceSelection[meshI].selectedFaces);
        }
        
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