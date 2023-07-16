
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 Copyright (c) 2022-2023, Mert Tetik

 All rights reserved.

 Official GitHub Link : https:github.com/mert-tetik/LigidPainter
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

                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(4,2),colorPalette,buttonShader,"New Project",Texture(),0.f,false)), 
                                        Element(TextBox(0,glm::vec2(4,2),colorPalette,buttonShader,"MyProject",2.f,false),context.window),
                                        Element(TextBox(0,glm::vec2(4,2),colorPalette,buttonShader,"./Projects",2.f,true),context.window),
                                        
                                        Element(ComboBox(ELEMENT_STYLE_BASIC,glm::vec2(4,2),colorPalette,buttonShader,
                                        {
                                            "256",
                                            "512",
                                            "1024",
                                            "2048",
                                            "4096"
                                        },"Texture Resolution",4.f),context.window),

                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(4,2),colorPalette,buttonShader,"Large",2.f)),
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(4,2),colorPalette,buttonShader,"Medium",0.f)),
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(4,2),colorPalette,buttonShader,"Small",0.f)),
                                        Element(TextBox(0,glm::vec2(4,2),colorPalette,buttonShader,"3D Model",2.f,2),context.window),
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(4,2),colorPalette,buttonShader,"Create",Texture(),2.f,false))
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

void NewProjectDialog::render(LigidWindow originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,
                                glm::vec2 videoScale,Project &project,bool &greetingDialogActive,bool &startScreen,Library &library,Shaders shaders,
                                Model &model, int &textureRes, std::vector<Node> &meshNodeScene ){
    
    dialogControl.updateStart(buttonShader);

    //Render the panel
    panel.render(videoScale,mouse,timer,textRenderer,dialogControl.isComplete());
    
    //If pressed to the last button of the panel (Create the project button)
    if(panel.sections[0].elements[panel.sections[0].elements.size()-1].button.hover && mouse.LClick){
        
        //Create the project
        if(project.createProject(   
                                    panel.sections[0].elements[2].textBox.text, //Destination path
                                    panel.sections[0].elements[1].textBox.text, //Title of the project
                                    panel.sections[0].elements[7].textBox.text,  //3D model path
                                    std::stoi(panel.sections[0].elements[3].comboBox.texts[panel.sections[0].elements[3].comboBox.selectedIndex])
                                 ))
        {
            project.loadProject(project.ligidFilePath(), library, shaders, model, appTextures, colorPalette, textureRes, meshNodeScene, videoScale);
            
            startScreen = false;

            this->dialogControl.unActivate();
        }
    
    }
    
    //Close the dialog
    if(originalWindow.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || panel.sections[0].elements[0].button.hover && mouse.LDoubleClick){
        if(startScreen)
            greetingDialogActive = true;
        
        this->dialogControl.unActivate();
    }

    dialogControl.updateEnd(timer,buttonShader,0.15f);
}