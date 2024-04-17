
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

#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Project/Project.hpp"
#include "UTIL/Project/ProjectUTIL.hpp"

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
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(4,2),"Library Materials",Texture(),0.f, true)),
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(4,2),"Layers",Texture(),0.f, true))
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

    this->subPanel.elementSelectionMode = true;

    //Create the panel
    this->libraryTexturesPanel = Panel(
                                        {
                                            Section(
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

                                                    Element(Button(ELEMENT_STYLE_STYLIZED,glm::vec2(4,2),"Export",Texture(),5.f,false))
                                                }
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

    this->libraryMaterialsPanel = Panel(
                                        {
                                            Section(
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
    
    this->layersPanel = Panel(
                                {
                                    Section(
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
                                            },"Texture File Format", 4.f)),

                                            Element(Button(ELEMENT_STYLE_STYLIZED,glm::vec2(4,2),"Export",Texture(),5.f,false))
                                        }
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
    
}


static void exportLibraryMaterials(std::string destPath, int resolution, std::string txtrFormat, std::string materialFormat);
static void exportLibraryTextures(std::string destPath, std::string format);
static void exportLayers(std::string destPath, std::string format);

void ExportDialog::show(Timer& timer){
    
    dialogControl.activate();

    while(!getContext()->window.shouldClose()){

        dialogControl.updateStart();

        //Render the panels
        panel.render(timer,false);
        subPanel.render(timer,true);
        
        float total_hover_mix = subPanel.sections[0].elements[0].button.hoverMixVal + subPanel.sections[0].elements[1].button.hoverMixVal + subPanel.sections[0].elements[2].button.hoverMixVal;

        glm::vec2 panel_scale = glm::vec2(panel.scale.x - subPanel.scale.x, panel.scale.y);
        if(subPanel.sections[0].elements[0].button.clickedMixVal){
            ShaderSystem::buttonShader().setFloat("properties.groupOpacity", (subPanel.sections[0].elements[0].button.clickedMixVal - (total_hover_mix - subPanel.sections[0].elements[0].button.hoverMixVal) / 20.f));
            this->libraryTexturesPanel.scale = panel_scale * (subPanel.sections[0].elements[0].button.clickedMixVal - (total_hover_mix - subPanel.sections[0].elements[0].button.hoverMixVal) / 20.f) + 0.1f;
            this->libraryTexturesPanel.render(timer, true);
        }
        if(subPanel.sections[0].elements[1].button.clickedMixVal){
            ShaderSystem::buttonShader().setFloat("properties.groupOpacity", (subPanel.sections[0].elements[1].button.clickedMixVal - (total_hover_mix - subPanel.sections[0].elements[1].button.hoverMixVal) / 20.f));
            this->libraryMaterialsPanel.scale = panel_scale * (subPanel.sections[0].elements[1].button.clickedMixVal - (total_hover_mix - subPanel.sections[0].elements[1].button.hoverMixVal) / 20.f) + 0.1f;
            this->libraryMaterialsPanel.render(timer, true);
        }
        if(subPanel.sections[0].elements[2].button.clickedMixVal){
            ShaderSystem::buttonShader().setFloat("properties.groupOpacity", (subPanel.sections[0].elements[2].button.clickedMixVal - (total_hover_mix - subPanel.sections[0].elements[2].button.hoverMixVal) / 20.f));
            this->layersPanel.scale = panel_scale * (subPanel.sections[0].elements[2].button.clickedMixVal - (total_hover_mix - subPanel.sections[0].elements[2].button.hoverMixVal) / 20.f) + 0.1f;
            this->layersPanel.render(timer, true);
        }

        ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);

        if(subPanel.selectedElement == 0){
            if(this->libraryTexturesPanel.sections[0].elements[this->libraryTexturesPanel.sections[0].elements.size() - 1].button.clicked){
                while(true){
                    if(!projectUTIL_processing)
                        break;
                }
                projectUTIL_processing = true;
                exportLibraryTextures(
                                            this->libraryTexturesPanel.sections[0].elements[0].textBox.text,
                                            this->libraryTexturesPanel.sections[0].elements[1].comboBox.texts[this->libraryTexturesPanel.sections[0].elements[1].comboBox.selectedIndex]
                                        );
                projectUTIL_processing = false;
            }
        }
        if(subPanel.selectedElement == 1){
            if(this->libraryMaterialsPanel.sections[0].elements[this->libraryMaterialsPanel.sections[0].elements.size() - 1].button.clicked){
                while(true){
                    if(!projectUTIL_processing)
                        break;
                }
                projectUTIL_processing = true;
                exportLibraryMaterials(
                                            this->libraryMaterialsPanel.sections[0].elements[0].textBox.text,
                                            std::stoi(this->libraryMaterialsPanel.sections[0].elements[1].comboBox.texts[this->libraryMaterialsPanel.sections[0].elements[1].comboBox.selectedIndex]),
                                            this->libraryMaterialsPanel.sections[0].elements[2].comboBox.texts[this->libraryMaterialsPanel.sections[0].elements[2].comboBox.selectedIndex],
                                            this->libraryMaterialsPanel.sections[0].elements[3].comboBox.texts[this->libraryMaterialsPanel.sections[0].elements[3].comboBox.selectedIndex],
                                        );
                projectUTIL_processing = false;
            }
        }
        if(subPanel.selectedElement == 2){
            if(this->layersPanel.sections[0].elements[this->layersPanel.sections[0].elements.size() - 1].button.clicked){
                while(true){
                    if(!projectUTIL_processing)
                        break;
                }
                projectUTIL_processing = true;
                exportLayers(                                            
                                this->layersPanel.sections[0].elements[0].textBox.text,
                                this->layersPanel.sections[0].elements[1].comboBox.texts[this->libraryTexturesPanel.sections[0].elements[1].comboBox.selectedIndex]
                            );
                projectUTIL_processing = false;
            }
        }
        
        //Close the dialog (panel.sections[0].elements[0].button.hover && *Mouse::LDoubleClick())
        if(getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || (!panel.hover && !dialog_log.isHovered() && *Mouse::LClick())){
            if(!dialogControl.firstFrameActivated)
                this->dialogControl.unActivate();
        }

        dialogControl.updateEnd(timer,0.15f);
        if(dialogControl.mixVal == 0.f)
            break;
    }

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

static void exportLibraryMaterials(std::string destPath, int resolution, std::string txtrFormat, std::string materialFormat){
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

        Library::getMaterialObj(i).apply_material(Model(), matMesh, resolution, false);
        
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

            channelTxtr.exportTexture(materialFolderPath, txtrFormat);
        }
    }
}

static void exportLibraryTextures(std::string destPath, std::string format){
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

    for (size_t i = 0; i < Library::getTextureArraySize(); i++)
    {
        Library::getTextureObj(i).exportTexture(
                                                    texturesFolderPath, 
                                                    format
                                                );
    }
}

static void exportLayers(std::string destPath, std::string format){
    if(!std::filesystem::exists(destPath)){

        showMessageBox(
                        "Warning!", 
                        "Error! Invalid exporting path.", 
                        MESSAGEBOX_TYPE_WARNING, 
                        MESSAGEBOX_BUTTON_OK
                    );

        return;
    }

    std::string layersFolderPath;

    CREATE_DIR(destPath, "LigidPainter-Layer-Results", layersFolderPath)

    if(layersFolderPath == ""){
        LGDLOG::start << "ERROR : Unable to generate a location for the layers folder!" << LGDLOG::end;
        return;
    }

    for (Mesh& mesh : getScene()->model->meshes)
    {
        std::string texturesFolderPath;
        CREATE_DIR(layersFolderPath, mesh.materialName, texturesFolderPath)

        if(texturesFolderPath == ""){
            LGDLOG::start << "ERROR : Unable to generate a location for the textures folder!" << LGDLOG::end;
            return;
        }

        mesh.albedo.exportTexture(texturesFolderPath, format);
        mesh.roughness.exportTexture(texturesFolderPath, format);
        mesh.metallic.exportTexture(texturesFolderPath, format);
        mesh.normalMap.exportTexture(texturesFolderPath, format);
        mesh.heightMap.exportTexture(texturesFolderPath, format);
        mesh.ambientOcclusion.exportTexture(texturesFolderPath, format); 
    }

}