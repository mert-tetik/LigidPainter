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

void UserInterface::renderAlert(std::string message,int duration,unsigned int uiProgram,int state){
    ColorData colorData;

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

    renderText(uiProgram,message,-0.2f,0.9f,0.00022f,glm::vec4(colorData.alertColor,opacity),1.f);

}