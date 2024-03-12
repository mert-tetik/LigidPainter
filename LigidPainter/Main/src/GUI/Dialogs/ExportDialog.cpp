
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

#include "3D/ThreeD.hpp" 
#include "GUI/GUI.hpp" 
#include "MouseSystem/Mouse.hpp"
#include "SettingsSystem/Settings.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"
#include "LibrarySystem/Library.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

ExportDialog::ExportDialog(int){
    
    //Create the panel
    this->panel = Panel(
                            
                            {
                                Section(
                                    Element(Button()),
                                    {
                                    }
                                )
                            },
                            glm::vec2(20,25),
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

    //Create the panel
    this->subPanel = Panel(
                            
                            {
                                Section(
                                    Element(Button()),
                                    {
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(4,2),"Library Textures",Texture(),0.f, true)),
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(4,2),"Library Materials",Texture(),0.f, true))
                                    }
                                )
                            },
                            glm::vec2(panel.scale.x / 4.f, panel.scale.y),
                            glm::vec3(panel.pos.x - panel.scale.x + panel.scale.x / 4.f, panel.pos.y, 0.8f),
                            ColorPalette::secondColor,
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

    //Create the panel
    this->propertiesPanel = Panel(
                            
                            {
                                Section(

                                )
                            },
                            glm::vec2(panel.scale.x - subPanel.scale.x, panel.scale.y),
                            glm::vec3(subPanel.pos.x + subPanel.scale.x + (panel.scale.x - subPanel.scale.x), subPanel.pos.y, 0.8f),
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
    
    this->libraryTexturesSection = Section(
                                    Element(Button()),
                                    {
                                        //Project settings
                                        Element(TextBox(ELEMENT_STYLE_BASIC, glm::vec2(4,2), "Select A Path", 6.f, true)),
                                        
                                        Element(ComboBox(ELEMENT_STYLE_BASIC,glm::vec2(4,2),
                                        {
                                            "PNG", 
                                            "JPEG", 
                                            "BMP", 
                                            "TGA"
                                        },"File Format", 4.f)),

                                        CheckBox(ELEMENT_STYLE_BASIC, glm::vec2(4,2), "Mesh Textures Only", 4.f),

                                        Element(Button(ELEMENT_STYLE_STYLIZED,glm::vec2(4,2),"Export",Texture(),5.f,false))
                                    }
                                );

    this->libraryMaterialsSection = Section(
                                    Element(Button()),
                                    {
                                        //Project settings
                                        Element(TextBox(ELEMENT_STYLE_BASIC, glm::vec2(4,2), "Select A Path", 6.f, true)),

                                        Element(ComboBox(ELEMENT_STYLE_BASIC,glm::vec2(4,2),
                                        {
                                            "256",
                                            "512",
                                            "1024",
                                            "2048",
                                            "4096"
                                        },"Texture Resolution", 4.f)),
                                        
                                        Element(ComboBox(ELEMENT_STYLE_BASIC,glm::vec2(4,2),
                                        {
                                            "PNG", 
                                            "JPEG", 
                                            "BMP", 
                                            "TGA"
                                        },"Texture File Format", 4.f)),
                                        
                                        Element(ComboBox(ELEMENT_STYLE_BASIC,glm::vec2(4,2),
                                        {
                                            "LGDMATERIAL", 
                                        },"Material File Format", 4.f)),

                                        Element(Button(ELEMENT_STYLE_STYLIZED,glm::vec2(4,2),"Export",Texture(),5.f,false))
                                    }
                                );
    

    //this->panel.sections[0].elements[0].button.color = ColorPalette::secondColor;
    //this->panel.sections[0].elements[0].button.color2 = ColorPalette::thirdColor;
    //this->panel.sections[0].elements[0].button.outlineColor = ColorPalette::thirdColor;
    //this->panel.sections[0].elements[0].button.outlineColor2 = ColorPalette::thirdColor;
}

#define CREATE_DIR(destPath, folderName, res)std::string folderNameX = folderName; \
                                        std::vector<std::string> names; \
                                        for (const auto& entry : std::filesystem::directory_iterator(destPath)) { \
                                            names.push_back(UTIL::getLastWordBySeparatingWithChar(entry.path().filename().string(), UTIL::folderDistinguisher())); \
                                        } \
                                        UTIL::uniqueName(folderNameX, names); \
                                        res = destPath + UTIL::folderDistinguisher() + folderNameX; \
                                        if(!std::filesystem::create_directories(res)){ \
                                            LGDLOG::start << "ERROR : Exporting library materials. Can't create folder in the location : " << res << LGDLOG::end; \
                                            return; \
                                        } 

