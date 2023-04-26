#include<iostream>

#include <string>
#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/LigidPainter.h"
#include "Core/UI/UserInterface.h"
#include "Core/gl.h"
#include "Core/Utilities.h"

float opacity = 0.f;
int frameCounter = 0;
bool start = false;

void UserInterface::renderAlert(std::string message,int duration,unsigned int uiProgram,int state,bool success){
    ColorData colorData;
    //TODO : Focus mode alert
    //TODO : Fix wrong positioning when the message is short
    Utilities util;

    if(state == 1){
        frameCounter = 0;
        start = true;
    }
    
    if(start)
        frameCounter++;

    if(frameCounter == duration){
        start = false;
    }

    const float phaseDifference = 0.1f;

    opacity = util.transitionEffect(start,opacity,phaseDifference);

    float posX = -((float)(message.size())+0.0001f)/250.f;
    glm::vec4 alertColor;
    if(!success)
        alertColor = glm::vec4(colorData.alertColor,opacity);
    else
        alertColor = glm::vec4(glm::vec3(0.11,0.76,0.11),opacity);
    if(opacity > 0.02f)
        renderText(uiProgram,message,posX,0.8f,0.00022f,alertColor,1.f,false);

}