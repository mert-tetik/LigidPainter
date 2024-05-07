
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 (c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
 Official Web Page : https:ligidtools.com/ligidpainter

 ---------------------------------------------------------------------------

    Display settings dialog.
    (Mostly manipulates the skybox )

 */
#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"

#include "3D/ThreeD.hpp"

#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/GL/GL.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <cmath>

#include "tinyfiledialogs.h"

ComboBox textureResComboBox = ComboBox(ELEMENT_STYLE_BASIC,glm::vec2(6.f, 2.f), {"256", "512", "1024", "2024", "4048", "8096"}, "Texture Resolution", 4.f);

BakingDialog::BakingDialog(int){

    this->selectMeshButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(7.f, 12.f), "", Texture(), 6.f, false);
    this->selectMeshButton.meshSelection = true;
    this->selectMeshButton.textureSizeScale = 1.f;

    pbrResultCheckbox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(5.f,2.f), "Combined", 0.f);
    aoCheckbox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(5.f,2.f), "Ambient Occlusion", 0.f);
    normalMapCheckbox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(8.f,2.f), "Normal Map)", 0.f);
    vertexPositionCheckbox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(5.f,2.f), "Vertex Position", 0.f);
    vertexNormalsCheckbox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(5.f,2.f), "Vertex Normals", 0.f);
    reflectanceCheckbox = CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(5.f,2.f), "Skybox Reflectance", 0.f);
    bakeButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(7.f,2.f), "Bake", Texture(), 1.f, false);
    modeImageDisplayer1 = Button(ELEMENT_STYLE_SOLID, glm::vec2(0.f), "", Texture(), 6.f, false);
    modeImage1ElementsBg = Button(ELEMENT_STYLE_SOLID, glm::vec2(0.f), "", Texture(), 6.f, false);
    modeImageDisplayer2 = Button(ELEMENT_STYLE_SOLID, glm::vec2(0.f), "", Texture(), 6.f, false);
    modeImage2ElementsBg = Button(ELEMENT_STYLE_SOLID, glm::vec2(0.f), "", Texture(), 6.f, false);
    renderModeLBtn = Button(ELEMENT_STYLE_BASIC, glm::vec2(0.f), "", Texture(), 6.f, false);
    renderModeRBtn = Button(ELEMENT_STYLE_BASIC, glm::vec2(0.f), "", Texture(), 6.f, false);
}

float easeInOut(float t) {
    return 0.5f * (1.0f - std::cos(t * 3.14159265358979323846));
}

void BakingDialog::show(Timer& timer, Skybox skybox){
    
    dialogControl.activate();

    while(!getContext()->window.shouldClose()){
        dialogControl.updateStart();

        if(dialogControl.firstFrameActivated){
            selectMeshButton.selectedMeshI = 0;
            if(selectMeshButton.selectedMeshI < getScene()->model->meshes.size()){
                selectMeshButton.texture = getScene()->model->meshes[selectMeshButton.selectedMeshI].displayingTxtr; 
                selectMeshButton.text = getScene()->model->meshes[selectMeshButton.selectedMeshI].materialName; 
            }
        }

        glm::vec3 pos = glm::vec3(50.f,40.f,0.7f);
        glm::vec2 scale = glm::vec2(25.f, 25.f / (appTextures.baking_Default.getResolution().x / appTextures.baking_Default.getResolution().y) * (Settings::videoScale()->x / Settings::videoScale()->y));

        glm::vec3 resultPos = glm::vec3(UTIL::getPercent(*Settings::videoScale(), glm::vec2(pos.x,pos.y)), pos.z); 
        glm::vec2 resultScale = UTIL::getPercent(*Settings::videoScale(), scale);

        this->renderElements(timer, pos, scale, resultPos, resultScale);

        if(this->bakeButton.clicked && selectMeshButton.selectedMeshI < getScene()->model->meshes.size()){
            Texture baked = this->bake(
                                        skybox, 
                                        std::stoi(textureResComboBox.texts[textureResComboBox.selectedIndex])
                                    );    

            Library::addTexture(baked, "New texture via baking");
        } 

        //End the dialog
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || (!this->isDialogHovered() && !dialog_log.isHovered() && *Mouse::LClick())){
            if(!dialogControl.firstFrameActivated){
                dialogControl.unActivate();
            }
        }
        
        dialogControl.updateEnd(timer,0.15f);

        if(dialogControl.mixVal == 0.f)
            break;
    }
}














