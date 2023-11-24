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
#include "SettingsSystem/Settings.hpp"

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

void Painter::initPainter(){
    
    glm::ivec2 paintingRes = glm::ivec2(*Settings::videoScale() / Settings::properties()->paintingResolutionDivier);
    glm::ivec2 depthRes = glm::ivec2(*Settings::videoScale() / Settings::properties()->paintingDepthTextureResolutionDivier);

    //--------- init paintingTexture8 --------- 
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1,&this->paintingTexture8);
    glBindTexture(GL_TEXTURE_2D,this->paintingTexture8);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, paintingRes.x, paintingRes.y, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);

    this->paintingTexture = this->paintingTexture8;


    //--------- init paintingTexture16f --------- 
    glGenTextures(1,&this->paintingTexture16f);
    glBindTexture(GL_TEXTURE_2D,this->paintingTexture16f);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, paintingRes.x, paintingRes.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);



    //--------- init depthTextures --------- 
    this->oSide.depthTexture = Texture(nullptr, depthRes.x, depthRes.y, GL_LINEAR, GL_RED, GL_R32F);
    this->oSide.mirroredPaintingTexture = Texture(nullptr, 1, 1);
    this->oSide.projectedPaintingTexture = Texture(nullptr, 1, 1);
    this->oSide.effectAxis = glm::vec3(-1.f, -1.f, -1.f);
    
    this->oXSide.depthTexture = Texture(nullptr, depthRes.x, depthRes.y, GL_LINEAR, GL_RED, GL_R32F);
    this->oXSide.mirroredPaintingTexture = Texture(nullptr, 1, 1);
    this->oXSide.projectedPaintingTexture = Texture(nullptr, 1, 1);
    this->oXSide.effectAxis = glm::vec3(1.f, -1.f, -1.f);
    
    this->oYSide.depthTexture = Texture(nullptr, depthRes.x, depthRes.y, GL_LINEAR, GL_RED, GL_R32F);
    this->oYSide.mirroredPaintingTexture = Texture(nullptr, 1, 1);
    this->oYSide.projectedPaintingTexture = Texture(nullptr, 1, 1);
    this->oYSide.effectAxis = glm::vec3(-1.f, 1.f, -1.f);
    
    this->oXYSide.depthTexture = Texture(nullptr, depthRes.x, depthRes.y, GL_LINEAR, GL_RED, GL_R32F);
    this->oXYSide.mirroredPaintingTexture = Texture(nullptr, 1, 1);
    this->oXYSide.projectedPaintingTexture = Texture(nullptr, 1, 1);
    this->oXYSide.effectAxis = glm::vec3(1.f, 1.f, -1.f);
    
    this->oZSide.depthTexture = Texture(nullptr, depthRes.x, depthRes.y, GL_LINEAR, GL_RED, GL_R32F);
    this->oZSide.mirroredPaintingTexture = Texture(nullptr, 1, 1);
    this->oZSide.projectedPaintingTexture = Texture(nullptr, 1, 1);
    this->oZSide.effectAxis = glm::vec3(-1.f, -1.f, 1.f);
    
    this->oXZSide.depthTexture = Texture(nullptr, depthRes.x, depthRes.y, GL_LINEAR, GL_RED, GL_R32F);
    this->oXZSide.mirroredPaintingTexture = Texture(nullptr, 1, 1);
    this->oXZSide.projectedPaintingTexture = Texture(nullptr, 1, 1);
    this->oXZSide.effectAxis = glm::vec3(1.f, -1.f, 1.f);
    
    this->oYZSide.depthTexture = Texture(nullptr, depthRes.x, depthRes.y, GL_LINEAR, GL_RED, GL_R32F);
    this->oYZSide.mirroredPaintingTexture = Texture(nullptr, 1, 1);
    this->oYZSide.projectedPaintingTexture = Texture(nullptr, 1, 1);
    this->oYZSide.effectAxis = glm::vec3(-1.f, 1.f, 1.f);
    
    this->oXYZSide.depthTexture = Texture(nullptr, depthRes.x, depthRes.y, GL_LINEAR, GL_RED, GL_R32F);
    this->oXYZSide.mirroredPaintingTexture = Texture(nullptr, 1, 1);
    this->oXYZSide.projectedPaintingTexture = Texture(nullptr, 1, 1);
    this->oXYZSide.effectAxis = glm::vec3(1.f, 1.f, 1.f);
    




    //--------- init paintingOverTexture --------- 
    glGenTextures(1, &this->paintingOverTexture);
    glBindTexture(GL_TEXTURE_2D, this->paintingOverTexture);
 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, paintingRes.x, paintingRes.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);



    //--------- init paintingFBO --------- 
    glGenFramebuffers(1, &this->paintingFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, this->paintingFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, paintingTexture8, 0);
    


    //--------- init depthRBO --------- 
    glGenRenderbuffers(1,&depthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER,depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, depthRes.x, depthRes.y);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    
    
    //--------- init paintingBGTexture --------- 
    this->paintingBGTexture = Texture(nullptr, 1, 1);

    //--------- init projectedPaintingTexture --------- 
    this->projectedPaintingTexture = Texture(nullptr, 1, 1 , GL_LINEAR);
    
    //--------- Finish --------- 
    Settings::defaultFramebuffer()->FBO.bind();
}