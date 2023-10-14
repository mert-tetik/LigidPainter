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

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "SettingsSystem/Settings.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"
#include "MouseSystem/Mouse.hpp"

#include <string>
#include <iostream>
#include <vector>

static void scaleAccordingToTextureRes(glm::vec2& scale, Texture txtr){
    float txtrRatio = (float)txtr.getResolution().x / (float)txtr.getResolution().y;
    if((float)txtr.getResolution().x < (float)txtr.getResolution().y)
        txtrRatio = (float)txtr.getResolution().y / (float)txtr.getResolution().x;

    float videoRatio = (float)Settings::videoScale()->x / (float)Settings::videoScale()->y;
    
    if((float)txtr.getResolution().x > (float)txtr.getResolution().y)
        scale = glm::vec2(scale.x, scale.x / txtrRatio * videoRatio);
    else
        scale = glm::vec2(scale.y / txtrRatio / videoRatio, scale.y);
}

TextureField::TextureField(Texture texture){
    this->texture = texture;
    this->scale = glm::vec2(10.f);
    scaleAccordingToTextureRes(this->scale, texture);
    pos = glm::vec3(50.f, 50.f, 0.9f);

    topLeft_ResizeButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(0.5f, 0.5f * (Settings::videoScale()->x / Settings::videoScale()->y)), "", Texture(), 1.f,false);
    bottomLeft_ResizeButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(0.5f, 0.5f * (Settings::videoScale()->x / Settings::videoScale()->y)), "", Texture(), 1.f,false);
    topRight_ResizeButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(0.5f, 0.5f * (Settings::videoScale()->x / Settings::videoScale()->y)), "", Texture(), 1.f,false);
    bottomRight_ResizeButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(0.5f, 0.5f * (Settings::videoScale()->x / Settings::videoScale()->y)), "", Texture(), 1.f,false);

    topLeft_ResizeButton.color = ColorPalette::themeColor;
    topLeft_ResizeButton.radius = 0.36f;
    bottomLeft_ResizeButton.color = ColorPalette::themeColor;
    bottomLeft_ResizeButton.radius = 0.36f;
    topRight_ResizeButton.color = ColorPalette::themeColor;
    topRight_ResizeButton.radius = 0.36f;
    bottomRight_ResizeButton.color = ColorPalette::themeColor;
    bottomRight_ResizeButton.radius = 0.36f;

    deleteButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1),"", Settings::appTextures().modifierIcon, 1.f,false);
    scaleToTextureResolutionButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1), "", Settings::appTextures().resizingIcon, 1.f,false);

    textureDisplayingButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),"", Texture(), 1.f,false);
    textureDisplayingButton.stretchTexture = true;
    textureDisplayingButton.color = glm::vec4(0.f);

    textureDisplayingButtonIOutline = Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),"", Texture(), 1.f,false);
}

static void poisitionTheElement(glm::vec3 orgPos, glm::vec2 orgScale, glm::vec3 &elementPos, const char* position){
    if(position == "LEFT_TOP"){
        elementPos = glm::vec3(orgPos.x - orgScale.x, orgPos.y - orgScale.y, orgPos.z);
    }
    else if(position == "RIGHT_TOP"){
        elementPos = glm::vec3(orgPos.x + orgScale.x, orgPos.y - orgScale.y, orgPos.z);
    }
    else if(position == "LEFT_BOTTOM"){
        elementPos = glm::vec3(orgPos.x - orgScale.x, orgPos.y + orgScale.y, orgPos.z);
    }
    else if(position == "RIGHT_BOTTOM"){
        elementPos = glm::vec3(orgPos.x + orgScale.x, orgPos.y + orgScale.y, orgPos.z);
    }
    else if(position == "CENTER"){
        elementPos = glm::vec3(orgPos.x, orgPos.y, orgPos.z);
    }
}

static void resizing(glm::vec3& pos, glm::vec2& scale, bool LT, bool LB, bool RT, bool RB, bool center){
    glm::vec2 crsrOffset = *Mouse::mouseOffset() / (glm::vec2)getContext()->windowScale * 100.f;
    
    if(RB){
        crsrOffset /= 1.75;
        pos.x += crsrOffset.x;
        pos.y += crsrOffset.y;
        scale.x += crsrOffset.x;
        scale.y += crsrOffset.y;
    }
    
    else if(LB){
        crsrOffset /= 1.75;
        pos.x += crsrOffset.x;
        pos.y += crsrOffset.y;
        scale.x -= crsrOffset.x;
        scale.y += crsrOffset.y;
    }
    else if(RT){
        crsrOffset /= 1.75;
        pos.x += crsrOffset.x;
        pos.y += crsrOffset.y;
        scale.x += crsrOffset.x;
        scale.y -= crsrOffset.y;
    }
    
    else if(LT){
        crsrOffset /= 1.75;
        pos.x += crsrOffset.x;
        pos.y += crsrOffset.y;
        scale.x -= crsrOffset.x;
        scale.y -= crsrOffset.y;
    }
    
    else if(center){
        pos.x += crsrOffset.x;
        pos.y += crsrOffset.y;
    }

    if(pos.x < 0.f)
        pos.x = 0.f;
    if(pos.x > 100.f)
        pos.x = 100.f;
    
    if(pos.y < 0.f)
        pos.y = 0.f;
    if(pos.y > 100.f)
        pos.y = 100.f;

    if(scale.x < 2.f){
        pos.x -= crsrOffset.x;
        scale.x = 2.f;
    }
    if(scale.y < 2.f){
        pos.y -= crsrOffset.y;
        scale.y = 2.f;
    }
}

