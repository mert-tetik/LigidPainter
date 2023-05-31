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

#ifndef LGDUIMATERIALEDITORDIALOG_HPP
#define LGDUIMATERIALEDITORDIALOG_HPP

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.hpp"
#include "Box.hpp"
#include "Renderer.h"
#include "Util.hpp"

#include "GUI/UI.hpp"
#include "GUI/Button.hpp"
#include "Mouse.hpp"
#include "Timer.hpp"

#include "GUI/Dialogs/MaterialModifier.hpp"
#include "GUI/Dialogs/TextureSelectionDialog.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

struct AppMaterialModifiers{
    MaterialModifier textureModifier;
};

class MaterialEditorDialog
{
private:
    Shader buttonShader;
    Shader tdModelShader;
    ColorPalette colorPalette;
    Model sphereModel;
public:
    bool firstFrameActivated = false; //To detect the first frame activated
    bool active = false; //Render the dialog
    glm::vec3 pos = glm::vec3(50.f,50.f,0.8f); ///Position of the dialog
    glm::vec2 scale = glm::vec2(40,40); ///Scale of the dialog
    
    glm::vec3 posLayer = glm::vec3(20.f,50.f,0.8f); ///Position of the layer panel
    glm::vec2 scaleLayer = glm::vec2(10,40); ///Scale of the layer panel
    
    glm::vec3 posModifier = glm::vec3(80.f,50.f,0.8f); ///Position of the layer panel
    glm::vec2 scaleModifier = glm::vec2(10,40); ///Scale of the layer panel

    Button materialDisplayer; //That buttons displays the material with a texture

    Panel bgPanel; //To cover the bg
    Panel layerPanel; //Modifiers will be displayed in there
    Panel modifiersPanel; //Modifiers will be displayed in there

    //Indicates which texture modifier's channel pressed & which one to give the texture
    int textureModifierTextureSelectingButtonIndex = 1000; //1000 if none of them is selecting

    AppMaterialModifiers appMaterialModifiers;

    int selectedMaterialModifierIndex = 0;

    MaterialEditorDialog(){}
    
    MaterialEditorDialog(Shader buttonShader,Shader tdModelShader,ColorPalette colorPalette,AppTextures appTextures,Model &sphereModel){
        this->buttonShader = buttonShader; 
        this->tdModelShader = tdModelShader; 
        this->colorPalette = colorPalette; 
        this->sphereModel = sphereModel; 
        
        bgPanel = Panel
        (
            buttonShader,
            colorPalette,
            {
                // {
                //     Section(
                //             Element(Button()),
                //             {
                //                 Element(Button(1,glm::vec2(30,30),colorPalette,buttonShader,"Add"        , appTextures.greetingDialogImage, 0.f,false))
                //             }
                //         )
                // }
            },
            scale,pos,colorPalette.mainColor,colorPalette.thirdColor,true,true,true,true,true,1.f,1.f,{},20.f);
        
        layerPanel = Panel(buttonShader,colorPalette,{},scaleLayer,posLayer,colorPalette.mainColor,colorPalette.thirdColor,true,true,false,true,true,1.f,1.f,                                
                                {
                                    Button(1,glm::vec2(2,1.5f),colorPalette,buttonShader,"Add"        , Texture(), 0.f,false),
                                    Button(1,glm::vec2(2,1.5f),colorPalette,buttonShader,"Del"        , Texture(), 0.f,false)
                                },
                                20.f
                          );
        modifiersPanel = Panel(buttonShader,colorPalette,{},scaleModifier,posModifier,colorPalette.mainColor,colorPalette.thirdColor,true,false,true,true,true,1.f,1.f,                                
                                {
                                
                                },
                                20.f
                          );
        materialDisplayer = Button(1,glm::vec2(45,45),colorPalette,buttonShader,"Material"        , appTextures.greetingDialogImage, 0.f,false);


        appMaterialModifiers.textureModifier = MaterialModifier("Texture Modifier",colorPalette,buttonShader,appTextures,
            {
                Section(
                    Element(Button(1,glm::vec2(1,2.f),colorPalette,buttonShader,"Channels",Texture(),0.f,true)),
                    {
                        Element(Button(1,glm::vec2(1,1.5f),colorPalette,buttonShader,"Albedo",appTextures.greetingDialogImage,0.f,false)),
                        Element(Button(1,glm::vec2(1,1.5f),colorPalette,buttonShader,"Roughness",appTextures.greetingDialogImage,0.f,false)),
                        Element(Button(1,glm::vec2(1,1.5f),colorPalette,buttonShader,"Metallic",appTextures.greetingDialogImage,0.f,false)),
                        Element(Button(1,glm::vec2(1,1.5f),colorPalette,buttonShader,"Normal map",appTextures.greetingDialogImage,0.f,false)),
                        Element(Button(1,glm::vec2(1,1.5f),colorPalette,buttonShader,"Height map",appTextures.greetingDialogImage,0.f,false)),
                        Element(Button(1,glm::vec2(1,1.5f),colorPalette,buttonShader,"Ambient Occlusion",appTextures.greetingDialogImage,0.f,false))
                    }
                )
            },
            0,
            "LigidPainter/Resources/Shaders/MaterialModifiers/TextureModifier.frag"
        );
        appMaterialModifiers.textureModifier.sections[0].header.button.clickState1 = true;

    }