static void exportLibraryMaterials(Panel& propertiesPanel){
    
    int resolution = std::stoi(propertiesPanel.sections[0].elements[1].comboBox.texts[propertiesPanel.sections[0].elements[1].comboBox.selectedIndex]);
        
    std::string destPath = propertiesPanel.sections[0].elements[0].textBox.text;

    if(!std::filesystem::exists(destPath)){

        showMessageBox(
                        "Warning!", 
                        "Error! Invalid exporting path.", 
                        MESSAGEBOX_TYPE_WARNING, 
                        MESSAGEBOX_BUTTON_OK
                    );

        return;
    }
    
    //Update all the materials connected to the mesh output & export it's textures
    for (size_t i = 0; i < Library::getMaterialArraySize(); i++)
    {
        std::string materialFolderPath = "";
        
        try
        {
            CREATE_DIR(destPath, Library::getMaterialObj(i).title, materialFolderPath)
        }
        catch (const std::filesystem::filesystem_error& ex) {
            LGDLOG::start << "ERROR : Filesystem : Location ID 772611 " << ex.what() << LGDLOG::end;
        }

        if(materialFolderPath == ""){
            LGDLOG::start << "ERROR : Unable to generate a location for the material folder!" << LGDLOG::end;
            return;
        }

        Vertex vert = Vertex();

        Mesh matMesh = Mesh(
                                {   //                  Pos                          TexCoords          Normal          Tangent         Bitangent
                                    Vertex(glm::vec3(glm::vec2(0.f, 0.f), 0.f), glm::vec2(0.f, 0.f), glm::vec3(0.f), glm::vec3(0.f), glm::vec3(0.f)),
                                    Vertex(glm::vec3(glm::vec2(1.f, 0.f), 0.f), glm::vec2(1.f, 0.f), glm::vec3(0.f), glm::vec3(0.f), glm::vec3(0.f)),
                                    Vertex(glm::vec3(glm::vec2(1.f, 1.f), 0.f), glm::vec2(1.f, 1.f), glm::vec3(0.f), glm::vec3(0.f), glm::vec3(0.f)),
                                    Vertex(glm::vec3(glm::vec2(0.f, 1.f), 0.f), glm::vec2(0.f, 1.f), glm::vec3(0.f), glm::vec3(0.f), glm::vec3(0.f)),
                                },
                                {
                                    0, 1, 3,   // first triangle
                                    1, 2, 3    // second triangle
                                },
                                "Meshes",
                                true
                            );


        for (int modI = Library::getMaterialObj(i).materialModifiers.size() - 1; modI >= 0; --modI)    
        {
            Library::getMaterialObj(i).materialModifiers[modI].updateMaterialChannels(*Library::getMaterial(i), matMesh, resolution, modI, appTextures.white, 0, false, Model());
        }
        
        FileHandler::writeLGDMATERIALFile(materialFolderPath, Library::getMaterialObj(i));

        //For all the channels
        for (size_t channelI = 0; channelI < 6; channelI++)
        {
            Texture channelTxtr;
            
            if(channelI == 0){
                channelTxtr = matMesh.albedo;
                channelTxtr.title = "albedo";
            }
            if(channelI == 1){
                channelTxtr = matMesh.roughness;
                channelTxtr.title = "roughness";
            }
            if(channelI == 2){
                channelTxtr = matMesh.metallic;
                channelTxtr.title = "metallic";
            }
            if(channelI == 3){
                channelTxtr = matMesh.normalMap;
                channelTxtr.title = "normalMap";
            }
            if(channelI == 4){
                channelTxtr = matMesh.heightMap;
                channelTxtr.title = "heightMap";
            }
            if(channelI == 5){
                channelTxtr = matMesh.ambientOcclusion;
                channelTxtr.title = "ambientOcclusion";
            }

            channelTxtr.exportTexture(materialFolderPath, propertiesPanel.sections[0].elements[2].comboBox.texts[propertiesPanel.sections[0].elements[2].comboBox.selectedIndex]);
        }
    }
}

