
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 (c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
 Official Web Page : https:ligidtools.com/ligidpainter

 ---------------------------------------------------------------------------
 
    TODO USE ALL THE PROJECT SETTINGS WHILE CREATING A PROJECT 

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

#include <string>
#include <iostream>
#include <vector>

NewProjectDialog::NewProjectDialog(){
    //Create the panel
    this->panel = Panel(  
                            {
                                Section(
                                    Button(),
                                    {
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(4,2),"New Project",Texture(),0.f,false))
                                    }
                                )
                            },
                            glm::vec2(22,25),
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
    

    this->tdModelsPanel = Panel(  
                            {
                                Section(
                                    Button(),
                                    {
                                    
                                    }
                                )
                            },
                            glm::vec2(20,13.f),
                            glm::vec3(50.f,53.f,0.8f),
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

    this->propertiesBtn = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(4,2), "Properties", Texture(), 0.f, false);
    this->propertiesBtn.outlineThickness = 0.f;
    this->propertiesBtn.color = glm::vec4(0.f);
    this->propertiesBtn.color2 = glm::vec4(0.f);
    
    this->tdModelsBtn = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(4,2), "3D Models", Texture(), 0.f, false);
    this->tdModelsBtn.outlineThickness = 0.f;
    this->tdModelsBtn.color = glm::vec4(0.f);
    this->tdModelsBtn.color2 = glm::vec4(0.f);
    
    this->titleTextboxText = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(4,2), "Title", Texture(), 0.f, false);
    this->titleTextboxText.outlineThickness = 0.f;
    this->titleTextboxText.color = glm::vec4(0.f);
    this->titleTextboxText.color2 = glm::vec4(0.f);
    
    this->pathTextboxText = Button(ELEMENT_STYLE_STYLIZED, glm::vec2(4,2), "Destination Path", Texture(), 0.f, false);
    this->pathTextboxText.outlineThickness = 0.f;
    this->pathTextboxText.color = glm::vec4(0.f);
    this->pathTextboxText.color2 = glm::vec4(0.f);

    this->activeSectionIndicator = Button(ELEMENT_STYLE_SOLID, glm::vec2(4, 0.1f), "",Texture(),2.f,false);
    this->activeSectionIndicator.outlineThickness = 0.0f;
    this->activeSectionIndicator.color = glm::vec4(ColorPalette::themeColor);
    
    this->sectionsBg = Button(ELEMENT_STYLE_SOLID, glm::vec2(4, 0.1f), "",Texture(),2.f,false);
    this->sectionsBg.outlineThickness = 0.0f;
    this->sectionsBg.color = glm::vec4(ColorPalette::secondColor);
    
    this->addTDModelBtn = Button(ELEMENT_STYLE_STYLIZED,glm::vec2(4,2),"Add Model",Texture(),2.f,false);
    this->addTDModelBtn.radius = 0.3f;
    
    this->deleteTDModelBtn = Button(ELEMENT_STYLE_STYLIZED,glm::vec2(4,2),"Remove selected",Texture(),2.f,false);
    this->deleteTDModelBtn.radius = 0.3f;
    
    this->createBtn = Button(ELEMENT_STYLE_STYLIZED,glm::vec2(4,2),"Create",Texture(),2.f,false);
    this->createBtn.radius = 0.3f;

    this->titleTextbox = TextBox(ELEMENT_STYLE_BASIC, glm::vec2(11,2), "MyProject", 2.f, false);
    this->pathTextbox = TextBox(ELEMENT_STYLE_BASIC, glm::vec2(11,2), UTIL::environmentSpecificAppDataFolderPath() + "LigidPainter" + UTIL::folderDistinguisher() + "Projects", 2.f, true);
    this->resolutionCombobox = ComboBox(ELEMENT_STYLE_BASIC,glm::vec2(10,2),
                                                                            {
                                                                                "256",
                                                                                "512",
                                                                                "1024",
                                                                                "2048",
                                                                                "4096"
                                                                            },"Texture Resolution",4.f);

    resolutionCombobox.selectedIndex = 2;
}

