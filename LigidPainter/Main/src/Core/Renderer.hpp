#ifndef LGDRENDERER_HPP
#define LGDRENDERER_HPP

#include <string>

#include "Core/UI/UserInterface.h"
#include "Core/Utilities.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "../../thirdparty/tinyfiledialogs.h"


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
        ColorData colorData;
        UserInterface ui;
        buttonEnter = ui.isMouseOnButton(window,width,height,pos.x,pos.y,cursorPos.x,cursorPos.y,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);
        
        Utilities util;
        mixVal = util.transitionEffect(buttonEnter,mixVal,0.1f);
        
        ui.box(width,height,pos.x,pos.y,text,color1,0,false,false,pos.z,curveValue,color2,mixVal,curveValue >= 100 ? colorData.textColor : color1,0.0003f);
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
    glm::vec4 textColor;
    float textSize;
    bool clickable;
    GLFWwindow* window;

    bool folderDialog;

    bool buttonEnter;
    bool active = false;
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
    RendererTextBox(float widthD,float heightD,bool outlineD,std::string textD, glm::vec4 color1D ,glm::vec4 color2D,bool clickableD,glm::vec4 textColorD,float textSizeD,bool folderDialogD,GLFWwindow* windowD){
        width = widthD;
        height = heightD;
        outline = outlineD;
        text = textD;
        color1 = color1D;
        color2 = color2D;
        textColor = textColorD;
        textSize = textSizeD;
        clickable = clickableD;
        folderDialog = folderDialogD;
        window = windowD;
    }
    void draw(glm::vec3 pos,glm::vec2 cursorPos,bool firstClick){
        UserInterface ui;
        buttonEnter = ui.isMouseOnButton(window,width,height,pos.x,pos.y,cursorPos.x,cursorPos.y,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);
        
        if(buttonEnter && firstClick && !folderDialog)
            active = true;
        if((!buttonEnter && firstClick) || glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetKey(window,GLFW_KEY_ENTER) == GLFW_PRESS)
            active = false;

        if(folderDialog && firstClick && buttonEnter){
            char* check = tinyfd_selectFolderDialog("Select a folder", text.c_str());
            if(check)
                text = check;
        }
        
        Utilities util;
        mixVal = util.transitionEffect(active,mixVal,0.1f);

        ui.box(width,height,pos.x,pos.y,text,buttonEnter == true ? color1/glm::vec4(2.) : color1 ,0,true,false,pos.z,10,color2,mixVal,textColor,textSize);
    }
};

class RendererListBox{
public : 
    float width;
    float height;
    bool outline;
    std::vector<std::string> elements;
    int selectedIndex = 0;
    glm::vec4 color1;
    glm::vec4 color2;
    bool clickable;
    GLFWwindow* window;
    float textSize = 0.00022f;

    bool buttonEnter;
    bool active = false;
    float mixVal = 0;

    RendererListBox(){
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
    RendererListBox(float widthD,float heightD,bool outlineD,std::vector<std::string> elementsD,int selectedIndexD, glm::vec4 color1D ,glm::vec4 color2D,bool clickableD,float textSizeD,GLFWwindow* windowD){
        width = widthD;
        height = heightD;
        outline = outlineD;
        elements = elementsD;
        color1 = color1D;
        color2 = color2D;
        clickable = clickableD;
        window = windowD;
        textSize = textSizeD;
        selectedIndex = selectedIndexD;
    }
    void draw(glm::vec3 pos,glm::vec2 cursorPos,bool firstClick){
        UserInterface ui;
        buttonEnter = ui.isMouseOnButton(window,width,height,pos.x,pos.y,cursorPos.x,cursorPos.y,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);
        
        if(buttonEnter && firstClick)
            active = true;
        if((!buttonEnter && firstClick) || glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetKey(window,GLFW_KEY_ENTER) == GLFW_PRESS)
            active = false;
        
        Utilities util;
        mixVal = util.transitionEffect(active,mixVal,0.1f);

        
        ui.box(width,height,pos.x,pos.y,elements[selectedIndex],buttonEnter == true ? color1/glm::vec4(2.) : color1 ,0,false,false,pos.z,10,color2,mixVal,color1,textSize);

        if(active){
            for (size_t i = 0; i < elements.size(); i++)
            {
                ui.box(width,height,pos.x,pos.y - (height*2*(i+1)),elements[i],glm::vec4(color1.x,color1.y,color1.z,mixVal),0,false,false,pos.z,1000,color2,0);
            }
            
        }
    }
};
class RendererCheckBox
{
private:
    /* data */
public:
    std::string text;
    bool clickable;
    GLFWwindow* window;
    bool checked;

    bool buttonEnter;

    RendererCheckBox(){
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
    RendererCheckBox(std::string textD ,bool checkedD,GLFWwindow* windowD){
        text = textD;
        checked = checkedD;
        window = windowD;
    }
    void draw(glm::vec3 pos,glm::vec2 cursorPos,bool firstClick){
        UserInterface ui;
        buttonEnter = ui.isMouseOnButton(window,0.05,0.05,pos.x,pos.y,cursorPos.x,cursorPos.y,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);

        if(buttonEnter && firstClick)        
            checked = !checked;
            
        ui.checkBoxW(pos.x,pos.y,text,buttonEnter,checked,0);
    }
};

class Renderer
{
public:

    Renderer(int arg){

    }
    RendererButton startScreenNewProjectButton;
    RendererButton startScreenLoadProjectButton;
    
    RendererButton startScreenCreateTheProjectButton;

    RendererTextBox startScreenProjectTitleTextBox;
    RendererTextBox startScreenProjectPathTextBox;
    RendererListBox startScreenProjectResolutionTextBox;
    
    RendererCheckBox startScreenIncludeTexturesCheckBox;
    RendererCheckBox startScreenIncludeNodesCheckBox;
    RendererCheckBox startScreenIncludeFontsCheckBox;
};

#endif