static void exportLibraryTextures(Panel& propertiesPanel){
    std::string destPath = propertiesPanel.sections[0].elements[0].textBox.text;

    if(!std::filesystem::exists(destPath)){

        showMessageBox(
                        "Warning!", 
                        "Error! Invalid exporting path.", 
                        MESSAGEBOX_TYPE_WARNING, 
                        MESSAGEBOX_BUTTON_OK
                    );

        return;
    }

    std::string texturesFolderPath;

    CREATE_DIR(destPath, "LigidPainter-Library_Textures", texturesFolderPath)

    if(texturesFolderPath == ""){
        LGDLOG::start << "ERROR : Unable to generate a location for the textures folder!" << LGDLOG::end;
        return;
    }

    bool markedOnly = propertiesPanel.sections[0].elements[2].checkBox.clickState1;
    
    for (size_t i = 0; i < Library::getTextureArraySize(); i++)
    {
        std::string FORMAT = propertiesPanel.sections[0].elements[1].comboBox.texts[propertiesPanel.sections[0].elements[1].comboBox.selectedIndex];
        
        bool meshMatch = false;

        for (size_t meshI = 0; meshI < getModel()->meshes.size(); meshI++)
        {
            if(
                    getModel()->meshes[meshI].albedo.ID == Library::getTextureObj(i).ID ||
                    getModel()->meshes[meshI].roughness.ID == Library::getTextureObj(i).ID ||
                    getModel()->meshes[meshI].metallic.ID == Library::getTextureObj(i).ID ||
                    getModel()->meshes[meshI].normalMap.ID == Library::getTextureObj(i).ID ||
                    getModel()->meshes[meshI].heightMap.ID == Library::getTextureObj(i).ID ||
                    getModel()->meshes[meshI].ambientOcclusion.ID == Library::getTextureObj(i).ID
                )
            {
                meshMatch = true;
            }
        }   
        
        if(!(markedOnly && !meshMatch))
            Library::getTextureObj(i).exportTexture(
                                                        texturesFolderPath, 
                                                        FORMAT
                                                    );
    }
}



void ExportDialog::render(Timer timer, Project &project, bool &greetingDialogActive){
    
    dialogControl.updateStart();

    if(this->activeSection == 0)
        propertiesPanel.sections[0] = libraryTexturesSection;
    if(this->activeSection == 1)
        propertiesPanel.sections[0] = libraryMaterialsSection;
    
    //Render the panels
    panel.render(timer,false);
    subPanel.render(timer,true);
    propertiesPanel.render(timer,true);

    if(this->activeSection == 0)
         libraryTexturesSection = propertiesPanel.sections[0];
    if(this->activeSection == 1)
         libraryMaterialsSection = propertiesPanel.sections[0];

    for (size_t i = 0; i < subPanel.sections[0].elements.size(); i++)
    {
        if(subPanel.sections[0].elements[i].button.clickState1 && this->activeSection != i){
            this->activeSection = i;
            for (size_t i = 0; i < subPanel.sections[0].elements.size(); i++){
                subPanel.sections[0].elements[i].button.clickState1 = false;
            }
        }
        if(this->activeSection == i){
            subPanel.sections[0].elements[i].button.clickState1 = true;
        }
        else{
            subPanel.sections[0].elements[i].button.clickState1 = false;
        }
    }
    
    //If pressed to the last button of the panel (Export button)
    if(propertiesPanel.sections[0].elements[propertiesPanel.sections[0].elements.size() - 1].button.clicked){

        while(true){
            if(!project.projectProcessing)
                break;
        }
        project.projectProcessing = true;

        if(this->activeSection == 0)
            exportLibraryTextures(this->propertiesPanel);
        if(this->activeSection == 1)
            exportLibraryMaterials(this->propertiesPanel);
        
        project.projectProcessing = false;
    }
    
    //Close the dialog (panel.sections[0].elements[0].button.hover && *Mouse::LDoubleClick())
    if(getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || (!panel.hover && !dialog_log.isHovered() && *Mouse::LClick())){
        if(!dialogControl.firstFrameActivated)
            this->dialogControl.unActivate();
    }

    dialogControl.updateEnd(timer,0.15f);
}