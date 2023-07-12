
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 Copyright (c) 2022-2023, LigidTools 

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

#include "3D/ThreeD.hpp" 
#include "GUI/GUI.hpp" 

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>


ExportDialog::ExportDialog(){}

ExportDialog::ExportDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures){
    
    //Take the parameters to the class member variables 
    this->buttonShader = buttonShader;
    this->appTextures = appTextures;
    this->context = context;

    

    //Create the panel
    this->panel = Panel(
                            buttonShader,
                            colorPalette,
                            {
                                Section(
                                    Element(Button()),
                                    {
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(4,2),colorPalette,buttonShader,"Export",Texture(),0.f,false)), 
                                        
                                        //Project settings
                                        Element(TextBox(0,glm::vec2(4,2),colorPalette,buttonShader,"Select A Path",2.f,true),context.window),
                                        
                                        Element(ComboBox(ELEMENT_STYLE_BASIC,glm::vec2(4,2),colorPalette,buttonShader,
                                        {
                                            "256",
                                            "512",
                                            "1024",
                                            "2048",
                                            "4096"
                                        },"Texture Resolution",4.f),context.window),
                                        
                                        Element(ComboBox(ELEMENT_STYLE_BASIC,glm::vec2(4,2),colorPalette,buttonShader,
                                        {
                                            "PNG", 
                                            "JPEG", 
                                            "BMP", 
                                            "TGA"
                                        },"File Format",4.f),context.window),

                                        Element(Button(ELEMENT_STYLE_STYLIZED,glm::vec2(4,2),colorPalette,buttonShader,"Export",Texture(),5.f,false))
                                    }
                                )
                            },
                            glm::vec2(15,30),
                            glm::vec3(50.f,50.f,0.8f),
                            colorPalette.mainColor,
                            colorPalette.thirdColor,
                            true,
                            true,
                            true,
                            true,
                            true,
                            1.f,
                            1,
                            {},
                            0.25f,
                            false
                        );

    this->panel.sections[0].elements[0].button.color = colorPalette.secondColor;
    this->panel.sections[0].elements[0].button.color2 = colorPalette.thirdColor;
    this->panel.sections[0].elements[0].button.outlineColor = colorPalette.thirdColor;
    this->panel.sections[0].elements[0].button.outlineColor2 = colorPalette.thirdColor;
}

void ExportDialog::render(LigidWindow originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,
                          glm::vec2 videoScale,Project &project,bool &greetingDialogActive,Library &library,Shaders shaders,
                          Model &model,MaterialEditorDialog &materialEditorDialog,std::vector<Node> &meshNodeScene,Model sphereModel){
    
    dialogControl.updateStart(buttonShader);

    //Render the panel
    panel.render(videoScale,mouse,timer,textRenderer,true);
    
    //If pressed to the last button of the panel (Export button)
    if(panel.sections[0].elements[panel.sections[0].elements.size()-1].button.hover && mouse.LClick){

        
        ;

        int resolution = std::stoi(panel.sections[0].elements[2].comboBox.texts[panel.sections[0].elements[2].comboBox.selectedIndex]);
        
        std::string destPath = panel.sections[0].elements[1].textBox.text;

        if(!std::filesystem::exists(destPath)){

            showMessageBox(
                            "Warning!", 
                            "Error! Invalid exporting path.", 
                            MESSAGEBOX_TYPE_WARNING, 
                            MESSAGEBOX_BUTTON_OK
                        );

            return;
        }
        //All the materials connected to the mesh output
        std::vector<Material> materials = UTIL::getTheMaterialsConnectedToTheMeshNode(meshNodeScene,library,resolution,appTextures);
        
        //Update all the materials connected to the mesh output & export it's textures
        for (size_t i = 0; i < materials.size(); i++)
        {
            //Update the material
            materials[i].updateMaterial(resolution, box, context, shaders, sphereModel);


            if(i >= model.meshes.size())
                break;
            
            std::string materialFolderPath = destPath + UTIL::folderDistinguisher() + model.meshes[i].materialName;
            
            std::filesystem::create_directories(materialFolderPath);

            //For all the channels
            for (size_t channelI = 0; channelI < 6; channelI++)
            {
                Texture channelTxtr;
                
                if(channelI == 0){
                    channelTxtr = materials[i].albedo;
                    channelTxtr.title = "albedo";
                }
                if(channelI == 1){
                    channelTxtr = materials[i].roughness;
                    channelTxtr.title = "roughness";
                }
                if(channelI == 2){
                    channelTxtr = materials[i].metallic;
                    channelTxtr.title = "metallic";
                }
                if(channelI == 3){
                    channelTxtr = materials[i].normalMap;
                    channelTxtr.title = "normalMap";
                }
                if(channelI == 4){
                    channelTxtr = materials[i].heightMap;
                    channelTxtr.title = "heightMap";
                }
                if(channelI == 5){
                    channelTxtr = materials[i].ambientOcclusion;
                    channelTxtr.title = "ambientOcclusion";
                }

                channelTxtr.exportTexture(materialFolderPath, panel.sections[0].elements[3].comboBox.texts[panel.sections[0].elements[3].comboBox.selectedIndex]);
            }
        }
    }
    
    //Close the dialog
    if(originalWindow.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || (!panel.hover && mouse.LClick) || (panel.sections[0].elements[0].button.hover && mouse.LDoubleClick)){
        if(!dialogControl.firstFrameActivated)
            this->dialogControl.unActivate();
    }

    dialogControl.updateEnd(timer,buttonShader,0.15f);
}