    void render(glm::vec2 videoScale,Mouse& mouse,Timer &timer,TextRenderer &textRenderer,TextureSelectionDialog &textureSelectionDialog,Library &library,
        Material &material, int textureRes,Box box,Context context){

        //Render the panels & material displayer button
        bgPanel.render(videoScale,mouse,timer,textRenderer,!textureSelectionDialog.active);
        layerPanel.render(videoScale,mouse,timer,textRenderer,!textureSelectionDialog.active);
        modifiersPanel.render(videoScale,mouse,timer,textRenderer,!textureSelectionDialog.active);
        
        //Update material displayer button
        materialDisplayer.texture = Texture(material.displayingTexture);
        materialDisplayer.pos = bgPanel.pos;
        materialDisplayer.scale = glm::vec2(35.f); 
        materialDisplayer.scale.x = (modifiersPanel.pos.x - modifiersPanel.scale.x) - (layerPanel.pos.x + layerPanel.scale.x); 
        materialDisplayer.scale.x /= 2.f;
        materialDisplayer.pos.x = modifiersPanel.pos.x - modifiersPanel.scale.x - materialDisplayer.scale.x;


        //If texture selection dialog is not active reset the index values used to navigate textures
        if(textureSelectionDialog.active == false){
            textureModifierTextureSelectingButtonIndex = 1000;
            textureSelectionDialog.selectedTextureIndex = 1000;
        }


        //Check on modifiers panel if it has to reset.
        //Check if interacted with it's elements
        checkModifiersPanel(material,textureRes,box,context,mouse);

        //Update layer panal elements
        if(layerPanel.barButtons[0].clickedMixVal == 1.f || firstFrameActivated){//If first frame panel is opened or clicked to add modifier button update the layerpanel 
            
            if(!firstFrameActivated) //If clicked to add modifier button
                material.materialModifiers.push_back(appMaterialModifiers.textureModifier); //Add the texture modifier to the modifiers of the material

            //Creates layer panel elements from scratch using material.materialModifiers
            updateLayerPanel(material,textureRes,box,context);
        }

        //Check layerpanel if any modifier is clicked & change selectedMaterialModifierIndex if clicked
        checkLayerPanel(material);
        

        //Make sure selectedMaterialModifierIndex won't cause any vector out of range error
        if(selectedMaterialModifierIndex >= material.materialModifiers.size()){
            selectedMaterialModifierIndex = material.materialModifiers.size()-1;
            if(selectedMaterialModifierIndex < 0)
                selectedMaterialModifierIndex = 0;
        }
        

        if(modifiersPanel.sections.size() && material.materialModifiers.size()){
            if(material.materialModifiers[selectedMaterialModifierIndex].modifierIndex == 0) {//If is a texture modifier
                for (size_t i = 0; i < modifiersPanel.sections[0].elements.size(); i++)
                {
                    if(modifiersPanel.sections[0].elements[i].button.clickedMixVal){ //If clicked to any channel button in the texture modifier's panel
                        //Show the texture selection dialog
                        textureModifierTextureSelectingButtonIndex = i; //Give texture to that button in that index
                        textureSelectionDialog.active = true;
                    }
                }
            }
        }

        //If the texture selection dialog is active and indexing number indicates a channel button 
        if(textureSelectionDialog.active && textureModifierTextureSelectingButtonIndex != 1000){
            if(textureSelectionDialog.selectedTextureIndex != 1000){// If a texture is selected in the texture selection dialog
                //Change the texture of the modifier's PANEL button
                modifiersPanel.sections[0].elements[textureModifierTextureSelectingButtonIndex].button.texture = library.textures[textureSelectionDialog.selectedTextureIndex];
                //Change the texture of the source modifier's texture 
                material.materialModifiers[selectedMaterialModifierIndex].sections[0].elements[textureModifierTextureSelectingButtonIndex].button.texture = library.textures[textureSelectionDialog.selectedTextureIndex];
                
                //modifiersPanel displays sections of the material modifier

                //Return to default after a texture is selected
                textureModifierTextureSelectingButtonIndex = 1000;
                textureSelectionDialog.selectedTextureIndex = 1000;
                textureSelectionDialog.active = false;
                updateMaterial(material,(float)textureRes,box,context);
            }
        }

        materialDisplayer.render(videoScale,mouse,timer,textRenderer,!textureSelectionDialog.active);
        firstFrameActivated = false;
    }



