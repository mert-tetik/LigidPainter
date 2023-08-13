
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
 
#include <string>
#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>

LoadProjectDialog::LoadProjectDialog(){}

LoadProjectDialog::LoadProjectDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,AppTextures appTextures, AppMaterialModifiers& appMaterialModifiers){
    
    //Take the parameters to the class member variables 
    this->appTextures = appTextures;
    this->appMaterialModifiers = appMaterialModifiers;

    //Create the bg panel
    this->bgPanel = Panel(
                            colorPalette,
                            {
                                Section(
                                    Element(Button()),
                                    {
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(4,2),colorPalette,"Load Project",Texture(),0.f,false)), 
                                    }
                                )
                            },
                            glm::vec2(30,30),
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

    //Create the projects panel                        
    this->projectsPanel = Panel(
                            
                            colorPalette,
                            {
                            },
                            glm::vec2(20,25),
                            glm::vec3(60.f,50.f,0.8f),
                            colorPalette.secondColor,
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
                            true
                        );

    //Panel's navigation (kinda) bar
    this->bgPanel.sections[0].elements[0].button.color = colorPalette.secondColor;
    this->bgPanel.sections[0].elements[0].button.color2 = colorPalette.thirdColor;
    this->bgPanel.sections[0].elements[0].button.outlineColor = colorPalette.thirdColor;
    this->bgPanel.sections[0].elements[0].button.outlineColor2 = colorPalette.thirdColor;
    
    //The load a project button
    this->loadButton = Button(ELEMENT_STYLE_STYLIZED,glm::vec2(4,2),colorPalette,"Load",Texture(),0.f,false);
    this->loadButton.pos.x = 30;
    this->loadButton.pos.y = 35;
    this->loadButton.pos.z = 0.9f;
    this->textBtn1 = Button(ELEMENT_STYLE_STYLIZED,glm::vec2(8,2),colorPalette,"Load a *.ligid file",Texture(),0.f,false);
    this->textBtn1.color = glm::vec4(0);
    this->textBtn1.pos.x = 30;
    this->textBtn1.pos.y = 30;
    this->textBtn1.pos.z = 0.9f;
    this->textBtn1.textScale = 1.0f;
    
    //Texts

    this->textBtn2 = Button(ELEMENT_STYLE_STYLIZED,glm::vec2(8,2),colorPalette,"The *.ligid file is a binary file located",Texture(),0.f,false);
    this->textBtn2.color = glm::vec4(0);
    this->textBtn2.pos.x = 30;
    this->textBtn2.pos.y = 45;
    this->textBtn2.pos.z = 0.9f;
    this->textBtn2.textScale = 0.7f;
    
    this->textBtn3 = Button(ELEMENT_STYLE_STYLIZED,glm::vec2(8,2),colorPalette,"in a project folder capable of contain project data.",Texture(),0.f,false);
    this->textBtn3.color = glm::vec4(0);
    this->textBtn3.pos.x = 30;
    this->textBtn3.pos.y = 48;
    this->textBtn3.pos.z = 0.9f;
    this->textBtn3.textScale = 0.7f;
    
    this->textBtn4 = Button(ELEMENT_STYLE_STYLIZED,glm::vec2(8,2),colorPalette,"Please create a project folder if you don't have access to one",Texture(),0.f,false);
    this->textBtn4.color = glm::vec4(0);
    this->textBtn4.pos.x = 30;
    this->textBtn4.pos.y = 54;
    this->textBtn4.pos.z = 0.9f;
    this->textBtn4.textScale = 0.5f;
}

void LoadProjectDialog::render(LigidWindow originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,
                                glm::vec2 videoScale,Project &project,bool &greetingDialogActive,bool &startScreen,
                                Model &model,int &textureRes,std::vector<Node> &meshNodeScene ){
    
    dialogControl.updateStart();

    //Render panels
    bgPanel.render(videoScale,mouse,timer,textRenderer,dialogControl.isComplete());
    loadButton.render(videoScale,mouse,timer,textRenderer,dialogControl.isComplete());
    
    //Render texts
    textBtn1.render(videoScale,mouse,timer,textRenderer,false);
    textBtn2.render(videoScale,mouse,timer,textRenderer,false);
    textBtn3.render(videoScale,mouse,timer,textRenderer,false);
    textBtn4.render(videoScale,mouse,timer,textRenderer,false);
    

    if(loadButton.hover && mouse.LClick){
        //Select a project file inside of a project folder
        std::string test = showFileSystemObjectSelectionDialog("Select a ligid file.", "", FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_LIGID, false, FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE);

        //If a file is selected        
        if(test.size()){
            
            //Load the project
            if(project.loadProject(test,model,appTextures,colorPalette,textureRes,meshNodeScene, videoScale, appMaterialModifiers)){
                
                startScreen = false;
                
                this->dialogControl.unActivate();
            
            }
            else{
                showMessageBox(
                            "Warning!", 
                            "Error while reading the *.ligid file! Detailed error message is printed to the terminal.", 
                            MESSAGEBOX_TYPE_WARNING, 
                            MESSAGEBOX_BUTTON_OK
                        );
            }
        }
        
    }
    
    int counter = 0;

    //Create a new section to give the projects panel    
    Section projectSection;

    //Clear the elements of the projects panel (will be updated)
    projectsPanel.sections.clear();
    
    for (const auto& entry : std::filesystem::directory_iterator("./Projects")) {
        
        //Project folder path inside of the ./Projects directory
        std::string projectPath = entry.path().string();
        
        //Create the button for the project path
        Button btn = Button(ELEMENT_STYLE_BASIC,glm::vec2(4,2),colorPalette,projectPath,Texture(),0.f,false);
        
        //Scale the button in x axis
        btn.scale.x = projectsPanel.scale.x;
        
        //Get the ligid file path inside of the project folder 
        std::string ligidFilePath = project.locateLigidFileInFolder(projectPath);
        
        //If a ligid file is loacted
        if(ligidFilePath.size()){
            //Transfer the button to the new section
            projectSection.elements.push_back(btn);
            
            counter++;
        }
    }
    
    //Give the new section to the projects panel
    projectsPanel.sections.push_back(projectSection);
    
    //After refreshing the elements render the projects panel
    projectsPanel.render(videoScale,mouse,timer,textRenderer,dialogControl.isComplete());
    
    //Check all the projects element if one them is pressed
    for (size_t i = 0; i < projectsPanel.sections[0].elements.size(); i++)
    {
        //If pressed to the project button
        if(projectsPanel.sections[0].elements[i].button.hover && mouse.LClick){
            
            //Get the ligid file path using the button's text as a project folder path source
            std::string ligidFilePath = project.locateLigidFileInFolder(projectsPanel.sections[0].elements[i].button.text);
            
            //Load the project
            if(project.loadProject(ligidFilePath,model, appTextures, colorPalette, textureRes, meshNodeScene, videoScale, appMaterialModifiers)){
                
                startScreen = false;
                
                this->dialogControl.unActivate();
            
            }
            else{

                showMessageBox(
                                "Warning!", 
                                "Error while reading the *.ligid file! Detailed error message is printed to the terminal." , 
                                MESSAGEBOX_TYPE_WARNING, 
                                MESSAGEBOX_BUTTON_OK
                            );

            }
        }
    }
    
    //Close the dialog
    if(originalWindow.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || bgPanel.sections[0].elements[0].button.hover && mouse.LDoubleClick){
        
        if(startScreen)
            greetingDialogActive = true;
        
        this->dialogControl.unActivate();
    
    }

    dialogControl.updateEnd(timer,0.15f);
}