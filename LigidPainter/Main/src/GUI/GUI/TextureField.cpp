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
#include <glm/gtx/string_cast.hpp>

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "SettingsSystem/Settings.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"
#include "MouseSystem/Mouse.hpp"

#include <string>
#include <iostream>
#include <vector>

static float degreesBetweenPoints(const glm::vec2& point1, const glm::vec2& point2) {
    // Calculate the angle in radians between the points and the origin.
    float angleRadians = glm::atan(point2.y, point2.x) - glm::atan(point1.y, point1.x);

    // Convert radians to degrees.
    float angleDegrees = glm::degrees(angleRadians);

    // Ensure the result is within the [0, 360] range.
    angleDegrees = fmod(angleDegrees + 360.0f, 360.0f);

    return angleDegrees;
}

glm::vec2 rotate2D(glm::vec2 v, float angleDegrees) {
    float angleRadians = glm::radians(angleDegrees);
    float cosA = cos(angleRadians);
    float sinA = sin(angleRadians);

    glm::mat2 rotationMatrix = glm::mat2(cosA, -sinA, sinA, cosA);

    return rotationMatrix * v;
}

static void rotateSquareCorners(glm::vec3& corner1, glm::vec3& corner2, glm::vec3& corner3, glm::vec3& corner4, float rotationDegrees) {
    
    // Calculate the center of the square.
    glm::vec2 center = 0.25f * (glm::vec2(corner1) + glm::vec2(corner2) + glm::vec2(corner3) + glm::vec2(corner4));

    // Translate the corners so that the center becomes the origin.
    corner1 -= glm::vec3(center, 0.0f);
    corner2 -= glm::vec3(center, 0.0f);
    corner3 -= glm::vec3(center, 0.0f);
    corner4 -= glm::vec3(center, 0.0f);

    // Convert the rotation angle to radians.
    float rotationRadians = glm::radians(rotationDegrees);

    // Create a 2D rotation matrix.
    glm::mat3 rotationMatrix = glm::mat3(
        glm::vec3(glm::cos(rotationRadians), -glm::sin(rotationRadians), 0.0f),
        glm::vec3(glm::sin(rotationRadians), glm::cos(rotationRadians), 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    // Apply the rotation to each corner.
    corner1 = glm::vec3(rotationMatrix * glm::vec3(glm::vec2(corner1), 1.0f));
    corner2 = glm::vec3(rotationMatrix * glm::vec3(glm::vec2(corner2), 1.0f));
    corner3 = glm::vec3(rotationMatrix * glm::vec3(glm::vec2(corner3), 1.0f));
    corner4 = glm::vec3(rotationMatrix * glm::vec3(glm::vec2(corner4), 1.0f));

    // Translate the corners back to their original position.
    corner1 += glm::vec3(center, 0.0f);
    corner2 += glm::vec3(center, 0.0f);
    corner3 += glm::vec3(center, 0.0f);
    corner4 += glm::vec3(center, 0.0f);
}


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

    rotateBtn = Button(ELEMENT_STYLE_STYLIZED,glm::vec2(1.f, Settings::videoScale()->x / Settings::videoScale()->y) / 1.5f,"", Texture(), 1.f, false);
    rotateBtn.color.a /= 2.f;

    deleteButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1),"Delete", Settings::appTextures().trashIcon, 1.f,false);
    deleteButton.textColor2 = glm::vec4(1.f);
    scaleToTextureResolutionButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1), "Scale to texture resolution", Settings::appTextures().resizingIcon, 1.f,false);
    scaleToTextureResolutionButton.textColor2 = glm::vec4(1.f);
    flipHorizontalButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1), "Flip horizontal", Settings::appTextures().flipH, 1.f,false);
    flipHorizontalButton.textColor2 = glm::vec4(1.f);
    flipVerticalButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1), "Flip vertical", Settings::appTextures().flipV, 1.f,false);
    flipVerticalButton.textColor2 = glm::vec4(1.f);
    cancelRotationButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1), "Cancel rotation", Settings::appTextures().X, 1.f,false);
    cancelRotationButton.textColor2 = glm::vec4(1.f);
    rotationAngleDisplayButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1), "90.0", Texture(), 1.f,false);
    rotationAngleDisplayButton.textColor2 = glm::vec4(1.f);
    
    textureDisplayingButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),"", Texture(), 1.f,false);
    textureDisplayingButton.stretchTexture = true;
    textureDisplayingButton.color = glm::vec4(0.f);

    textureDisplayingButtonIOutline = Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),"", Texture(), 1.f,false);
}