/* UTILITY FUNCTIONS */
void BakingDialog::renderElements(Timer& timer, glm::vec3 pos, glm::vec2 scale, glm::vec3 resultPos, glm::vec2 resultScale){
    
    //Right
    ShaderSystem::buttonShader().setVec3("pos", glm::vec3(resultPos.x + resultScale.x + resultScale.x * 2.f, resultPos.y,   1.f)); 
    ShaderSystem::buttonShader().setVec2("scale", glm::vec2(resultScale * 2.f));
    ShaderSystem::buttonShader().setFloat("properties.radius", 0.f); 
    ShaderSystem::buttonShader().setInt("outlineExtra" , false); 
    ShaderSystem::buttonShader().setVec4("properties.color", glm::vec4(0)); //Invisible
    ShaderSystem::buttonShader().setVec3("properties.outline.color", glm::vec4(0)); //Invisible
    getBox()->draw("Panel : Barrier bottom");
    
    //Left
    ShaderSystem::buttonShader().setVec3("pos", glm::vec3(resultPos.x - resultScale.x - resultScale.x * 2.f, resultPos.y,   1.f));
    ShaderSystem::buttonShader().setVec2("scale", glm::vec2(resultScale * 2.f));
    getBox()->draw("Panel : Barrier top");

    modeImageDisplayer1.scale = glm::vec2(scale.x, scale.y);
    modeImageDisplayer1.pos = glm::vec3(pos.x - modeImageDisplayer1.scale.x * 2.f * easeInOut(rendererModeMixVal), pos.y - scale.y + modeImageDisplayer1.scale.y, pos.z + 0.01f); 
    modeImageDisplayer1.texture = appTextures.baking_Default;
    modeImageDisplayer1.outline = false;
    modeImageDisplayer1.color = glm::vec4(0.f);
    modeImageDisplayer1.stretchTexture = true;
    modeImageDisplayer1.render(timer, false);

    modeImage1ElementsBg.scale = glm::vec2(modeImageDisplayer1.scale.x, 3.f);
    modeImage1ElementsBg.pos = glm::vec3(modeImageDisplayer1.pos.x, modeImageDisplayer1.pos.y + modeImage1ElementsBg.scale.y + modeImageDisplayer1.scale.y, modeImageDisplayer1.pos.z);
    modeImage1ElementsBg.color = glm::vec4(55.f / 255.f, 86.f / 255.f, 100.f / 255.f, 1.f);
    modeImage1ElementsBg.render(timer, false);

    this->vertexPositionCheckbox.pos = modeImage1ElementsBg.pos;
    this->vertexPositionCheckbox.pos.x -= scale.x - this->vertexPositionCheckbox.scale.x - 8.f;
    this->vertexPositionCheckbox.render(timer, true);
    
    this->vertexNormalsCheckbox.pos = this->vertexPositionCheckbox.pos;
    this->vertexNormalsCheckbox.pos.x += this->vertexPositionCheckbox.scale.x + this->vertexNormalsCheckbox.scale.x;
    this->vertexNormalsCheckbox.render(timer, true);
    
    this->reflectanceCheckbox.pos = this->vertexNormalsCheckbox.pos;
    this->reflectanceCheckbox.pos.x += this->vertexNormalsCheckbox.scale.x + this->reflectanceCheckbox.scale.x;
    this->reflectanceCheckbox.render(timer, true);

    modeImageDisplayer2.scale = glm::vec2(scale.x, modeImageDisplayer1.scale.y);
    modeImageDisplayer2.pos = glm::vec3(pos.x + modeImageDisplayer1.scale.x * 2.f - modeImageDisplayer1.scale.x * 2.f * easeInOut(rendererModeMixVal), pos.y - scale.y + modeImageDisplayer2.scale.y, pos.z + 0.01f); 
    modeImageDisplayer2.texture = appTextures.baking_LigidRay;
    modeImageDisplayer2.outline = false;
    modeImageDisplayer2.color = glm::vec4(0.f);
    modeImageDisplayer2.stretchTexture = true;
    modeImageDisplayer2.render(timer, false);
    
    modeImage2ElementsBg.scale = glm::vec2(modeImageDisplayer2.scale.x, 3.f);
    modeImage2ElementsBg.pos = glm::vec3(modeImageDisplayer2.pos.x, modeImageDisplayer2.pos.y + modeImage2ElementsBg.scale.y + modeImageDisplayer2.scale.y, modeImageDisplayer2.pos.z);
    modeImage2ElementsBg.color = glm::vec4(125.f / 255.f, 103.f / 255.f, 85.f / 255.f, 1.f);
    modeImage2ElementsBg.render(timer, false);

    this->pbrResultCheckbox.pos = modeImage2ElementsBg.pos;
    this->pbrResultCheckbox.pos.x -= scale.x - this->pbrResultCheckbox.scale.x - 8.f;
    this->pbrResultCheckbox.render(timer, true);
    
    this->aoCheckbox.pos = this->pbrResultCheckbox.pos;
    this->aoCheckbox.pos.x += this->pbrResultCheckbox.scale.x + this->aoCheckbox.scale.x;
    this->aoCheckbox.render(timer, true);
    
    this->normalMapCheckbox.pos = this->aoCheckbox.pos;
    this->normalMapCheckbox.pos.x += this->aoCheckbox.scale.x + this->reflectanceCheckbox.scale.x;
    this->normalMapCheckbox.render(timer, true);
    
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", rendererModeMixVal); 
    renderModeLBtn.scale = glm::vec2(3.5f, modeImageDisplayer1.scale.y);
    renderModeLBtn.pos = glm::vec3(pos.x - modeImageDisplayer2.scale.x + renderModeLBtn.scale.x, modeImageDisplayer2.pos.y, modeImageDisplayer2.pos.z + 0.02f); 
    renderModeLBtn.texture = appTextures.arrowL;
    renderModeLBtn.color.a = renderModeLBtn.hoverMixVal + 0.5f;
    renderModeLBtn.render(timer, rendererMode == 1);

    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f - rendererModeMixVal); 
    renderModeRBtn.scale = glm::vec2(3.5f, modeImageDisplayer1.scale.y);
    renderModeRBtn.pos = glm::vec3(pos.x + modeImageDisplayer2.scale.x - renderModeLBtn.scale.x, modeImageDisplayer2.pos.y, modeImageDisplayer2.pos.z + 0.02f); 
    renderModeRBtn.texture = appTextures.arrowR;
    renderModeRBtn.color.a = renderModeRBtn.hoverMixVal + 0.5f;
    renderModeRBtn.render(timer, rendererMode == 0);
    
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f); 
    
    if(renderModeLBtn.clicked)
        rendererMode = 0;
    if(renderModeRBtn.clicked)
        rendererMode = 1;

    timer.transition(rendererMode, rendererModeMixVal, 0.5f);

    this->selectMeshButton.scale.x = modeImageDisplayer2.scale.x;
    this->selectMeshButton.scale.y = 5.f;
    this->selectMeshButton.pos = glm::vec3(pos.x, modeImage1ElementsBg.pos.y + modeImage1ElementsBg.scale.y + this->selectMeshButton.scale.y + 2.f, pos.z + 0.02f);
    
    this->selectMeshButton.render(timer, true);

    this->bakeButton.pos = glm::vec3(modeImage1ElementsBg.pos.x, modeImage1ElementsBg.pos.y - this->bakeButton.scale.y - modeImage1ElementsBg.scale.y - 1.f, modeImage1ElementsBg.pos.z + 0.04f);
    this->bakeButton.render(timer, rendererMode == 0);
    
    Button textureResComboBoxBG = Button(ELEMENT_STYLE_BASIC, glm::vec2(6.f,2.f), "", Texture(), 1.f, false);
    textureResComboBox.pos = glm::vec3(bakeButton.pos.x, bakeButton.pos.y - textureResComboBox.scale.y - bakeButton.scale.y - 1.f, bakeButton.pos.z + 0.04f);
    textureResComboBoxBG.pos = textureResComboBox.pos;
    textureResComboBoxBG.render(timer, true);
    textureResComboBox.render(timer, true);

    Button rendererMode0HeaderText = Button(ELEMENT_STYLE_SOLID, glm::vec2(7.f,2.f), "Baking Dialog - LigidPainter Renderer", Texture(), 1.f, false);
    rendererMode0HeaderText.pos = glm::vec3(modeImageDisplayer1.pos.x, modeImageDisplayer1.pos.y - modeImageDisplayer1.scale.y - rendererMode0HeaderText.scale.y, modeImageDisplayer1.pos.z);
    rendererMode0HeaderText.scale.x = modeImageDisplayer1.scale.x;
    rendererMode0HeaderText.color = modeImage1ElementsBg.color;
    rendererMode0HeaderText.textScale = 0.75f;
    rendererMode0HeaderText.render(timer, false);

    Button rendererMode1HeaderText = Button(ELEMENT_STYLE_SOLID, glm::vec2(7.f,2.f), "Baking Dialog - LigidRay Renderer (NOT AVAILABLE)", Texture(), 1.f, false);
    rendererMode1HeaderText.pos = glm::vec3(modeImageDisplayer2.pos.x, modeImageDisplayer2.pos.y - modeImageDisplayer2.scale.y - rendererMode1HeaderText.scale.y, modeImageDisplayer2.pos.z);
    rendererMode1HeaderText.scale.x = modeImageDisplayer1.scale.x;
    rendererMode1HeaderText.color = modeImage2ElementsBg.color;
    rendererMode1HeaderText.textScale = 0.75f;
    rendererMode1HeaderText.render(timer, false);

    std::vector<CheckBox*> checkBoxes; 
    checkBoxes.push_back(&pbrResultCheckbox);
    checkBoxes.push_back(&aoCheckbox);
    checkBoxes.push_back(&normalMapCheckbox);
    for (size_t i = 0; i < checkBoxes.size(); i++){
        if(checkBoxes[i]->clickState1 && i != this->selectedBakeMode){
            this->selectedBakeMode = i;
            for (size_t j = 0; j < checkBoxes.size(); j++)
            {
                checkBoxes[j]->clickState1 = false;
            }
        }
    }

    for (size_t i = 0; i < checkBoxes.size(); i++)
    {
        if(checkBoxes[i]->clickState1 == false && i == this->selectedBakeMode)
            checkBoxes[i]->clickState1 = true;
    }
    
    checkBoxes.clear();

    checkBoxes.push_back(&vertexPositionCheckbox);
    checkBoxes.push_back(&vertexNormalsCheckbox);
    checkBoxes.push_back(&reflectanceCheckbox);
    for (size_t i = 0; i < checkBoxes.size(); i++){
        if(checkBoxes[i]->clickState1 && i != this->selectedBakeMode){
            this->selectedBakeMode = i;
            for (size_t j = 0; j < checkBoxes.size(); j++)
            {
                checkBoxes[j]->clickState1 = false;
            }
        }
    }

    for (size_t i = 0; i < checkBoxes.size(); i++)
    {
        if(checkBoxes[i]->clickState1 == false && i == this->selectedBakeMode)
            checkBoxes[i]->clickState1 = true;
    }


}

