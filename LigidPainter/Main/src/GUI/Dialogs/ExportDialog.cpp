
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

ExportDialog::ExportDialog(){}

ExportDialog::ExportDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures){
    
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
                                        Element(Button(0,glm::vec2(4,2),colorPalette,buttonShader,"Export",Texture(),0.f,false)), 
                                        Element(TextBox(0,glm::vec2(4,2),colorPalette,buttonShader,"Select A Path",2.f,true),context.window),
                                        
                                        Element(ComboBox(0,glm::vec2(4,2),colorPalette,buttonShader,
                                        {
                                            "256",
                                            "512",
                                            "1024",
                                            "2048",
                                            "4096"
                                        },4.f),context.window),
                                        
                                        Element(ComboBox(0,glm::vec2(4,2),colorPalette,buttonShader,
                                        {
                                            "PNG", 
                                            "JPEG", 
                                            "BMP", 
                                            "TIFF"
                                        },4.f),context.window),

                                        Element(Button(0,glm::vec2(4,2),colorPalette,buttonShader,"Export",Texture(),5.f,false))
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

void ExportDialog::render(GLFWwindow* originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,glm::vec2 videoScale,Project &project,bool &greetingDialogActive,Library &library,Shaders shaders,Model &model){
    
    //Render the panel
    panel.render(videoScale,mouse,timer,textRenderer,true);
    
    //If pressed to the last button of the panel (Export button)
    if(panel.sections[0].elements[panel.sections[0].elements.size()-1].button.hover && mouse.LClick){
        
    }
    
    //Close the dialog
    if(glfwGetKey(originalWindow,GLFW_KEY_ESCAPE) == GLFW_PRESS || panel.sections[0].elements[0].button.hover && mouse.LDoubleClick){
        greetingDialogActive = true;
        this->active = false;
    }
}