// TODO Refresh texturefields after loading a new project

void TextureField::render(Timer& timer, bool doMouseTracking, bool generatingTextureMode, std::vector<TextureField>& srcVector, int& i){
    
    // Positioning the GUI elements
    poisitionTheElement(this->pos, this->scale, this->topLeft_ResizeButton.pos, "LEFT_TOP");
    poisitionTheElement(this->pos, this->scale, this->bottomLeft_ResizeButton.pos, "LEFT_BOTTOM");
    poisitionTheElement(this->pos, this->scale, this->topRight_ResizeButton.pos, "RIGHT_TOP");
    poisitionTheElement(this->pos, this->scale, this->bottomRight_ResizeButton.pos, "RIGHT_BOTTOM");
    poisitionTheElement(this->pos, this->scale, this->textureDisplayingButton.pos, "CENTER");
    poisitionTheElement(this->pos, this->scale, this->textureDisplayingButtonIOutline.pos, "CENTER");
    poisitionTheElement(this->pos, this->scale, this->deleteButton.pos, "LEFT_BOTTOM");
    poisitionTheElement(this->pos, this->scale, this->scaleToTextureResolutionButton.pos, "LEFT_BOTTOM");

    deleteButton.pos.y += bottomLeft_ResizeButton.scale.y + deleteButton.scale.y;
    scaleToTextureResolutionButton.pos.y = deleteButton.pos.y;
    scaleToTextureResolutionButton.pos.x += deleteButton.scale.x + scaleToTextureResolutionButton.scale.x;

    // Assigning the selected texture to the texture displaying button
    textureDisplayingButton.texture = this->texture;
    textureDisplayingButtonIOutline.color = ColorPalette::themeColor;

    // Scaling the texture displaying button according to the selected texture
    textureDisplayingButton.scale = this->scale;
    textureDisplayingButtonIOutline.scale = this->scale + glm::vec2(0.2f,0.2f * (Settings::videoScale()->x / Settings::videoScale()->y));
    
    // Rendering the GUI elements
    if(!generatingTextureMode)
        ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 0.4f);
    this->textureDisplayingButton.render(timer, doMouseTracking); 
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);

    if(*Mouse::LClick()){
        this->active = this->isHover();
    }

    if(this->active && !generatingTextureMode){
        glDepthFunc(GL_LESS);
        this->textureDisplayingButtonIOutline.render(timer, false);
        glDepthFunc(GL_LEQUAL);

        this->topLeft_ResizeButton.render(timer, doMouseTracking);
        this->bottomLeft_ResizeButton.render(timer, doMouseTracking);
        this->topRight_ResizeButton.render(timer, doMouseTracking);
        this->bottomRight_ResizeButton.render(timer, doMouseTracking); 
        this->deleteButton.render(timer, doMouseTracking);
        this->scaleToTextureResolutionButton.render(timer, doMouseTracking);
    }

    resizing(
                this->pos, 
                this->scale, 
                this->topLeft_ResizeButton.clickState1, 
                this->bottomLeft_ResizeButton.clickState1, 
                this->topRight_ResizeButton.clickState1, 
                this->bottomRight_ResizeButton.clickState1,
                this->textureDisplayingButton.clickState1
            );

    if(this->deleteButton.clicked){
        glDeleteTextures(1, &srcVector[i].texture.ID);
        srcVector.erase(srcVector.begin() + i);
        i--;
    }
    else if(this->scaleToTextureResolutionButton.clicked){
        glm::vec2 prevScale = this->scale;
        scaleAccordingToTextureRes(this->scale, texture);
        this->pos.x -= prevScale.x - this->scale.x; 
    }
    
    this->transformedFlag = !(this->prevPos != this->pos || this->prevScale != this->scale) && this->prevTransformedFlag; 

    this->prevTransformedFlag = this->prevPos != this->pos || this->prevScale != this->scale;

    this->prevPos = this->pos;
    this->prevScale = this->scale;
}

bool TextureField::isHover(){
    return  this->textureDisplayingButton.hover || 
            this->topLeft_ResizeButton.hover ||
            this->bottomLeft_ResizeButton.hover ||
            this->topRight_ResizeButton.hover ||
            this->bottomRight_ResizeButton.hover ||
            this->deleteButton.hover ||
            this->scaleToTextureResolutionButton.hover;
}