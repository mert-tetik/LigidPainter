#ifndef LGDRENDERER_HPP
#define LGDRENDERER_HPP

#include <string>

#include "Core/UI/UserInterface.h"
#include "Core/Utilities.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>



class RendererButton
{
private:
    /* data */
public:
    float width;
    float height;
    float curveValue;
    bool outline;
    std::string text;
    glm::vec4 color1;
    glm::vec4 color2;
    bool clickable;
    GLFWwindow* window;


    bool buttonEnter;
    float mixVal = 0;

    RendererButton(){
        //width = widthD
        //height = heightD
        //curveValue = curveValueD
        //outline = outlineD
        //text = textD;
        //color1 = color1D;
        //color2 = color2D;
        //clickable = false;
        //window = windowD
    }//
    RendererButton(float widthD,float heightD,float curveValueD,bool outlineD,std::string textD, glm::vec4 color1D ,glm::vec4 color2D,bool clickableD,GLFWwindow* windowD){
        width = widthD;
        height = heightD;
        curveValue = curveValueD;
        outline = outlineD;
        text = textD;
        color1 = color1D;
        color2 = color2D;
        clickable = clickableD;
        window = windowD;
    }
    void draw(glm::vec3 pos,glm::vec2 cursorPos){
        UserInterface ui;
        buttonEnter = ui.isMouseOnButton(window,width,height,pos.x,pos.y,cursorPos.x,cursorPos.y,0);
        
        Utilities util;
        mixVal = util.transitionEffect(buttonEnter,mixVal,0.1f);
        
        ui.box(width,height,pos.x,pos.y,text,color1,0,false,false,pos.z,curveValue,color2,mixVal);
    }
};

class RendererTextBox{
public : 
    float width;
    float height;
    bool outline;
    std::string text;
    glm::vec4 color1;
    glm::vec4 color2;
    bool clickable;
    GLFWwindow* window;

    bool buttonEnter;
    float mixVal = 0;

    RendererTextBox(){
        //width = widthD
        //height = heightD
        //curveValue = curveValueD
        //outline = outlineD
        //text = textD;
        //color1 = color1D;
        //color2 = color2D;
        //clickable = false;
        //window = windowD
    }//
    RendererTextBox(float widthD,float heightD,bool outlineD,std::string textD, glm::vec4 color1D ,glm::vec4 color2D,bool clickableD,GLFWwindow* windowD){
        width = widthD;
        height = heightD;
        outline = outlineD;
        text = textD;
        color1 = color1D;
        color2 = color2D;
        clickable = clickableD;
        window = windowD;
    }
    void draw(glm::vec3 pos,glm::vec2 cursorPos){
        UserInterface ui;
        buttonEnter = ui.isMouseOnButton(window,width,height,pos.x,pos.y,cursorPos.x,cursorPos.y,0);
        
        Utilities util;
        mixVal = util.transitionEffect(buttonEnter,mixVal,0.1f);
        
        ui.box(width,height,pos.x,pos.y,text,color1,0,true,false,pos.z,10,color2,mixVal);
    }
};


class Renderer
{
public:
    Renderer(int arg){

    }
    RendererButton startScreenNewProjectButton;
    RendererButton startScreenLoadProjectButton;

    RendererTextBox startScreenProjectTitleTextBox;
};

#endif