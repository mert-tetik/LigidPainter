
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

#include "3D/ThreeD.hpp" 
#include "GUI/GUI.hpp" 
#include "NodeSystem/Node/Node.hpp" 
#include "MouseSystem/Mouse.hpp"
#include "SettingsSystem/Settings.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

ExportDialog::ExportDialog(){
    
    //Create the panel
    this->panel = Panel(
                            
                            {
                                Section(
                                    Element(Button()),
                                    {
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(4,2),"Export",Texture(),0.f,false)), 
                                        
                                        //Project settings
                                        Element(TextBox(0,glm::vec2(4,2),"Select A Path",2.f,true)),
                                        
                                        Element(ComboBox(ELEMENT_STYLE_BASIC,glm::vec2(4,2),
                                        {
                                            "256",
                                            "512",
                                            "1024",
                                            "2048",
                                            "4096"
                                        },"Texture Resolution",4.f)),
                                        
                                        Element(ComboBox(ELEMENT_STYLE_BASIC,glm::vec2(4,2),
                                        {
                                            "PNG", 
                                            "JPEG", 
                                            "BMP", 
                                            "TGA"
                                        },"File Format",4.f)),

                                        Element(Button(ELEMENT_STYLE_STYLIZED,glm::vec2(4,2),"Export",Texture(),5.f,false))
                                    }
                                )
                            },
                            glm::vec2(15,30),
                            glm::vec3(50.f,50.f,0.8f),
                            ColorPalette::mainColor,
                            ColorPalette::thirdColor,
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

    this->panel.sections[0].elements[0].button.color = ColorPalette::secondColor;
    this->panel.sections[0].elements[0].button.color2 = ColorPalette::thirdColor;
    this->panel.sections[0].elements[0].button.outlineColor = ColorPalette::thirdColor;
    this->panel.sections[0].elements[0].button.outlineColor2 = ColorPalette::thirdColor;
}

void ExportDialog::render(Timer timer,
                          Project &project,bool &greetingDialogActive,
                          MaterialEditorDialog &materialEditorDialog){
    
    dialogControl.updateStart();

    //Render the panel
    panel.render(timer,true);
    
    //If pressed to the last button of the panel (Export button)
    if(panel.sections[0].elements[panel.sections[0].elements.size()-1].button.hover && *Mouse::LClick()){


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
        
        NodeScene::updateNodeResults(resolution, -1);
        
        //Update all the materials connected to the mesh output & export it's textures
        for (size_t i = 0; i < getModel()->meshes.size(); i++)
        {

            std::string materialFolderPath = destPath + UTIL::folderDistinguisher() + getModel()->meshes[i].materialName;
            
            try
            {
                std::vector<std::string> filesInTheFolder;
                for (const auto& entry : std::filesystem::directory_iterator(destPath)) {
                    filesInTheFolder.push_back(entry.path().string());
                }            
                
                UTIL::uniqueName(materialFolderPath, filesInTheFolder);

                if(!std::filesystem::create_directories(materialFolderPath))
                    LGDLOG::start << "ERROR : Exporting. Can't create folder in the location : " << materialFolderPath << LGDLOG::end;
            }
            catch (const std::filesystem::filesystem_error& ex) {
                LGDLOG::start << "ERROR : Filesystem : Location ID 772611 " << ex.what() << LGDLOG::end;
            }
            
            //For all the channels
            for (size_t channelI = 0; channelI < 6; channelI++)
            {
                Texture channelTxtr;
                
                if(channelI == 0){
                    channelTxtr = getModel()->meshes[i].albedo;
                    channelTxtr.title = "albedo";
                }
                if(channelI == 1){
                    channelTxtr = getModel()->meshes[i].roughness;
                    channelTxtr.title = "roughness";
                }
                if(channelI == 2){
                    channelTxtr = getModel()->meshes[i].metallic;
                    channelTxtr.title = "metallic";
                }
                if(channelI == 3){
                    channelTxtr = getModel()->meshes[i].normalMap;
                    channelTxtr.title = "normalMap";
                }
                if(channelI == 4){
                    channelTxtr = getModel()->meshes[i].heightMap;
                    channelTxtr.title = "heightMap";
                }
                if(channelI == 5){
                    channelTxtr = getModel()->meshes[i].ambientOcclusion;
                    channelTxtr.title = "ambientOcclusion";
                }

                channelTxtr.exportTexture(materialFolderPath, panel.sections[0].elements[3].comboBox.texts[panel.sections[0].elements[3].comboBox.selectedIndex]);
            }
        }
    }
    
    //Close the dialog
    if(getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || (!panel.hover && *Mouse::LClick()) || (panel.sections[0].elements[0].button.hover && *Mouse::LDoubleClick())){
        if(!dialogControl.firstFrameActivated)
            this->dialogControl.unActivate();
    }

    dialogControl.updateEnd(timer,0.15f);
}