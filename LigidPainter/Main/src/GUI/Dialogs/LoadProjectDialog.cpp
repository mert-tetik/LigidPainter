
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
#include<GLFW/glfw3.h>

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

#include "tinyfiledialogs.h"

LoadProjectDialog::LoadProjectDialog(){}

LoadProjectDialog::LoadProjectDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures){
    
    //Take the parameters to the class member variables 
    this->buttonShader = buttonShader;
    this->appTextures = appTextures;

    //Create the bg panel
    this->bgPanel = Panel(
                            buttonShader,
                            colorPalette,
                            {
                                Section(
                                    Element(Button()),
                                    {
                                        Element(Button(0,glm::vec2(4,2),colorPalette,buttonShader,"Load Project",Texture(),0.f,false)), 
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
                            buttonShader,
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
    this->loadButton = Button(0,glm::vec2(4,2),colorPalette,buttonShader,"Load",Texture(),0.f,false);
    this->loadButton.pos.x = 30;
    this->loadButton.pos.y = 35;
    this->loadButton.pos.z = 0.9f;
    this->textBtn1 = Button(0,glm::vec2(8,2),colorPalette,buttonShader,"Load a *.ligid file",Texture(),0.f,false);
    this->textBtn1.color = glm::vec4(0);
    this->textBtn1.pos.x = 30;
    this->textBtn1.pos.y = 30;
    this->textBtn1.pos.z = 0.9f;
    this->textBtn1.textScale = 1.0f;
    
    //Texts

    this->textBtn2 = Button(0,glm::vec2(8,2),colorPalette,buttonShader,"The *.ligid file is a binary file located",Texture(),0.f,false);
    this->textBtn2.color = glm::vec4(0);
    this->textBtn2.pos.x = 30;
    this->textBtn2.pos.y = 45;
    this->textBtn2.pos.z = 0.9f;
    this->textBtn2.textScale = 0.7f;
    
    this->textBtn3 = Button(0,glm::vec2(8,2),colorPalette,buttonShader,"in a project folder capable of contain project data.",Texture(),0.f,false);
    this->textBtn3.color = glm::vec4(0);
    this->textBtn3.pos.x = 30;
    this->textBtn3.pos.y = 48;
    this->textBtn3.pos.z = 0.9f;
    this->textBtn3.textScale = 0.7f;
    
    this->textBtn4 = Button(0,glm::vec2(8,2),colorPalette,buttonShader,"Please create a project folder if you don't have access to a project folder",Texture(),0.f,false);
    this->textBtn4.color = glm::vec4(0);
    this->textBtn4.pos.x = 30;
    this->textBtn4.pos.y = 54;
    this->textBtn4.pos.z = 0.9f;
    this->textBtn4.textScale = 0.5f;
}

void LoadProjectDialog::render(GLFWwindow* originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,glm::vec2 videoScale,Project &project,bool &greetingDialogActive,Library &library,Shaders shaders,Model &model){
    
    dialogControl.updateStart(buttonShader);

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
        char const* lFilterPatterns[1] = { "*.ligid" };
        char* test = tinyfd_openFileDialog("Select a ligid file","",1, lFilterPatterns,"",false);

        //If a file is selected        
        if(test){
            
            //Load the project
            if(project.loadProject(test,library,shaders,model,appTextures,colorPalette))
                this->dialogControl.unActivate();
            else{
                const char* title = "Warning";
                const char* message = "Error while reading the *.ligid file! Detailed error message is printed to the terminal.";
                const char* icon = "warning";
                const char* button = "Ok";
                tinyfd_messageBox(title, message, button, icon, 1);
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
        Button btn = Button(2,glm::vec2(4,2),colorPalette,buttonShader,projectPath,Texture(),0.f,false);
        
        //Scale the button in x axis
        btn.scale.x = projectsPanel.scale.x;
        
        //Get the ligid file path inside of the project folder 
        std::string ligidFilePath = project.locateLigidFileInFolder(projectPath);
        
        //If a ligid file is loacted
        if(ligidFilePath.size()){
            //Date data stored in the ligid file
            time_t creationDate;
            time_t lastOpeningDate;
            
            //Get the ligid file data
            project.returnLigidFileData(ligidFilePath,creationDate,lastOpeningDate);
            
            //Date to string 
            std::string creationDateStr = (std::string)std::ctime(&creationDate);
            std::string lastOpeningDateStr = (std::string)std::ctime(&lastOpeningDate);
            
            //Remove suspecious '\n' character
            creationDateStr.pop_back(); 

            //TODO Do something with the date (maybe)

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
            if(project.loadProject(ligidFilePath,library,shaders,model,appTextures,colorPalette))
                this->dialogControl.unActivate();
            else{
                const char* title = "Warning";
                const char* message = "Error while reading the *.ligid file! Detailed error message is printed to the terminal.";
                const char* icon = "warning";
                const char* button = "Ok";
                tinyfd_messageBox(title, message, button, icon, 1);
            }
        }
    }
    
    //Close the dialog
    if(glfwGetKey(originalWindow,GLFW_KEY_ESCAPE) == GLFW_PRESS || bgPanel.sections[0].elements[0].button.hover && mouse.LDoubleClick){
        greetingDialogActive = true;
        this->dialogControl.unActivate();
    }

    dialogControl.updateEnd(timer,buttonShader,0.3f);
}