void NewProjectDialog::render(Timer timer, Project &project, bool &greetingDialogActive, bool &startScreen, LogDialog& logDialog){
    
    dialogControl.updateStart();

    //Render the panel
    panel.render(timer,dialogControl.isComplete());
    
    // Sections background
    sectionsBg.scale = panel.sections[0].elements[0].scale;
    sectionsBg.pos = panel.sections[0].elements[0].pos;
    sectionsBg.pos.y += panel.sections[0].elements[0].scale.y + sectionsBg.scale.y;
    sectionsBg.render(timer, false);

    // Properties section button
    propertiesBtn.pos = panel.sections[0].elements[0].pos;
    propertiesBtn.pos.x -= panel.sections[0].elements[0].scale.x - propertiesBtn.scale.x;
    propertiesBtn.pos.y += panel.sections[0].elements[0].scale.y + propertiesBtn.scale.y;
    if(propertiesBtn.hover)
        propertiesBtn.textScale = 0.55f;
    else
        propertiesBtn.textScale = 0.5f;
    propertiesBtn.render(timer, true);
    
    // 3D models section button
    tdModelsBtn.pos = propertiesBtn.pos; 
    tdModelsBtn.pos.x += propertiesBtn.scale.x + tdModelsBtn.scale.x;
    if(tdModelsBtn.hover)
        tdModelsBtn.textScale = 0.55f;
    else
        tdModelsBtn.textScale = 0.5f;
    tdModelsBtn.render(timer, true);

    // Active selection indicator button
    if(propertiesBtn.clicked)
        this->activeSection = 0;
    if(tdModelsBtn.clicked)
        this->activeSection = 1;
    timer.transition(this->activeSection, this->activeSectionIndicatorMixVal, 0.1f);
    this->activeSectionIndicator.pos = propertiesBtn.pos;
    this->activeSectionIndicator.pos.y += propertiesBtn.scale.y;
    this->activeSectionIndicator.pos.x += (sin(this->activeSectionIndicatorMixVal) * 1.2f) * (tdModelsBtn.pos.x - propertiesBtn.pos.x);
    this->activeSectionIndicator.render(timer, false);

    // Properties section elements
    if(this->activeSection == 0){
        this->titleTextboxText.pos = panel.sections[0].elements[0].pos;
        this->titleTextboxText.pos.y += 12.f;
        this->titleTextboxText.render(timer, true);

        this->pathTextboxText.pos = panel.sections[0].elements[0].pos;
        this->pathTextboxText.pos.y += 20.f;
        this->pathTextboxText.render(timer, true);

        titleTextbox.pos = panel.sections[0].elements[0].pos;
        titleTextbox.pos.y += 15.f;
        titleTextbox.render(timer, true);
        
        pathTextbox.pos = panel.sections[0].elements[0].pos;
        pathTextbox.pos.y += 23.f;
        pathTextbox.render(timer, true);
        
    }
    else{
        tdModelsPanel.render(timer, true);

        addTDModelBtn.pos = propertiesBtn.pos;
        addTDModelBtn.pos.y += propertiesBtn.scale.y + addTDModelBtn.scale.y * 1.65f;
        addTDModelBtn.pos.x += 2.f;
        addTDModelBtn.render(timer, true);
        
        deleteTDModelBtn.pos = addTDModelBtn.pos;
        deleteTDModelBtn.pos.x += addTDModelBtn.scale.x + deleteTDModelBtn.scale.x + 1.f;
        deleteTDModelBtn.render(timer, true);

        if(addTDModelBtn.clicked){

            std::string test = showFileSystemObjectSelectionDialog("Select a 3D model file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MODEL, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);
            
            if(test.size()){
                tdModelsPanel.sections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(4, 2.f), test, Texture(), 0.f, true));
            }
        }

        if(deleteTDModelBtn.clicked){
            for (size_t i = 0; i < tdModelsPanel.sections[0].elements.size(); i++)
            {
                if(tdModelsPanel.sections[0].elements[i].button.clickState1){
                    tdModelsPanel.sections[0].elements.erase(tdModelsPanel.sections[0].elements.begin() + i);
                    i--;
                }
            }                   
        }
    }


    // Create project button
    createBtn.pos = panel.pos;
    createBtn.pos.x += panel.scale.x - createBtn.scale.x*2.f;
    createBtn.pos.y += panel.scale.y - createBtn.scale.y*2.f;
    createBtn.render(timer, true);


    
    //If pressed to the last button of the panel (Create the project button)
    if(createBtn.clicked){
        
        std::vector<std::string> TDModelPaths;
        for (size_t i = 0; i < tdModelsPanel.sections[0].elements.size(); i++)
        {
            TDModelPaths.push_back(tdModelsPanel.sections[0].elements[i].button.text);
        }        

        //Create the project
        if(project.createProject(   
                                    pathTextbox.text, //Destination path
                                    titleTextbox.text, //Title of the project
                                    TDModelPaths  //3D model path
                                 ))
        {
            project.loadProject(project.ligidFilePath());

            startScreen = false;

            this->dialogControl.unActivate();
        }
    
    }
    
    //Close the dialog
    if(getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || (!this->panel.hover && !logDialog.isHovered() && *Mouse::LClick()) || panel.sections[0].elements[0].button.hover && *Mouse::LDoubleClick()){
        if(startScreen)
            greetingDialogActive = true;
        
        this->dialogControl.unActivate();
    }

    dialogControl.updateEnd(timer,0.15f);
}