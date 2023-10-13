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
}

// TODOV Refresh texturefields after loading a new project

void TextureField::render(Timer& timer, bool doMouseTracking){
    
    // Positioning the GUI elements
    poisitionTheElement(this->pos, this->scale, this->topLeft_ResizeButton.pos, "LEFT_TOP");
    poisitionTheElement(this->pos, this->scale, this->bottomLeft_ResizeButton.pos, "LEFT_BOTTOM");
    poisitionTheElement(this->pos, this->scale, this->topRight_ResizeButton.pos, "RIGHT_TOP");
    poisitionTheElement(this->pos, this->scale, this->bottomRight_ResizeButton.pos, "RIGHT_BOTTOM");
    poisitionTheElement(this->pos, this->scale, this->textureDisplayingButton.pos, "CENTER");
    poisitionTheElement(this->pos, this->scale, this->deleteButton.pos, "LEFT_BOTTOM");
    poisitionTheElement(this->pos, this->scale, this->changeTextureButton.pos, "LEFT_BOTTOM");
    poisitionTheElement(this->pos, this->scale, this->scaleToTextureResolutionButton.pos, "LEFT_BOTTOM");

    deleteButton.pos.y += bottomLeft_ResizeButton.scale.y + deleteButton.scale.y;
    changeTextureButton.pos.y = deleteButton.pos.y;
    scaleToTextureResolutionButton.pos.y = deleteButton.pos.y;
    
    changeTextureButton.pos.x += deleteButton.scale.x + changeTextureButton.scale.x;
    scaleToTextureResolutionButton.pos.x = changeTextureButton.pos.x;
    scaleToTextureResolutionButton.pos.x += changeTextureButton.scale.x + scaleToTextureResolutionButton.scale.x;

    // Assigning the selected texture to the texture displaying button
    textureDisplayingButton.texture = this->texture;
    
    // Scaling the texture displaying button according to the selected texture
    textureDisplayingButton.scale = this->scale;

    
    // Rendering the GUI elements
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 0.4f);
    this->textureDisplayingButton.render(timer, doMouseTracking); 
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);
    
    this->topLeft_ResizeButton.render(timer, doMouseTracking);
    this->bottomLeft_ResizeButton.render(timer, doMouseTracking);
    this->topRight_ResizeButton.render(timer, doMouseTracking);
    this->bottomRight_ResizeButton.render(timer, doMouseTracking); 
    this->deleteButton.render(timer, doMouseTracking);
    this->changeTextureButton.render(timer, doMouseTracking);
    this->scaleToTextureResolutionButton.render(timer, doMouseTracking);
    

    resizing(
                this->pos, 
                this->scale, 
                this->topLeft_ResizeButton.clickState1, 
                this->bottomLeft_ResizeButton.clickState1, 
                this->topRight_ResizeButton.clickState1, 
                this->bottomRight_ResizeButton.clickState1,
                this->textureDisplayingButton.clickState1
            );
}