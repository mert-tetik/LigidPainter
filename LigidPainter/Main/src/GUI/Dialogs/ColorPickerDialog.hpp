
//  /*
//  ---------------------------------------------------------------------------
//  LigidPainter - 3D Model texturing software / Texture generator   
//  ---------------------------------------------------------------------------

//  Copyright (c) 2022-2023, LigidTools 

//  All rights reserved.

//  Official GitHub Link : https:github.com/mert-tetik/LigidPainter
//  Official Web Page : https:ligidtools.com/ligidpainter

//  ---------------------------------------------------------------------------
//  */

//  #ifndef LGDUICOLORPICKERDIALOG_HPP
//  #define LGDUICOLORPICKERDIALOG_HPP

//  #include<glad/glad.h>
//  #include<GLFW/glfw3.h>

//  #include <glm/glm.hpp>
//  #include <glm/gtc/matrix_transform.hpp>

//  #include "assimp/Importer.hpp"
//  #include "assimp/scene.h"
//  #include "assimp/postprocess.h"

//  #include "Shader.hpp"
//  #include "Box.hpp"
//  #include "Renderer.h"
//  #include "Util.hpp"

//  #include "GUI/UI.hpp"
//  #include "GUI/Button.hpp"
//  #include "Mouse.hpp"
//  #include "Timer.hpp"

//  #include "tinyfiledialogs.h"
 
//  #include "Project.hpp"

//  #include <glm/gtc/type_ptr.hpp>

//  #include <string>
//  #include <fstream>
//  #include <sstream>
//  #include <iostream>
//  #include <map>
//  #include <vector>
//  #include <cstdlib>

//  class ColorPickerDialog
//  {
//  private:
//      /* data */
//  public:
//     Panel panel;

//     ColorPickerDialog(){}

//     Box box;

//     Shader buttonShader;
//     Shader colorPickerShader;
    
//     RangeBar redRangeBar;
//     RangeBar greenRangeBar;
//     RangeBar blueRangeBar;

//     //Transform values of the panel
//     glm::vec3 pos = glm::vec3(50,50,0.95f);
//     glm::vec2 scale = glm::vec2(10,10);

//     ColorPickerDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,Shader colorPickerShader,AppTextures appTextures){
        
//         this->buttonShader = buttonShader;
//         this->colorPickerShader = colorPickerShader;
        
//         this->panel = Panel(
//                                 buttonShader,
//                                 colorPalette,
//                                 {
                                   
//                                 },
//                                 scale,
//                                 pos,
//                                 colorPalette.mainColor,
//                                 colorPalette.thirdColor,
//                                 true,
//                                 true,
//                                 true,
//                                 true,
//                                 true,
//                                 1.f,
//                                 1,
//                                 {},
//                                 0.25f
//                             );
        

//         redRangeBar = RangeBar(2,glm::vec2(scale.x/2.f,1.5f),colorPalette,buttonShader,"Red",Texture(),0.f,0,255,0);
//         redRangeBar.pos = pos;
//         redRangeBar.pos.x = pos.x + scale.x/2.f;
//         redRangeBar.pos.y = pos.y + scale.y/2.f;
        
//         greenRangeBar = RangeBar(2,glm::vec2(scale.x/2.f,1.5f),colorPalette,buttonShader,"Green",Texture(),0.f,0,255,0);
//         greenRangeBar.pos = pos;
//         greenRangeBar.pos.y = pos.y + scale.y/1.5f;
        
//         blueRangeBar = RangeBar(2,glm::vec2(scale.x/2.f,1.5f),colorPalette,buttonShader,"Blue",Texture(),0.f,0,255,0);
//         blueRangeBar.pos = pos;
//         blueRangeBar.pos.y = pos.y + scale.y/1.f;
//     }
    
//     void drawColorPicker(glm::vec2 aScale,glm::vec3 aPos){
//         colorPickerShader.use();

//         colorPickerShader.setVec2("scale", aScale);
//         colorPickerShader.setVec3("pos", aPos);
//         colorPickerShader.setInt("renderHSCircle", true);

//         glDrawArrays(GL_TRIANGLES, 0, 6);

//         glm::vec2 barScale = aScale;
//         barScale.x /= 8.f;
        
//         glm::vec3 barPos = aPos;
//         barPos.x = aPos.x + aScale.x;

//         colorPickerShader.setVec2("scale", barScale);
//         colorPickerShader.setVec3("pos", barPos);
//         colorPickerShader.setInt("renderHSCircle", false);
        
//         glDrawArrays(GL_TRIANGLES, 0, 6);
        
//         buttonShader.use();
//     }
    
//     void render(GLFWwindow* originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,glm::vec2 videoScale,Project &project){
//         panel.render(videoScale,mouse,timer,textRenderer,true);



//         Util util;
//         // pos value % of the video scale
//         glm::vec3 resultPos = glm::vec3( 
//                               util.getPercent(videoScale,glm::vec2(pos.x,pos.y)) //Don't include the depth
//                               ,pos.z); //Use the original depth value

//         // scale value % of the video scale
//         glm::vec2 resultScale = util.getPercent(videoScale,scale);

//         drawColorPicker(
//                             glm::vec2(min(resultScale.x,resultScale.y)),
//                             glm::vec3(resultPos.x  - resultScale.x + min(resultScale.x,resultScale.y),resultPos.y,resultPos.z)
//                         );
//         unsigned char defRGB[3];

//         tinyfd_colorChooserW((wchar_t*)"Color",(wchar_t*)"",defRGB,defRGB);
//         redRangeBar.render(videoScale,mouse,timer,textRenderer,true);
//         greenRangeBar.render(videoScale,mouse,timer,textRenderer,true);
//         blueRangeBar.render(videoScale,mouse,timer,textRenderer,true);
//     }
//  };

//  #endif