    void activate(){
        selectedMaterialModifierIndex = 0;
        active = true;
        firstFrameActivated = true;
    }
    void deactivate(TextureSelectionDialog &textureSelectionDialog){
        selectedMaterialModifierIndex = 0;
        active = false;
        firstFrameActivated = false;
        textureSelectionDialog.active = false;
    }

    

private:
    void updateLayerPanel(Material &material,int &textureRes,Box &box,Context &context){
        layerPanel.sections.clear(); //Clear the elements of the layerPanel
        Section layerPanelSection;
        layerPanelSection.header = Element(Button());
        //Push the elements one by one from the materialModifiers of the material
        for (size_t i = 0; i < material.materialModifiers.size(); i++)
        {
            layerPanelSection.elements.push_back(
                Element(Button(1,glm::vec2(2,1.5f),colorPalette,buttonShader,material.materialModifiers[i].title , Texture(), 0.f,true))
            );
        }
        updateMaterial(material,(float)textureRes,box,context);
        layerPanel.sections.push_back(layerPanelSection);
    }

    void checkLayerPanel(Material &material){
        //Update the selected material modifier index
        if(layerPanel.sections.size()){
            for (size_t i = 0; i < layerPanel.sections[0].elements.size(); i++)
            {
                if(layerPanel.sections[0].elements[i].button.clickState1){ //If a modifier button is clicked 
                    if(selectedMaterialModifierIndex != i){ //If the clicked button is not selected 
                        layerPanel.sections[0].elements[selectedMaterialModifierIndex].button.clickState1 = false; //Unselect the selected one
                        selectedMaterialModifierIndex = i; //Select the clicked button
                        if(material.materialModifiers.size()){
                            modifiersPanel.sections = material.materialModifiers[selectedMaterialModifierIndex].sections;
                        }
                        break; 
                    }
                }
            }
        }
    }

    void checkModifiersPanel(Material &material,float textureRes,Box box,Context context,Mouse &mouse){
        //Check on modifiers panel if it has to reset.
        //Check if interacted with it's elements

        if(firstFrameActivated){//Clear the modifiers panel as the panel starts
            //If you don't there will be unrelated modifier's elements will be displayed as you open the panel to edit another material
            modifiersPanel.sections.clear();
        }

        //Update the material if interacted with modifier's panel
        for (size_t secI = 0; secI < modifiersPanel.sections.size(); secI++)
        {
            for (size_t elementI = 0; elementI < modifiersPanel.sections[secI].elements.size(); elementI++)
            {
                if(modifiersPanel.sections[secI].elements[elementI].state == 0)
                    if(modifiersPanel.sections[secI].elements[elementI].button.clickedMixVal == 1.f)
                        updateMaterial(material,(float)textureRes,box,context);

                if(modifiersPanel.sections[secI].elements[elementI].state == 1)
                    if(modifiersPanel.sections[secI].elements[elementI].rangeBar.pointerPressed == true)
                        updateMaterial(material,(float)textureRes,box,context);

                if(modifiersPanel.sections[secI].elements[elementI].state == 2)
                    if(modifiersPanel.sections[secI].elements[elementI].checkBox.hover && mouse.LClick == true)
                        updateMaterial(material,(float)textureRes,box,context);
            }
        }
    }

