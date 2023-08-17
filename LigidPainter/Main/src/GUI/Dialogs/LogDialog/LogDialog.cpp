
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
#include "MouseSystem/Mouse.hpp" 

#include <string>
#include <iostream>
#include <vector>
   
LogDialog::LogDialog(){

}
namespace LGDLOG{
    LogMsg start;
    std::string end = "$#";
}

LogDialog::LogDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,AppTextures appTextures, AppMaterialModifiers& appMaterialModifiers){
    this->panel = Panel(
        colorPalette,
        {
            {
                Section(
                    Element(Button()),
                    {

                    }
                )
            }
        },
        glm::vec2(10.f,16.f),
        glm::vec3(50.f,50.f,0.8f),
        colorPalette.mainColor,
        colorPalette.thirdColor,
        true,
        true,
        true,
        true,
        true,
        1.f,
        1.f,
        {},
        0.25f,
        false
    );

    this->panel.hasSlider = true;
}

void LogDialog::render(LigidWindow originalWindow, ColorPalette colorPalette,Timer timer, TextRenderer &textRenderer, glm::vec2 videoScale){
    
    std::vector<std::string> messages;
    
    std::string a;
    LGDLOG::start >> a;
    for (size_t i = 0; i < a.size(); i++)
    {
        if(i < a.size()-1){
            if(a[i] == '$' && a[i+1] == '#'){
                std::string res(a.begin(), a.begin() + i );

                a.erase(a.begin(), a.begin() + i + 2);
                i = 0;
                messages.push_back(res);
            }
        }
    }

    messages.push_back(a);

    std::vector<Section> logSections;
    logSections.push_back(Section(
                                    Element(),
                                    {
                                        
                                    }
                                )
                        );
    for (size_t i = 0; i < messages.size(); i++)
    {
        logSections[0].elements.push_back(Button(0, glm::vec2(1), colorPalette, messages[i], Texture(), 0., false));
    }
    
    this->panel.sections = logSections;
    this->panel.render(videoScale, timer, textRenderer, true);
}