static void poisitionTheElement(glm::vec3 orgPos, glm::vec2 orgScale, glm::vec3 &elementPos, std::string position, bool rotate, float rot){
  
    glm::vec2 nPos = glm::vec2(0.);  

    if(position == "LEFT_TOP"){
        nPos = glm::vec2(-orgScale.x, -orgScale.y);
    }
    else if(position == "RIGHT_TOP"){
        nPos = glm::vec2(+orgScale.x, -orgScale.y);
    }
    else if(position == "LEFT_BOTTOM"){
        nPos = glm::vec2(-orgScale.x, +orgScale.y);
    }
    else if(position == "RIGHT_BOTTOM"){
        nPos = glm::vec2(+orgScale.x, +orgScale.y);
    }
    else if(position == "CENTER"){
    
    }
    
    if(rotate)
        nPos = rotate2D(nPos, rot);

    elementPos = glm::vec3(orgPos.x + nPos.x, orgPos.y + nPos.y, orgPos.z);
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

void TextureField::render(Timer& timer, bool doMouseTracking, bool generatingTextureMode, std::vector<TextureField>& srcVector, int& i, bool renderTheTexture){
    
    glm::vec3 orgPos = this->pos;
    glm::vec2 orgScale = this->scale;
    orgPos -= glm::vec3(*Settings::videoScale() - (glm::vec2)getContext()->windowScale, 0.f) / glm::vec3(*Settings::videoScale(), 1.f) * glm::vec3(50.f,50.f,1.f);     
    orgScale /= (*Settings::videoScale() / (glm::vec2)getContext()->windowScale);   

    // Positioning the GUI elements
    poisitionTheElement(orgPos, orgScale, this->topLeft_ResizeButton.pos, "LEFT_TOP", false, this->rotation);
    poisitionTheElement(orgPos, orgScale, this->bottomLeft_ResizeButton.pos, "LEFT_BOTTOM", false, this->rotation);
    poisitionTheElement(orgPos, orgScale, this->topRight_ResizeButton.pos, "RIGHT_TOP", false, this->rotation);
    poisitionTheElement(orgPos, orgScale, this->bottomRight_ResizeButton.pos, "RIGHT_BOTTOM", false, this->rotation);
    poisitionTheElement(orgPos, orgScale, this->textureDisplayingButton.pos, "CENTER", false, this->rotation);
    poisitionTheElement(orgPos, orgScale, this->rotateBtn.pos, "CENTER", false, this->rotation);
    poisitionTheElement(orgPos, orgScale, this->textureDisplayingButtonIOutline.pos, "CENTER", false, this->rotation);
    poisitionTheElement(orgPos, orgScale, this->deleteButton.pos, "LEFT_BOTTOM", false, this->rotation);
    poisitionTheElement(orgPos, orgScale, this->scaleToTextureResolutionButton.pos, "LEFT_BOTTOM", false, this->rotation);
    poisitionTheElement(orgPos, orgScale, this->flipHorizontalButton.pos, "LEFT_BOTTOM", false, this->rotation);
    poisitionTheElement(orgPos, orgScale, this->flipVerticalButton.pos, "LEFT_BOTTOM", false, this->rotation);
    poisitionTheElement(orgPos, orgScale, this->cancelRotationButton.pos, "LEFT_BOTTOM", false, this->rotation);
    poisitionTheElement(orgPos, orgScale, this->rotationAngleDisplayButton.pos, "LEFT_BOTTOM", false, this->rotation);

    //rotateSquareCorners(this->topLeft_ResizeButton.pos, this->bottomLeft_ResizeButton.pos, this->topRight_ResizeButton.pos, this->bottomRight_ResizeButton.pos, this->rotation);
    glm::vec2 crsPos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;

	if((getContext()->window.isKeyPressed(LIGIDGL_KEY_R) || getContext()->window.isKeyClicked(LIGIDGL_KEY_R)) && this->active || (rotateBtn.clickState1 || (rotateBtn.hover && *Mouse::LClick()))){
        glm::vec2 direction = glm::normalize(crsPos - glm::vec2(orgPos));
        // Left side
        if(direction.x < -0.5 && direction.y > -0.85 && direction.y < 0.85){
            this->rotation += Mouse::mouseOffset()->y / 2.f;
        }
        // Right side
        if(direction.x > 0.5 && direction.y > -0.85 && direction.y < 0.85){
            this->rotation -= Mouse::mouseOffset()->y / 2.f;
        }
        // Top side
        if(direction.y < -0.85 && direction.x > -0.5 && direction.x < 0.5){
            this->rotation -= Mouse::mouseOffset()->x / 2.f;
        }
        // Bottom side
        if(direction.y > 0.85 && direction.x > -0.5 && direction.x < 0.5){
            this->rotation += Mouse::mouseOffset()->x / 2.f;
        }

        if(getContext()->window.isKeyClicked(LIGIDGL_KEY_R) || (rotateBtn.hover && *Mouse::LClick())){
            registerTextureFieldAction("Texture field rotated", srcVector);
        }
	}

    if(rotation > 360)
        rotation = 0;
    if(rotation < 0)
        rotation = 360;


    deleteButton.scale.x = 1.f + deleteButton.hoverMixVal * 2.f;
    scaleToTextureResolutionButton.scale.x = 1.f + scaleToTextureResolutionButton.hoverMixVal * 5.f;
    flipHorizontalButton.scale.x = 1.f + flipHorizontalButton.hoverMixVal * 4.f;
    flipVerticalButton.scale.x = 1.f + flipVerticalButton.hoverMixVal * 3.f;
    cancelRotationButton.scale.x = 1.f + cancelRotationButton.hoverMixVal * 4.f;
    rotationAngleDisplayButton.scale.x = 1.f + rotationAngleDisplayButton.hoverMixVal * 4.f;

    deleteButton.pos.x += deleteButton.scale.x;
    deleteButton.pos.y += bottomLeft_ResizeButton.scale.y + deleteButton.scale.y;
    scaleToTextureResolutionButton.pos.y = deleteButton.pos.y;
    scaleToTextureResolutionButton.pos.x = deleteButton.pos.x + deleteButton.scale.x + scaleToTextureResolutionButton.scale.x;
    flipHorizontalButton.pos.y = scaleToTextureResolutionButton.pos.y;
    flipHorizontalButton.pos.x = scaleToTextureResolutionButton.pos.x + scaleToTextureResolutionButton.scale.x + flipHorizontalButton.scale.x;
    flipVerticalButton.pos.y = flipHorizontalButton.pos.y;
    flipVerticalButton.pos.x = flipHorizontalButton.pos.x + flipHorizontalButton.scale.x + flipVerticalButton.scale.x;
    cancelRotationButton.pos.y = flipVerticalButton.pos.y;
    cancelRotationButton.pos.x = flipVerticalButton.pos.x + flipVerticalButton.scale.x + cancelRotationButton.scale.x;
    rotationAngleDisplayButton.pos.y = cancelRotationButton.pos.y;
    rotationAngleDisplayButton.pos.x = cancelRotationButton.pos.x + cancelRotationButton.scale.x + rotationAngleDisplayButton.scale.x;
    rotationAngleDisplayButton.text = "";
    if(rotationAngleDisplayButton.hover)
        rotationAngleDisplayButton.text += "Rotation : ";

    rotationAngleDisplayButton.text += std::to_string((int)this->rotation);

    // Assigning the selected texture to the texture displaying button
    textureDisplayingButton.texture = this->texture;
    textureDisplayingButtonIOutline.color = ColorPalette::themeColor;

    // Scaling the texture displaying button according to the selected texture
    textureDisplayingButton.scale = orgScale;
    textureDisplayingButtonIOutline.scale = orgScale + glm::vec2(0.2f,0.2f * (Settings::videoScale()->x / Settings::videoScale()->y));
    
        // Rendering the GUI elements
        if(!generatingTextureMode){
            if(renderTheTexture)
                ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 0.4f);
            else
                ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 0.f);
        }

        ShaderSystem::buttonShader().setFloat("rotation", this->rotation);
        
        this->textureDisplayingButton.render(timer, doMouseTracking); 
        
        ShaderSystem::buttonShader().setFloat("rotation", 0.f);
        ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);

    if(*Mouse::LClick()){
        if(this->textureDisplayingButton.hover && this->active && !this->rotateBtn.hover){
            registerTextureFieldAction("Texture field transformed", srcVector);
        }

        this->active = this->isHover();
    }

    if(this->active && !generatingTextureMode){
        if(renderTheTexture){
            glDepthFunc(GL_LESS);
            ShaderSystem::buttonShader().setFloat("rotation", this->rotation);
            this->textureDisplayingButtonIOutline.render(timer, false);
            ShaderSystem::buttonShader().setFloat("rotation", 0.f);
            glDepthFunc(GL_LEQUAL);
        }

        glDepthFunc(GL_ALWAYS);
        this->topLeft_ResizeButton.render(timer, doMouseTracking);
        this->bottomLeft_ResizeButton.render(timer, doMouseTracking);
        this->topRight_ResizeButton.render(timer, doMouseTracking);
        this->bottomRight_ResizeButton.render(timer, doMouseTracking); 
        this->deleteButton.render(timer, doMouseTracking);
        this->scaleToTextureResolutionButton.render(timer, doMouseTracking);
        this->flipHorizontalButton.render(timer, doMouseTracking);
        this->flipVerticalButton.render(timer, doMouseTracking);
        this->cancelRotationButton.render(timer, doMouseTracking);
        this->rotationAngleDisplayButton.render(timer, false);
        this->rotateBtn.render(timer, doMouseTracking);
        if(rotateBtn.clickState1){
            VectorStroke stroke = VectorStroke(rotateBtn.pos, *Mouse::cursorPos() / *Settings::videoScale() * 100.f, rotateBtn.pos);
            ShaderSystem::vectoralCurve().use();
        
            ShaderSystem::vectoralCurve().setVec3("pos", glm::vec3(Settings::videoScale()->x/2.f, Settings::videoScale()->y/2.f, 0.9f));
            ShaderSystem::vectoralCurve().setVec2("scale", glm::vec2(Settings::videoScale()->x/2.f, Settings::videoScale()->y/2.f));

            ShaderSystem::vectoralCurve().setVec2("direction", stroke.offsetPos);

            ShaderSystem::vectoralCurve().setVec2("startPos", stroke.startPos);
            ShaderSystem::vectoralCurve().setVec2("destPos", stroke.endPos);

            ShaderSystem::vectoralCurve().setVec2("percScale", *Settings::videoScale());
            ShaderSystem::vectoralCurve().setInt("lineCapturingMode", 0);

            ShaderSystem::vectoralCurve().setFloat("EDGE", 0.0005f);

            glDrawArrays(GL_TRIANGLES, 0 , 6);

            ShaderSystem::buttonShader().use();
        }
        glDepthFunc(GL_LEQUAL);
    }

    if(topLeft_ResizeButton.hover && *Mouse::LClick() || bottomLeft_ResizeButton.hover && *Mouse::LClick() || topRight_ResizeButton.hover && *Mouse::LClick() || bottomRight_ResizeButton.hover && *Mouse::LClick()){
        registerTextureFieldAction("Texture field resized", srcVector);
    }

    resizing(
                this->pos, 
                this->scale, 
                this->topLeft_ResizeButton.clickState1, 
                this->bottomLeft_ResizeButton.clickState1, 
                this->topRight_ResizeButton.clickState1, 
                this->bottomRight_ResizeButton.clickState1,
                this->textureDisplayingButton.clickState1 && !this->rotateBtn.clickState1
            );

    if(this->deleteButton.clicked){
        registerTextureFieldAction("Texture field deleted", srcVector);
        srcVector.erase(srcVector.begin() + i);
        i--;
    }
    else if(this->scaleToTextureResolutionButton.clicked){
        registerTextureFieldAction("Texture field scaled to the texture resolution", srcVector);
        glm::vec2 prevScale = scale;
        scaleAccordingToTextureRes(scale, texture);
        pos.x -= prevScale.x -scale.x; 
    }
    else if(this->flipHorizontalButton.clicked){
        registerTextureFieldAction("Texture field flipped horizontally", srcVector);
        this->flippedH = !this->flippedH;
        this->texture.flipTexture(true, false);
        Settings::defaultFramebuffer()->FBO.bind();
    }
    else if(this->flipVerticalButton.clicked){
        registerTextureFieldAction("Texture field flipped vertically", srcVector);
        this->flippedV = !this->flippedV;
        this->texture.flipTexture(false, true);
        Settings::defaultFramebuffer()->FBO.bind();
    }
    else if(this->cancelRotationButton.clicked){
        registerTextureFieldAction("Texture field rotation set to 0", srcVector);
        this->rotation = 0.f;
    }
    
    this->transformedFlag = !(this->prevPos != orgPos || this->prevScale != orgScale || this->prevRot != this->rotation) && this->prevTransformedFlag; 

    this->prevTransformedFlag = this->prevPos != orgPos || this->prevScale != orgScale || this->prevRot != this->rotation;

    this->prevPos = orgPos;
    this->prevScale = orgScale;
    this->prevRot = this->rotation;
}

bool TextureField::isHover(){
    return  this->textureDisplayingButton.hover || 
            this->topLeft_ResizeButton.hover ||
            this->bottomLeft_ResizeButton.hover ||
            this->topRight_ResizeButton.hover ||
            this->bottomRight_ResizeButton.hover ||
            this->deleteButton.hover ||
            this->scaleToTextureResolutionButton.hover ||
            this->flipHorizontalButton.hover ||
            this->flipVerticalButton.hover ||
            this->cancelRotationButton.hover ||
            this->rotationAngleDisplayButton.hover;
}