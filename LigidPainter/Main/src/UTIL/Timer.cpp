/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
    Timer takes care of time related things
    Like getting fps or frame rendering speed
    And helps you manage the time
*/

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UTIL/Util.hpp"

#include <string>
#include <iostream>
#include <vector>

Timer::Timer(/* args */){
    
}

bool Timer::runTimer(float cycleTime){ 
    //Returns true every second
    double currentTime = glfwGetTime();
    nbFramesT++;
    if (currentTime - lastTimeT >= cycleTime) {
        
    	if(nbFramesT > 5 && nbFramesT < 800)
            FPS = (double)nbFramesT;
            
        renderingSpeed = 1000./FPS;
        nbFramesT = 0;
    	lastTimeT += cycleTime;
        seconds++;
    	return true;
    }
    return false;
}

void Timer::transition(bool state ,float &value,float timeInSeconds){
    //If state is true move value to the 1 gradually
    //timeInSeconds determines how many time will pass since the value hits 1 or reduces back to 0
    //If timeInSeconds == 1.f value 0 will hit the value 1 in one second
    if (state && value <= 1.0f) {value += 1./(FPS) / timeInSeconds;}
    else if (!state && value >= 0.0f) {value -= 1./(FPS) / timeInSeconds;}
    if(value < 0.f)
    	value = 0.f;
    if(value > 1.f)
    	value = 1.f;
}