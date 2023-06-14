
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 Copyright (c) 2022-2023, LigidTools 

 All rights reserved.

 Official GitHub Link : https:github.com/mert-tetik/LigidPainter
 Official Web Page : https:ligidtools.com/ligidpainter

 ---------------------------------------------------------------------------
 
    TODO USE ALL THE PROJECT SETTINGS WHILE CREATING A PROJECT 

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

NewProjectDialog::NewProjectDialog(){}

NewProjectDialog::NewProjectDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures){
    
    //Take the parameters to the class member variables 
    this->buttonShader = buttonShader;
    this->appTextures = appTextures;
    
    //Create the panel
    this->panel = Panel(
                            buttonShader,
                            colorPalette,
                            {
                                Section(
                                    Element(Button()),
                                    {
                                        //Project settings

                                        Element(Button(0,glm::vec2(4,2),colorPalette,buttonShader,"New Project",Texture(),4.f,false)), 
                                        Element(TextBox(0,glm::vec2(4,2),colorPalette,buttonShader,"MyProject",2.f,false),context.window),
                                        Element(TextBox(0,glm::vec2(4,2),colorPalette,buttonShader,"./Projects",2.f,true),context.window),
                                        
                                        Element(ComboBox(0,glm::vec2(4,2),colorPalette,buttonShader,
                                        {
                                            "256",
                                            "512",
                                            "1024",
                                            "2048",
                                            "4096"
                                        },4.f),context.window),

                                        Element(CheckBox(0,glm::vec2(4,2),colorPalette,buttonShader,"Large",2.f)),
                                        Element(CheckBox(0,glm::vec2(4,2),colorPalette,buttonShader,"Medium",0.f)),
                                        Element(CheckBox(0,glm::vec2(4,2),colorPalette,buttonShader,"Small",0.f)),
                                        Element(TextBox(0,glm::vec2(4,2),colorPalette,buttonShader,"3D Model",2.f,2),context.window),
                                        Element(Button(1,glm::vec2(4,2),colorPalette,buttonShader,"Create",Texture(),2.f,false))
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
                            0.25f
                        );
    this->panel.sections[0].elements[0].button.color = colorPalette.secondColor;
    this->panel.sections[0].elements[0].button.color2 = colorPalette.thirdColor;
    this->panel.sections[0].elements[0].button.outlineColor = colorPalette.thirdColor;
    this->panel.sections[0].elements[0].button.outlineColor2 = colorPalette.thirdColor;
}

void NewProjectDialog::render(GLFWwindow* originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,glm::vec2 videoScale,Project &project,bool &greetingDialogActive,Library &library,Shaders shaders,Model &model){
    
    //Render the panel
    panel.render(videoScale,mouse,timer,textRenderer,true);
    
    //If pressed to the last button of the panel (Create the project button)
    if(panel.sections[0].elements[panel.sections[0].elements.size()-1].button.hover && mouse.LClick){
        
        //Create the project
        if(project.createProject(   
                                    panel.sections[0].elements[2].textBox.text, //Destination path
                                    panel.sections[0].elements[1].textBox.text, //Title of the project
                                    panel.sections[0].elements[7].textBox.text  //3D model path
                                 ))
        {
            project.loadProject(project.ligidFilePath,library,shaders,model,appTextures,colorPalette);
            this->active = false;
        }
    
    }
    
    //Close the dialog
    if(glfwGetKey(originalWindow,GLFW_KEY_ESCAPE) == GLFW_PRESS || panel.sections[0].elements[0].button.hover && mouse.LDoubleClick){
        greetingDialogActive = true;
        this->active = false;
    }
}