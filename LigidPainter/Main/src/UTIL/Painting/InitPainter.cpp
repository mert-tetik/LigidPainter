/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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
    
    glm::ivec2 paintingRes = glm::vec2(getBufferResolutions(0));
    glm::ivec2 depthRes = glm::vec2(getBufferResolutions(1));
    glm::ivec2 lowResTxtrRes = glm::vec2(getBufferResolutions(2));

    //--------- init mesh mask ---------
    this->faceSelection.meshMask = Texture(nullptr, 1024, 1024);

    //--------- init paintingTexture8 ---------
    paintingTexture8 = Texture(nullptr, paintingRes.x, paintingRes.y, GL_LINEAR, GL_RGBA, GL_RGBA8); 
    this->paintingTexture = this->paintingTexture8;

    //--------- init paintingTexture16f --------- 
    paintingTexture16f = Texture(nullptr, paintingRes.x, paintingRes.y, GL_LINEAR, GL_RGBA, GL_RGBA16F); 

    //--------- init depthTextures --------- 
    this->oSide.depthTexture = Texture(nullptr, depthRes.x, depthRes.y, GL_LINEAR, GL_RGBA, GL_RGBA32F);
    this->oSide.mirroredPaintingTexture = Texture(nullptr, paintingRes.x, paintingRes.y);
    this->oSide.projectedPaintingTexture8Low = Texture(nullptr, lowResTxtrRes.x, lowResTxtrRes.y, GL_LINEAR);
    this->oSide.projectedPaintingTexture16fLow = Texture(nullptr, lowResTxtrRes.x, lowResTxtrRes.y, GL_LINEAR, GL_RGBA, GL_RGBA16F);
    this->oSide.projectedPaintingTexture8 = Texture(nullptr, 1, 1, GL_LINEAR);
    this->oSide.projectedPaintingTexture16f = Texture(nullptr, 1, 1, GL_LINEAR, GL_RGBA, GL_RGBA16F);
    this->oSide.projectedPaintingTexture = projectedPaintingTexture8Low;

    this->oSide.effectAxis = glm::vec3(-1.f, -1.f, -1.f);
    
    this->oXSide.depthTexture = Texture(nullptr, depthRes.x, depthRes.y, GL_LINEAR, GL_RGBA, GL_RGBA32F);
    this->oXSide.mirroredPaintingTexture = Texture(nullptr, paintingRes.x, paintingRes.y);
    this->oXSide.projectedPaintingTexture8Low = Texture(nullptr, lowResTxtrRes.x, lowResTxtrRes.y, GL_LINEAR);
    this->oXSide.projectedPaintingTexture16fLow = Texture(nullptr, lowResTxtrRes.x, lowResTxtrRes.y, GL_LINEAR, GL_RGBA, GL_RGBA16F);
    this->oXSide.projectedPaintingTexture8 = Texture(nullptr, 1, 1, GL_LINEAR);
    this->oXSide.projectedPaintingTexture16f = Texture(nullptr, 1, 1, GL_LINEAR, GL_RGBA, GL_RGBA16F);
    this->oXSide.projectedPaintingTexture = projectedPaintingTexture8Low;

    this->oXSide.effectAxis = glm::vec3(1.f, -1.f, -1.f);
    
    this->oYSide.depthTexture = Texture(nullptr, depthRes.x, depthRes.y, GL_LINEAR, GL_RGBA, GL_RGBA32F);
    this->oYSide.mirroredPaintingTexture = Texture(nullptr, paintingRes.x, paintingRes.y);
    this->oYSide.projectedPaintingTexture8Low = Texture(nullptr, lowResTxtrRes.x, lowResTxtrRes.y, GL_LINEAR);
    this->oYSide.projectedPaintingTexture16fLow = Texture(nullptr, lowResTxtrRes.x, lowResTxtrRes.y, GL_LINEAR, GL_RGBA, GL_RGBA16F);
    this->oYSide.projectedPaintingTexture8 = Texture(nullptr, 1, 1, GL_LINEAR);
    this->oYSide.projectedPaintingTexture16f = Texture(nullptr, 1, 1, GL_LINEAR, GL_RGBA, GL_RGBA16F);
    this->oYSide.projectedPaintingTexture = projectedPaintingTexture8Low;

    this->oYSide.effectAxis = glm::vec3(-1.f, 1.f, -1.f);
    
    this->oXYSide.depthTexture = Texture(nullptr, depthRes.x, depthRes.y, GL_LINEAR, GL_RGBA, GL_RGBA32F);
    this->oXYSide.mirroredPaintingTexture = Texture(nullptr, paintingRes.x, paintingRes.y);
    this->oXYSide.projectedPaintingTexture8Low = Texture(nullptr, lowResTxtrRes.x, lowResTxtrRes.y, GL_LINEAR);
    this->oXYSide.projectedPaintingTexture16fLow = Texture(nullptr, lowResTxtrRes.x, lowResTxtrRes.y, GL_LINEAR, GL_RGBA, GL_RGBA16F);
    this->oXYSide.projectedPaintingTexture8 = Texture(nullptr, 1, 1, GL_LINEAR);
    this->oXYSide.projectedPaintingTexture16f = Texture(nullptr, 1, 1, GL_LINEAR, GL_RGBA, GL_RGBA16F);
    this->oXYSide.projectedPaintingTexture = projectedPaintingTexture8Low;

    this->oXYSide.effectAxis = glm::vec3(1.f, 1.f, -1.f);
    
    this->oZSide.depthTexture = Texture(nullptr, depthRes.x, depthRes.y, GL_LINEAR, GL_RGBA, GL_RGBA32F);
    this->oZSide.mirroredPaintingTexture = Texture(nullptr, paintingRes.x, paintingRes.y);
    this->oZSide.projectedPaintingTexture8Low = Texture(nullptr, lowResTxtrRes.x, lowResTxtrRes.y, GL_LINEAR);
    this->oZSide.projectedPaintingTexture16fLow = Texture(nullptr, lowResTxtrRes.x, lowResTxtrRes.y, GL_LINEAR, GL_RGBA, GL_RGBA16F);
    this->oZSide.projectedPaintingTexture8 = Texture(nullptr, 1, 1, GL_LINEAR);
    this->oZSide.projectedPaintingTexture16f = Texture(nullptr, 1, 1, GL_LINEAR, GL_RGBA, GL_RGBA16F);
    this->oZSide.projectedPaintingTexture = projectedPaintingTexture8Low;

    this->oZSide.effectAxis = glm::vec3(-1.f, -1.f, 1.f);
    
    this->oXZSide.depthTexture = Texture(nullptr, depthRes.x, depthRes.y, GL_LINEAR, GL_RGBA, GL_RGBA32F);
    this->oXZSide.mirroredPaintingTexture = Texture(nullptr, paintingRes.x, paintingRes.y);
    this->oXZSide.projectedPaintingTexture8Low = Texture(nullptr, lowResTxtrRes.x, lowResTxtrRes.y, GL_LINEAR);
    this->oXZSide.projectedPaintingTexture16fLow = Texture(nullptr, lowResTxtrRes.x, lowResTxtrRes.y, GL_LINEAR, GL_RGBA, GL_RGBA16F);
    this->oXZSide.projectedPaintingTexture8 = Texture(nullptr, 1, 1, GL_LINEAR);
    this->oXZSide.projectedPaintingTexture16f = Texture(nullptr, 1, 1, GL_LINEAR, GL_RGBA, GL_RGBA16F);
    this->oXZSide.projectedPaintingTexture = projectedPaintingTexture8Low;

    this->oXZSide.effectAxis = glm::vec3(1.f, -1.f, 1.f);
    
    this->oYZSide.depthTexture = Texture(nullptr, depthRes.x, depthRes.y, GL_LINEAR, GL_RGBA, GL_RGBA32F);
    this->oYZSide.mirroredPaintingTexture = Texture(nullptr, paintingRes.x, paintingRes.y);
    this->oYZSide.projectedPaintingTexture8Low = Texture(nullptr, lowResTxtrRes.x, lowResTxtrRes.y, GL_LINEAR);
    this->oYZSide.projectedPaintingTexture16fLow = Texture(nullptr, lowResTxtrRes.x, lowResTxtrRes.y, GL_LINEAR, GL_RGBA, GL_RGBA16F);
    this->oYZSide.projectedPaintingTexture8 = Texture(nullptr, 1, 1, GL_LINEAR);
    this->oYZSide.projectedPaintingTexture16f = Texture(nullptr, 1, 1, GL_LINEAR, GL_RGBA, GL_RGBA16F);
    this->oYZSide.projectedPaintingTexture = projectedPaintingTexture8Low;

    this->oYZSide.effectAxis = glm::vec3(-1.f, 1.f, 1.f);
    
    this->oXYZSide.depthTexture = Texture(nullptr, depthRes.x, depthRes.y, GL_LINEAR, GL_RGBA, GL_RGBA32F);
    this->oXYZSide.mirroredPaintingTexture = Texture(nullptr, paintingRes.x, paintingRes.y);
    this->oXYZSide.projectedPaintingTexture8Low = Texture(nullptr, lowResTxtrRes.x, lowResTxtrRes.y, GL_LINEAR);
    this->oXYZSide.projectedPaintingTexture16fLow = Texture(nullptr, lowResTxtrRes.x, lowResTxtrRes.y, GL_LINEAR, GL_RGBA, GL_RGBA16F);
    this->oXYZSide.projectedPaintingTexture8 = Texture(nullptr, 1, 1, GL_LINEAR);
    this->oXYZSide.projectedPaintingTexture16f = Texture(nullptr, 1, 1, GL_LINEAR, GL_RGBA, GL_RGBA16F);
    this->oXYZSide.projectedPaintingTexture = projectedPaintingTexture8Low;

    this->oXYZSide.effectAxis = glm::vec3(1.f, 1.f, 1.f);

    //--------- init paintingFBO --------- 
    this->paintingFBO = Framebuffer(paintingTexture8, GL_TEXTURE_2D, "Painting feature's FBO");
    
    //--------- init depthRBO --------- 
    depthRBO1024 = Renderbuffer(GL_DEPTH_COMPONENT32F, GL_DEPTH_ATTACHMENT, glm::ivec2(1024));
    depthRBO512 = Renderbuffer(GL_DEPTH_COMPONENT32F, GL_DEPTH_ATTACHMENT, glm::ivec2(512));
    depthRBOcustom = Renderbuffer(GL_DEPTH_COMPONENT32F, GL_DEPTH_ATTACHMENT, glm::ivec2(1024));
    
    //--------- init paintingBGTexture --------- 
    this->paintingBGTexture = Texture(nullptr, lowResTxtrRes.x, lowResTxtrRes.y);

    //--------- init projectedPaintingTexture --------- 
    this->projectedPaintingTexture8Low = Texture(nullptr, lowResTxtrRes.x, lowResTxtrRes.y, GL_LINEAR);
    this->projectedPaintingTexture16fLow = Texture(nullptr, lowResTxtrRes.x, lowResTxtrRes.y, GL_LINEAR, GL_RGBA, GL_RGBA16F);
    this->projectedPaintingTexture8 = Texture(nullptr, 1, 1, GL_LINEAR);
    this->projectedPaintingTexture16f = Texture(nullptr, 1, 1, GL_LINEAR, GL_RGBA, GL_RGBA16F);
    this->projectedPaintingTexture = projectedPaintingTexture8Low;

    
    //--------- Finish --------- 
    Settings::defaultFramebuffer()->FBO.bind();
}