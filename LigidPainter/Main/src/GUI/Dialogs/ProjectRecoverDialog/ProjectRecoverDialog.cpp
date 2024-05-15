
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

#include "3D/ThreeD.hpp" 

#include "GUI/GUI.hpp" 

#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Project/Project.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>


ProjectRecoverDialog::ProjectRecoverDialog(int){
    //Create the panel
    this->panel = Panel(
                            
                            {
                                Section(
                                    Element(Button()),
                                    {
                                    }
                                )
                            },
                            glm::vec2(40,40),
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

    lpIconBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(5.f), "", appTextures.ligidPainterIcon, 0.f, false);
    lpIconBtn.color = glm::vec4(0.);
    lpIconBtn.outlineColor = glm::vec4(0.);

    recoverTitleBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(10.f, 4.f), "Project Recovering Dialog", Texture(), 0.f, false);
    recoverTitleBtn.color = glm::vec4(0.);
    recoverTitleBtn.outlineColor = glm::vec4(0.);
    recoverTitleBtn.textScale = 1.2f;
    
    recoverInfoBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(20.f, 4.f), "Select a recovery slot to recover", Texture(), 0.f, false);
    recoverInfoBtn.color = glm::vec4(0.);
    recoverInfoBtn.outlineColor = glm::vec4(0.);
    recoverInfoBtn.textScale = 0.8f;

    recover1Btn = Button(ELEMENT_STYLE_BASIC, glm::vec2(10.f, 20.f), "Slot 1", Texture(), 0.f, false);
    recover2Btn = Button(ELEMENT_STYLE_BASIC, glm::vec2(10.f, 20.f), "Slot 2", Texture(), 0.f, false);
    recover3Btn = Button(ELEMENT_STYLE_BASIC, glm::vec2(10.f, 20.f), "Slot 3", Texture(), 0.f, false);

    projectPanelSelectBtn = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(6.f, 2.f), "Select", Texture(), 0.f, false);
    projectPanelFileExplorerBtn = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(8.f, 2.f), "Open In File Explorer", Texture(), 0.f, false);
    projectPanelExitBtn = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(6.f, 2.f), "Exit", Texture(), 0.f, false);
    projectPanelExitBtn.color = glm::vec4(0.9f, 0.f, 0.f, 1.f);

    recover1Btn.textScale = 0.9f;
    recover2Btn.textScale = 0.9f;
    recover3Btn.textScale = 0.9f;

    projectPanel = Panel(
                            
                            {
                                Section(
                                    Element(Button()),
                                    {
                                    }
                                )
                            },
                            glm::vec2(30,30),
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
    projectPanel.color.a = 1.;

    glm::vec2 projectPnSc = glm::vec2(projectPanel.scale.x/4, projectPanel.scale.y/1.2f);

    project_texturesPanel = Panel(
                            
                            {
                                Section(
                                    Element(Button()),
                                    {
                                    }
                                )
                            },
                            projectPnSc,
                            glm::vec3(projectPanel.pos.x - projectPnSc.x * 2.f - 2.f, projectPanel.pos.y - projectPanel.scale.y + projectPnSc.y + 1.f, 0.8f),
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
    project_materialsPanel = Panel(
                            
                            {
                                Section(
                                    Element(Button()),
                                    {
                                    }
                                )
                            },
                            projectPnSc,
                            glm::vec3(projectPanel.pos.x, projectPanel.pos.y - projectPanel.scale.y + projectPnSc.y + 1.f, 0.8f),
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
    project_brushesPanel = Panel(
                            
                            {
                                Section(
                                    Element(Button()),
                                    {
                                    }
                                )
                            },
                            projectPnSc,
                            glm::vec3(projectPanel.pos.x + projectPnSc.x * 2.f + 2.f, projectPanel.pos.y - projectPanel.scale.y + projectPnSc.y + 1.f, 0.8f),
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
}

static int slot = 1;

void ProjectRecoverDialog::show(Timer& timer){
    
    this->dialogControl.activate();

    while (!getContext()->window.shouldClose())
    {
        dialogControl.updateStart(true);

        project_discard_update_flag = true;

        // Render the background panel
        this->panel.render(timer, false);
        // Render the ligidpainter icon
        this->lpIconBtn.pos = this->panel.pos;
        this->lpIconBtn.pos.y -= this->panel.scale.y - this->lpIconBtn.scale.y * 1.5f;

        this->recoverTitleBtn.pos = this->lpIconBtn.pos;
        this->recoverTitleBtn.pos.y += 10.f;
        
        this->recoverInfoBtn.pos = this->recoverTitleBtn.pos;
        this->recoverInfoBtn.pos.y += 4.f;

        this->recover1Btn.pos = this->panel.pos;
        this->recover1Btn.pos.y += 10.f;
        this->recover1Btn.pos.x -= 25.f;

        this->recover2Btn.pos = this->recover1Btn.pos;
        this->recover2Btn.pos.x += 25.f;
        
        this->recover3Btn.pos = this->recover2Btn.pos;
        this->recover3Btn.pos.x += 25.f;

        recover1Btn.locked = !std::filesystem::exists(project_recover_path(1));
        recover2Btn.locked = !std::filesystem::exists(project_recover_path(2));
        recover3Btn.locked = !std::filesystem::exists(project_recover_path(3));

        recover1Btn.render(timer, !projectSelectionMode);
        recover2Btn.render(timer, !projectSelectionMode);
        recover3Btn.render(timer, !projectSelectionMode);

        this->lpIconBtn.render(timer, false);
        this->recoverTitleBtn.render(timer, false);
        this->recoverInfoBtn.render(timer, false);

        glClear(GL_DEPTH_BUFFER_BIT);

        if(recover1Btn.clicked || recover2Btn.clicked || recover3Btn.clicked){
            projectSelectionMode = true;

            slot = 1;

            if(recover2Btn.clicked)
                slot = 2;
            
            else if(recover3Btn.clicked)
                slot = 3;

            project_texturesPanel.sections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(10.f, 2.f), "Textures", Texture(), 0.f, false));
            project_brushesPanel.sections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(10.f, 2.f), "Brushes", Texture(), 0.f, false));
            project_materialsPanel.sections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(10.f, 2.f), "Materials", Texture(), 0.f, false));

            try
            {
                if(std::filesystem::exists(project_recover_path(slot) + UTIL::folderDistinguisher() + "Textures")){
                    for (const auto& entry : std::filesystem::directory_iterator(project_recover_path(slot) + UTIL::folderDistinguisher() + "Textures")) {
                        std::string path = entry.path().string();

                        if(std::filesystem::is_regular_file(path)){
                            Texture txtr;
                            txtr.load(path.c_str());

                            std::string title = UTIL::removeExtension(UTIL::getLastWordBySeparatingWithChar(path, UTIL::folderDistinguisher()));

                            project_texturesPanel.sections[0].elements.push_back(Button(ELEMENT_STYLE_BASIC, glm::vec2(10.f, 3.f), title, txtr, 0.f, false));
                            project_texturesPanel.sections[0].elements[project_texturesPanel.sections[0].elements.size() - 1].button.textureSizeScale = 0.98f;
                        }
                    }            
                }
                if(std::filesystem::exists(project_recover_path(slot) + UTIL::folderDistinguisher() + "Brushes")){
                    for (const auto& entry : std::filesystem::directory_iterator(project_recover_path(slot) + UTIL::folderDistinguisher() + "Brushes")) {
                        std::string path = entry.path().string();

                        if(std::filesystem::is_regular_file(path)){
                            std::string title = UTIL::removeExtension(UTIL::getLastWordBySeparatingWithChar(path, UTIL::folderDistinguisher()));
                            project_brushesPanel.sections[0].elements.push_back(Button(ELEMENT_STYLE_BASIC, glm::vec2(10.f, 2.f), title, Texture(), 0.f, false));
                        }
                    }            
                }
                if(std::filesystem::exists(project_recover_path(slot) + UTIL::folderDistinguisher() + "Materials")){
                    for (const auto& entry : std::filesystem::directory_iterator(project_recover_path(slot) + UTIL::folderDistinguisher() + "Materials")) {
                        std::string path = entry.path().string();

                        if(std::filesystem::is_regular_file(path)){
                            Material mat = Material("", {});
                            FileHandler::readLGDMATERIALFile(path, mat);

                            std::string title = UTIL::removeExtension(UTIL::getLastWordBySeparatingWithChar(path, UTIL::folderDistinguisher()));
                            project_materialsPanel.sections[0].elements.push_back(Button(ELEMENT_STYLE_BASIC, glm::vec2(10.f, 3.f), title, mat.displayingTexture, 0.f, false));
                            project_materialsPanel.sections[0].elements[project_materialsPanel.sections[0].elements.size() - 1].button.textureSizeScale = 0.98f;
                        
                            for (size_t modI = 0; modI < mat.materialModifiers.size(); modI++)
                            {
                                for (size_t secI = 0; secI < mat.materialModifiers[modI].sections.size(); secI++)
                                {
                                    for (size_t elI = 0; elI < mat.materialModifiers[modI].sections[secI].elements.size(); elI++)
                                    {
                                        glDeleteTextures(1, &mat.materialModifiers[modI].sections[secI].elements[elI].button.texture.ID);
                                    }
                                }
                            }
                        }
                    }            
                }
            }
            catch (const std::filesystem::filesystem_error& ex) {
                LGDLOG::start << "ERROR : Filesystem : Location ID 784689 " << ex.what() << LGDLOG::end;
            }
        }
            
        if(projectSelectionMode){
            projectPanel.render(timer, false);
            project_texturesPanel.render(timer, false);
            project_materialsPanel.render(timer, false);
            project_brushesPanel.render(timer, false);

            projectPanelSelectBtn.pos = projectPanel.pos;     
            projectPanelSelectBtn.pos.y += projectPanel.scale.y - projectPanelSelectBtn.scale.y - 1.f;     
            projectPanelSelectBtn.pos.x += projectPanel.scale.x - projectPanelSelectBtn.scale.x - 1.f;     
            
            projectPanelFileExplorerBtn.pos = projectPanelSelectBtn.pos;     
            projectPanelFileExplorerBtn.pos.x -= projectPanelSelectBtn.scale.x + projectPanelFileExplorerBtn.scale.x + 1.f;     
            
            projectPanelExitBtn.pos = projectPanel.pos;     
            projectPanelExitBtn.pos.y += projectPanel.scale.y - projectPanelExitBtn.scale.y - 1.f;     
            projectPanelExitBtn.pos.x -= projectPanel.scale.x - projectPanelExitBtn.scale.x - 1.f;     
            
            projectPanelExitBtn.render(timer, true);
            projectPanelFileExplorerBtn.render(timer, true);
            projectPanelSelectBtn.render(timer, true);
        }

        if(this->projectPanelFileExplorerBtn.clicked){
            UTIL::openInFileExplorer(std::filesystem::absolute(project_recover_path(slot)).string().c_str());
        }

        //End the dialog
        if(((getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE)) || (!this->panel.hover && *Mouse::LClick()) && !projectSelectionMode) && !this->dialogControl.firstFrameActivated){
            dialogControl.unActivate();
        }
        
        if((((getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) ) || (!this->projectPanel.hover && *Mouse::LClick()) || projectPanelExitBtn.clicked || projectPanelSelectBtn.clicked) && projectSelectionMode) && !this->dialogControl.firstFrameActivated){
            projectSelectionMode = false;

            for (size_t i = 0; i < project_texturesPanel.sections[0].elements.size(); i++)
            {
                glDeleteTextures(1, &project_texturesPanel.sections[0].elements[i].button.texture.ID);
            }
            for (size_t i = 0; i < project_materialsPanel.sections[0].elements.size(); i++)
            {
                glDeleteTextures(1, &project_materialsPanel.sections[0].elements[i].button.texture.ID);
            }
            /*
            for (size_t i = 0; i < project_brushesPanel.sections[0].elements.size(); i++)
            {
                glDeleteTextures(1, &project_brushesPanel.sections[0].elements[i].button.texture.ID);
            }
            */

            project_texturesPanel.sections[0].elements.clear();
            project_brushesPanel.sections[0].elements.clear();
            project_materialsPanel.sections[0].elements.clear();
        
            if(projectPanelSelectBtn.clicked){
                if(showMessageBox("WARNING!", "Are you sure you want to recover this project", MESSAGEBOX_TYPE_WARNING, MESSAGEBOX_BUTTON_OKCANCEL)){
                    dialogControl.unActivate();
                    
                    std::string lgdPath = project_locate_ligid_file(project_recover_path(slot)); 
                    if(lgdPath.size()){
                        project_load_library_elements(project_recover_path(slot), lgdPath);
                    }
                    else{
                        LGDLOG::start << "WARNING! No ligid file detected. Only the library elements will be updated." << LGDLOG::end;
                        project_load_library_elements(project_recover_path(slot), "");
                    }
                }
            }
        }

        dialogControl.updateEnd(timer,0.15f);
        if(dialogControl.mixVal == 0.f)
            break;
    }

    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();  

    project_discard_update_flag = false;
}