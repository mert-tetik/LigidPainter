/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

#include "tinyfiledialogs.h"


Painter::Painter(){
    
}

void Painter::initPainter(glm::vec2 videoScale, Shader paintingShader, Shader buttonShader, Shader tdModelShader,Shader depth3DShader, Shader textureUpdatingShader, Shader twoDPaintingModeAreaShader, Shader boundaryExpandingShader){
    
    //Send the parameters to the member variables
    this->videoScale = videoScale;
    this->paintingShader = paintingShader;
    this->buttonShader = buttonShader;
    this->tdModelShader = tdModelShader;
    this->depth3DShader = depth3DShader;
    this->textureUpdatingShader = textureUpdatingShader;
    this->twoDPaintingModeAreaShader = twoDPaintingModeAreaShader;
    this->boundaryExpandingShader = boundaryExpandingShader;
    

    
    //--------- init paintingTexture8 --------- 
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1,&this->paintingTexture8);
    glBindTexture(GL_TEXTURE_2D,this->paintingTexture8);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, videoScale.x, videoScale.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);



    //--------- init paintingTexture16f --------- 
    glGenTextures(1,&this->paintingTexture16f);
    glBindTexture(GL_TEXTURE_2D,this->paintingTexture16f);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, videoScale.x, videoScale.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);



    //--------- init depthTexture --------- 
    glGenTextures(1,&this->depthTexture);
    glBindTexture(GL_TEXTURE_2D,this->depthTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, videoScale.x, videoScale.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);



    //--------- init paintingFBO --------- 
    glGenFramebuffers(1, &this->paintingFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, this->paintingFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, paintingTexture8, 0);
    


    //--------- init depthRBO --------- 
    glGenRenderbuffers(1,&depthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER,depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, videoScale.x, videoScale.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);


    
    //--------- Finish --------- 
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}