    void updateMaterial(Material &material,float textureRes,Box box,Context context){ //Updates textures of the material using modifier shaders
        //layout(location=0) out vec4 albedo;
        //layout(location=1) out vec4 roughness;
        //layout(location=2) out vec4 metallic; 
        //layout(location=3) out vec4 normalMap;
        //layout(location=4) out vec4 heightMap;
        //layout(location=5) out vec4 ambientOcclusion;

        glViewport(0,0,textureRes,textureRes);
        
        glm::mat4 projection = glm::ortho(0.f,(float)textureRes,(float)textureRes,0.f);
        
        //Take the texture in to the middle of the screen and cover it completely
        glm::vec2 fragScale = glm::vec2((float)textureRes/2.f,(float)textureRes/2.f);
        glm::vec3 fragPos = glm::vec3((float)textureRes/2.f,(float)textureRes/2.f,1.0f);

        std::cout << "MATERIAL UPDATED " << textureRes << std::endl;
        
        for (size_t i = 0; i < material.materialModifiers.size(); i++)
        {

            for (size_t channelI = 0; channelI < 6; channelI++) //For all the material channels
            {
                glDisable(GL_DEPTH_TEST);

                unsigned int FBO; //That framebuffer will be used to get the results of the shader (modifier)
                glGenFramebuffers(1,&FBO);
                glBindFramebuffer(GL_FRAMEBUFFER,FBO);
            
                
                unsigned int textureBuffer;

                if(channelI == 0){
                    textureBuffer = material.albedo.ID;
                    material.albedo.width = textureRes;
                    material.albedo.height = textureRes;
                }
                if(channelI == 1){
                    textureBuffer = material.roughness.ID;
                    material.roughness.width = textureRes;
                    material.roughness.height = textureRes;
                }
                if(channelI == 2){
                    textureBuffer = material.metallic.ID;
                    material.metallic.width = textureRes;
                    material.metallic.height = textureRes;
                }
                if(channelI == 3){
                    textureBuffer = material.normalMap.ID;
                    material.normalMap.width = textureRes;
                    material.normalMap.height = textureRes;
                }
                if(channelI == 4){
                    textureBuffer = material.heightMap.ID;
                    material.heightMap.width = textureRes;
                    material.heightMap.height = textureRes;
                }
                if(channelI == 5){
                    textureBuffer = material.ambientOcclusion.ID;
                    material.ambientOcclusion.width = textureRes;
                    material.ambientOcclusion.height = textureRes;
                }
                
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D,textureBuffer);
                
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
                
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureRes, textureRes, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                glGenerateMipmap(GL_TEXTURE_2D);

                material.materialModifiers[i].shader.use(); //Use the shader of the modifier
                material.materialModifiers[i].shader.setMat4("projection",projection); //Set the projection
                material.materialModifiers[i].shader.setVec2("scale",fragScale); //Set the scale
                material.materialModifiers[i].shader.setVec3("pos",fragPos); //Set the position
                material.materialModifiers[i].shader.setInt("theTexture",0); //Set the texture slot
                material.materialModifiers[i].shader.setInt("state",channelI); //Set the texture slot

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBuffer, 0);

                glClearColor(1,0,1,1);
                glClear(GL_COLOR_BUFFER_BIT);

                //Bind the texture (bind the channel textures if rendering a texture modifier & bind the result of the previous modifier)
                glActiveTexture(GL_TEXTURE0);

                if(material.materialModifiers[i].modifierIndex == 0) //Is texture modifier
                    glBindTexture(GL_TEXTURE_2D,material.materialModifiers[i].sections[0].elements[channelI].button.texture.ID);
                //else
                //  glBindTexture(GL_TEXTURE_2D,previousTexture);
                
                //box.bindBuffers();
                glDrawArrays(GL_TRIANGLES, 0, 6); //Draw the square

                glGenerateMipmap(GL_TEXTURE_2D);
                
                //Delete the framebuffer after completing the channel
                glDeleteFramebuffers(1,&FBO);

                glEnable(GL_DEPTH_TEST);
            }

        }
        
        //Update the material texture
        unsigned int FBO; //That framebuffer will be used to get the results of the shader (modifier)
        glGenFramebuffers(1,&FBO);
        glBindFramebuffer(GL_FRAMEBUFFER,FBO);
        
        glViewport(0,0,2048,2048);

        unsigned int RBO;
		glGenRenderbuffers(1,&RBO);
		glBindRenderbuffer(GL_RENDERBUFFER,RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 2048, 2048);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, material.displayingTexture, 0);

        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        tdModelShader.use();
        glm::mat4 view = glm::lookAt(glm::vec3(3.f,0,0), 
                                     glm::vec3(0), 
                                     glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.f), 
                                         1.f, 
                                         100.f, 
                                         0.1f);


        tdModelShader.setMat4("view",view);
        tdModelShader.setMat4("projection",projectionMatrix);
        
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D,material.albedo.ID);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D,material.roughness.ID);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D,material.metallic.ID);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D,material.normalMap.ID);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D,material.heightMap.ID);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D,material.ambientOcclusion.ID);
        
        sphereModel.Draw();
        
        glGenerateMipmap(GL_TEXTURE_2D);
        

        //Finish
        buttonShader.use();
        glBindFramebuffer(GL_FRAMEBUFFER,0);
        glDeleteFramebuffers(1,&FBO);

        glViewport(0,0,context.windowScale.x,context.windowScale.y);
    }
};

#endif