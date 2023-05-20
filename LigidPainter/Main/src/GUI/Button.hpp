/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef LGDUIBUTTON_HPP
#define LGDUIBUTTON_HPP

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Shader.hpp"
#include "Box.hpp"
#include "Renderer.h"
#include "Util.hpp"
#include "Timer.hpp"

#include "GUI/Panel.hpp"
#include "GUI/Button.hpp"
#include "Mouse.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

class Button
{
private:
    /* data */
public:
    //For now buttons can only be usable with panels

    Shader shader;

    //---Button properties
    std::string text;
    glm::vec4 color;
    glm::vec4 color2; 
    bool outline; 
    float radius; 
    int animationStyle;
    
    //Those values are adepting to the panel if button is attached to one
    //Values are percentage of the monitor size
    glm::vec2 scale;  //Example : w : 20, h : 30 means cover %20 of the window in x axis & cover 30% of the window in y axis
    glm::vec3 pos; //Same here
    
    bool hover = false;

    float hoverMixVal = 0.f;
    float clickedMixVal = 0.f;

    Button(){}
    Button(Shader shader, glm::vec2 scale, glm::vec4 color, glm::vec4 color2, bool outline, float radius, int animationStyle){
        //animationStyle determines what type of mouse hover or click animation will be used
        
        //0 = Change thickness for mousehover
        //1 = Change color for mousehover

        this->shader = shader;
        this->color = color;
        this->color2 = color2;
        this->scale = scale;
        this->outline = outline;
        this->radius = radius;
        this->animationStyle = animationStyle;
    }

    void render(glm::vec2 videoScale,Mouse& mouse, Timer &timer){
        Util util;

        // pos value % of the video scale
        glm::vec3 resultPos = glm::vec3( 
                              util.getPercent(videoScale,glm::vec2(pos.x,pos.y)) //Don't include the depth
                              ,pos.z); //Use the original depth value

        // scale value % of the video scale
        glm::vec2 resultScale = util.getPercent(videoScale,scale);

        //Check if mouse on top of the button
        hover = mouse.isMouseHover(resultScale,glm::vec2(resultPos.x,resultPos.y));

        if(hover && mouse.LClick){
            clickedMixVal = 1.f;
        }

        timer.transition(hover,hoverMixVal,0.2f); 
        timer.transition(false,clickedMixVal,0.5f); 
        

        shader.setVec3("pos"    ,     resultPos );
        shader.setVec2("scale"  ,     resultScale);
        shader.setVec4("color"  ,     color     );
        shader.setVec4("color2"  ,     color2     );
        shader.setFloat("colorMixVal"  ,     clickedMixVal     );

        shader.setFloat("width" ,     scale.x   );
        shader.setFloat("height",     scale.y   );

        shader.setFloat("radius",     radius    );
        shader.setInt("outline" ,     outline      );
        
        shader.setFloat("thickness" ,    100.f + hoverMixVal*100.f );

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
};
#endif