
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

#include "GUI/GUI.hpp"
#include "LibrarySystem/Library.hpp"
#include "MouseSystem/Mouse.hpp" 
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

#include "tinyfiledialogs.h"

#define DISPLAY_RESOLUTION glm::vec2(this->panel.scale * 30.f)

ObjectTexturingDialog::ObjectTexturingDialog(){
    //Create the panel
    this->panel = Panel(
        {
            {
                Section(
                    Element(Button()),
                    {
                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,40.f), ""  , Texture(), 1.f, false)),
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

    this->maskViaFaceSelection = Button(ELEMENT_STYLE_BASIC, glm::vec2(6, 2.f), "Mask via face selection", Texture(), 1.f, false);
    this->maskViaTexture = Button(ELEMENT_STYLE_BASIC, glm::vec2(6, 2.f), "Mask via texture", Texture(), 1.f, false);
    
    this->assignRelatedTexturesButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(6, 2.f), "Assign to related textures", Texture(), 1.f, false);
    this->createNewTextureButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(6, 2.f), "Create new texture and assign", Texture(), 1.f, false);
    this->assignToCustomTexturesButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(6, 2.f), "Assign to custom textures", Texture(), 1.f, false);
    
    this->materialDisplayerButton = Button(ELEMENT_STYLE_SOLID, glm::vec2(6, 4.f), "", Texture(), 1.f, false);
    this->editMaterialButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(6, 2.f), "Edit material", Texture(), 1.f, false);
    this->selectMaterialButton = Button(ELEMENT_STYLE_BASIC, glm::vec2(2.f, 4.f), "", Settings::appTextures().arrowB, 1.f, false);

    this->displayingTexture = Texture(nullptr, DISPLAY_RESOLUTION.x, DISPLAY_RESOLUTION.y);

    this->displayingFBO = Framebuffer(this->displayingTexture, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, DISPLAY_RESOLUTION));

}

void ObjectTexturingDialog::render(Timer timer, glm::mat4 projection){
    dialogControl.updateStart();

    if(dialogControl.firstFrameActivated){
        this->sceneCam.setCameraPosition(glm::vec3(0,0,-3.5f));
        this->sceneCam.radius = 3.5f;
    }

    // Update the scene texture
    updateDisplayingTexture();

    // Modifying the elements    
    this->panel.sections[0].elements[0].button.scale = this->panel.scale;

    this->maskViaFaceSelection.pos = this->panel.pos;
    this->maskViaFaceSelection.pos.x -= this->panel.scale.x - this->maskViaFaceSelection.scale.x - 2.f;
    this->maskViaFaceSelection.pos.y += 15.f;
    this->maskViaTexture.pos = this->maskViaFaceSelection.pos;
    this->maskViaTexture.pos.y += this->maskViaTexture.scale.y + this->maskViaFaceSelection.scale.y;

    this->assignRelatedTexturesButton.pos = maskViaTexture.pos;
    this->assignRelatedTexturesButton.pos.y += assignRelatedTexturesButton.scale.y + maskViaTexture.scale.y * 2.f;
    this->createNewTextureButton.pos = assignRelatedTexturesButton.pos;
    this->createNewTextureButton.pos.y += createNewTextureButton.scale.y + assignRelatedTexturesButton.scale.y;
    this->assignToCustomTexturesButton.pos = createNewTextureButton.pos;
    this->assignToCustomTexturesButton.pos.y += assignToCustomTexturesButton.scale.y + createNewTextureButton.scale.y;

    this->materialDisplayerButton.pos = this->panel.pos;
    this->materialDisplayerButton.pos.x += this->panel.scale.x - this->materialDisplayerButton.scale.x - 6.f;
    this->materialDisplayerButton.pos.y -= 25.f;
    this->editMaterialButton.pos = this->materialDisplayerButton.pos;
    this->editMaterialButton.pos.y += this->editMaterialButton.scale.y + this->materialDisplayerButton.scale.y;
    this->selectMaterialButton.pos = this->materialDisplayerButton.pos;
    this->selectMaterialButton.pos.x += this->selectMaterialButton.scale.x + this->materialDisplayerButton.scale.x;

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
    this->assignRelatedTexturesButton.render(timer, !this->materialSelection && !this->textureSelection);
    this->createNewTextureButton.render(timer, !this->materialSelection && !this->textureSelection);
    this->assignToCustomTexturesButton.render(timer, !this->materialSelection && !this->textureSelection);
    this->materialDisplayerButton.render(timer, !this->materialSelection && !this->textureSelection);
    this->editMaterialButton.render(timer, !this->materialSelection && !this->textureSelection);
    this->selectMaterialButton.render(timer, !this->materialSelection && !this->textureSelection);

    // Element interactions
    if(this->selectMaterialButton.clicked){
        this->materialSelection = true;
    }
    else if(this->assignToCustomTexturesButton.clicked){
        this->textureSelection = true;
    }

    if(!this->panel.hover && *Mouse::LClick() || getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) && !this->materialSelection)
        this->dialogControl.unActivate();

    dialogControl.updateEnd(timer,0.15f);
}

void ObjectTexturingDialog::updateDisplayingTexture(){
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

    
    //Bind the channels of the material
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, getSphereModel()->meshes[0].albedo.ID);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, getSphereModel()->meshes[0].roughness.ID);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, getSphereModel()->meshes[0].metallic.ID);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, getSphereModel()->meshes[0].normalMap.ID);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, getSphereModel()->meshes[0].heightMap.ID);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, getSphereModel()->meshes[0].ambientOcclusion.ID);
    
    ShaderSystem::tdModelShader().setInt("displayingMode", 0);
    
    for (size_t i = 0; i < getModel()->meshes.size(); i++)
    {
        ShaderSystem::tdModelShader().setInt("usingMeshSelection", true);
        ShaderSystem::tdModelShader().setInt("meshSelectionEditing", false);
        ShaderSystem::tdModelShader().setInt("hideUnselected", true);
        ShaderSystem::tdModelShader().setInt("primitiveCount", getModel()->meshes[i].indices.size() / 3);

        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_2D, getModel()->meshes[i].selectedObjectPrimitivesTxtr.ID);
    
        getModel()->meshes[i].Draw(false);
    }
    
    
    //!Finish (prepare rendering the GUI)

    //Use the button shader (Is necessary since that process is done in the middle of GUI rendering) 
    ShaderSystem::buttonShader().use();

    //Bind the default framebuffer
    Settings::defaultFramebuffer()->FBO.bind();
    
    //Set the OpenGL viewport to default
    Settings::defaultFramebuffer()->setViewport();

    getBox()->bindBuffers();
}