Texture BakingDialog::bake(Skybox skybox, unsigned int resolution){

    Camera cam;

    //Move the camera to the side
    glm::mat4 view = glm::lookAt(cam.cameraPos, 
                                    glm::vec3(0), 
                                    glm::vec3(0.0, 1.0, 0.0));

    //The perspective projection matrix    
    glm::mat4 projectionMatrix = glm::perspective(
                                                    glm::radians(35.f), //Fov  
                                                    -1.f,  //Ratio (is 1 since the width & the height is equal to displayRes)
                                                    100.f,  //Near (the material is pretty close to the camera actually  ) 
                                                    0.1f    //Far
                                                );

    ShaderSystem::bakingShader().use();
    ShaderSystem::bakingShader().setVec3("viewPos", cam.cameraPos); 
    ShaderSystem::bakingShader().setInt("skybox", GL::bindTexture_CubeMap(skybox.ID, "BakingDialog::bake"));
    ShaderSystem::bakingShader().setInt("prefilterMap", GL::bindTexture_CubeMap(skybox.IDPrefiltered, "BakingDialog::bake"));
    ShaderSystem::bakingShader().setInt("albedoTxtr", GL::bindTexture_2D(getScene()->model->meshes[selectMeshButton.selectedMeshI].material_channels.albedo.ID, "BakingDialog::bake"));
    ShaderSystem::bakingShader().setInt("roughnessTxtr", GL::bindTexture_2D(getScene()->model->meshes[selectMeshButton.selectedMeshI].material_channels.roughness.ID, "BakingDialog::bake"));
    ShaderSystem::bakingShader().setInt("metallicTxtr", GL::bindTexture_2D(getScene()->model->meshes[selectMeshButton.selectedMeshI].material_channels.metallic.ID, "BakingDialog::bake"));
    ShaderSystem::bakingShader().setInt("normalMapTxtr", GL::bindTexture_2D(getScene()->model->meshes[selectMeshButton.selectedMeshI].material_channels.normalMap.ID, "BakingDialog::bake"));
    ShaderSystem::bakingShader().setInt("heightMapTxtr", GL::bindTexture_2D(getScene()->model->meshes[selectMeshButton.selectedMeshI].material_channels.heightMap.ID, "BakingDialog::bake"));
    ShaderSystem::bakingShader().setInt("ambientOcclusionTxtr", GL::bindTexture_2D(getScene()->model->meshes[selectMeshButton.selectedMeshI].material_channels.ambientOcclusion.ID, "BakingDialog::bake"));
    ShaderSystem::bakingShader().setInt("modeIndex", this->selectedBakeMode); 
    ShaderSystem::bakingShader().setInt("useLights", this->aoCheckbox.clickState1); 

    ShaderSystem::bakingShader().setMat4("orthoProjection", glm::ortho(0.f, 1.f, 0.f, 1.f)); 
    ShaderSystem::bakingShader().setMat4("perspectiveProjection", projectionMatrix); 
    ShaderSystem::bakingShader().setMat4("view", view); 

    Texture txtr = Texture((char*)nullptr, resolution, resolution);
    txtr.title = "baked_" + getScene()->model->meshes[selectMeshButton.selectedMeshI].materialName;

    /* Capturing FBO */
    unsigned int FBO; 
    glGenFramebuffers(1,&FBO);
    glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, txtr.ID, 0);
    glClearColor(0,0,0,0);

    unsigned int RBO;
    glGenRenderbuffers(1,&RBO);
    glBindRenderbuffer(GL_RENDERBUFFER,RBO);
    
    //Set the renderbuffer to store depth
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, resolution, resolution);
    
    //Give the renderbuffer to the framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glDepthFunc(GL_LEQUAL);
    
    getScene()->model->meshes[selectMeshButton.selectedMeshI].Draw();

    GL::releaseBoundTextures("BakingDialog::bake");

    glDeleteFramebuffers(1, &FBO);
    glDeleteRenderbuffers(1, &RBO);

    return txtr;
}

bool BakingDialog::isDialogHovered(){
    return  ((Mouse::isMouseHover(this->modeImageDisplayer1.resultScale, this->modeImageDisplayer1.resultPos) || Mouse::isMouseHover(this->modeImage1ElementsBg.resultScale, this->modeImage1ElementsBg.resultPos)) && this->rendererMode == 0) ||
            ((Mouse::isMouseHover(this->modeImageDisplayer2.resultScale, this->modeImageDisplayer2.resultPos) || Mouse::isMouseHover(this->modeImage2ElementsBg.resultScale, this->modeImage2ElementsBg.resultPos)) && this->rendererMode == 1) ||
            this->selectMeshButton.hover;
}