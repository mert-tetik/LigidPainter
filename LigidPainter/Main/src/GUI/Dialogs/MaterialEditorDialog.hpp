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
    ColorPalette colorPalette;
public:
    bool active = true; //Render the dialog
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

    int textureModifierTextureSelectingButtonIndex = 1000; //1000 if none of them is selecting

    AppMaterialModifiers appMaterialModifiers;

    std::vector<MaterialModifier> materialModifiers;

    int selectedMaterialModifierIndex = 0;

    MaterialEditorDialog(){}
    
    MaterialEditorDialog(Shader buttonShader,ColorPalette colorPalette,AppTextures appTextures){
        this->buttonShader = buttonShader; 
        this->colorPalette = colorPalette; 
        
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
            0
        );
        appMaterialModifiers.textureModifier.sections[0].header.button.clickState1 = true;

    }

    void render(glm::vec2 videoScale,Mouse& mouse,Timer &timer,TextRenderer &textRenderer,TextureSelectionDialog &textureSelectionDialog,Library &library){
        bgPanel.render(videoScale,mouse,timer,textRenderer);
        layerPanel.render(videoScale,mouse,timer,textRenderer);
        modifiersPanel.render(videoScale,mouse,timer,textRenderer);
        materialDisplayer.pos = bgPanel.pos;
        materialDisplayer.scale = glm::vec2(35.f); 
        materialDisplayer.scale.x = (modifiersPanel.pos.x - modifiersPanel.scale.x) - (layerPanel.pos.x + layerPanel.scale.x); 
        materialDisplayer.scale.x /= 2.f;
        materialDisplayer.pos.x = modifiersPanel.pos.x - modifiersPanel.scale.x - materialDisplayer.scale.x;



        //Update layer panal elements
        if(layerPanel.barButtons[0].clickedMixVal == 1.f){
            materialModifiers.push_back(appMaterialModifiers.textureModifier);

            layerPanel.sections.clear();
            Section layerPanelSection;
            layerPanelSection.header = Element(Button());
            for (size_t i = 0; i < materialModifiers.size(); i++)
            {
                layerPanelSection.elements.push_back(
                    Element(Button(1,glm::vec2(2,1.5f),colorPalette,buttonShader,materialModifiers[i].title , Texture(), 0.f,true))
                );
            }
            layerPanel.sections.push_back(layerPanelSection);
        }

        //Update the selected material modifier index
        if(layerPanel.sections.size()){
            for (size_t i = 0; i < layerPanel.sections[0].elements.size(); i++)
            {
                if(layerPanel.sections[0].elements[i].button.clickState1){ //If a modifier button is clicked 
                    if(selectedMaterialModifierIndex != i){ //If the clicked button is not selected 
                        layerPanel.sections[0].elements[selectedMaterialModifierIndex].button.clickState1 = false; //Unselect the selected one
                        selectedMaterialModifierIndex = i; //Select the clicked button
                        if(materialModifiers.size()){
                            modifiersPanel.sections = materialModifiers[selectedMaterialModifierIndex].sections;
                        }
                        break; 
                    }
                }
            }
        }

        if(modifiersPanel.sections.size()){
            if(materialModifiers[selectedMaterialModifierIndex].modifierIndex == 0) {//If is a texture modifier
                for (size_t i = 0; i < modifiersPanel.sections[0].elements.size(); i++)
                {
                    if(modifiersPanel.sections[0].elements[i].button.clickedMixVal){
                        textureModifierTextureSelectingButtonIndex = i;
                        textureSelectionDialog.active = true;
                    }
                }
            }
        }
        if(textureSelectionDialog.active && textureModifierTextureSelectingButtonIndex != 1000){
            if(textureSelectionDialog.selectedTextureIndex != 1000){
                modifiersPanel.sections[0].elements[textureModifierTextureSelectingButtonIndex].button.texture = library.textures[textureSelectionDialog.selectedTextureIndex];
                materialModifiers[selectedMaterialModifierIndex].sections[0].elements[textureModifierTextureSelectingButtonIndex].button.texture = library.textures[textureSelectionDialog.selectedTextureIndex];
                textureModifierTextureSelectingButtonIndex = 1000;
                textureSelectionDialog.selectedTextureIndex = 1000;
                textureSelectionDialog.active = false;
            }
        }


        materialDisplayer.render(videoScale,mouse,timer,textRenderer);
    }
};

#endif