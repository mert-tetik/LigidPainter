
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 Copyright (c) 2022-2023, Mert Tetik

 All rights reserved.

 Official GitHub Link : https:github.com/mert-tetik/LigidPainter
 Official Web Page : https:ligidtools.com/ligidpainter

 ---------------------------------------------------------------------------
 */
#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "ShaderSystem/Shader.hpp"

#include <string>
#include <iostream>
#include <vector>

DialogControl::DialogControl(){

}

DialogControl::DialogControl(bool active){
    this->active = active;
}

void DialogControl::updateStart(){
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", mixVal);
}

void DialogControl::updateEnd(Timer timer,float transitionDuration){
    
    timer.transition(active,mixVal,transitionDuration);
    firstFrameActivated = false;

    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.);
}

void DialogControl::activate(){
    active = true;
    firstFrameActivated = true;
}

void DialogControl::unActivate(){
    active = false;
}

bool DialogControl::isActive(){
    if(mixVal > 0.1f)
        return true;
    
    return active;
}

bool DialogControl::isComplete(){
    return mixVal > 0.9f;
}