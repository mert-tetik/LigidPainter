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
#include <glm/gtx/string_cast.hpp>

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "Toolkit/TextureFieldScene/TextureFieldScene.hpp"

#include <string>
#include <iostream>
#include <vector>

static void resizing(glm::vec3& pos, glm::vec2& scale, bool LT, bool LB, bool RT, bool RB, bool center);
static void poisitionTheElement(glm::vec3 orgPos, glm::vec2 orgScale, glm::vec3 &elementPos, std::string position, bool rotate, float rot);
static glm::vec2 rotate2D(glm::vec2 v, float angleDegrees);

/* Defined in the TextureField.cpp */
extern bool textureField_alreadyInteracted;

void TextureField::position2DElements(){
    // Position the texture field to match with the window ratio
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
    poisitionTheElement(orgPos, orgScale, this->wrapModeButton.pos, "LEFT_BOTTOM", false, this->rotation);

    
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
    wrapModeButton.pos.y = rotationAngleDisplayButton.pos.y;
    wrapModeButton.pos.x = rotationAngleDisplayButton.pos.x + rotationAngleDisplayButton.scale.x + wrapModeButton.scale.x;

    // Scaling the texture displaying button according to the selected texture
    textureDisplayingButton.scale = orgScale;
    textureDisplayingButtonIOutline.scale = orgScale + glm::vec2(0.2f,0.2f * (Settings::videoScale()->x / Settings::videoScale()->y));
}

void TextureField::render2DTextureField(                            
                                                Timer& timer, 
                                                Painter& painter, 
                                                std::vector<TextureField>& srcVector, 
                                                int& srcVectorI, 
                                                bool editMode, 
                                                bool generatingTextureMode, 
                                                bool doMouseTracking
                                            )
{
    if(!editMode)
        this->active = false;
    
    this->position2DElements();

    // Cursor pos in percentage
    glm::vec2 crsPos = *Mouse::cursorPos() / *Settings::videoScale() * 100.f;

    if((getContext()->window.isKeyPressed(LIGIDGL_KEY_R) || getContext()->window.isKeyClicked(LIGIDGL_KEY_R)) && this->active || (rotateBtn.clickState1 || (rotateBtn.hover && *Mouse::LClick()))){
        glm::vec2 direction = glm::normalize(crsPos - glm::vec2(this->pos));
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

    // Update the rotation angle display button text
    rotationAngleDisplayButton.text = "";
    if(rotationAngleDisplayButton.hover)
        rotationAngleDisplayButton.text += "Rotation : ";
    rotationAngleDisplayButton.text += std::to_string((int)this->rotation);

    // Assigning the selected texture to the texture displaying button
    textureDisplayingButton.texture = this->texture;
    textureDisplayingButtonIOutline.color = ColorPalette::themeColor;

    // Rendering the texture
    {
        if(!generatingTextureMode){
            ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 0.4f);
        }
        else
            ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);

        ShaderSystem::buttonShader().setFloat("rotation", this->rotation);
        
        this->textureDisplayingButton.render(timer, editMode); 
        
        ShaderSystem::buttonShader().setFloat("rotation", 0.f);
        ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);
    }

    if(*Mouse::LClick()){
        if(this->textureDisplayingButton.hover && this->active && !this->rotateBtn.hover){
            registerTextureFieldAction("Texture field moved", srcVector);
        }

        this->active = this->is2DModeHovered();
    }

    // Rendering edit mode elements
    if(this->active && !generatingTextureMode && editMode){
        
        {
            // Render outline
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
        this->wrapModeButton.render(timer, doMouseTracking);
        this->rotateBtn.render(timer, doMouseTracking);
        
        if(rotateBtn.clickState1){
            VectorStroke stroke = VectorStroke(rotateBtn.pos, *Mouse::cursorPos() / *Settings::videoScale() * 100.f, rotateBtn.pos);
            ShaderSystem::vectoralCurve().use();
            ShaderSystem::vectoralCurve().setMat4("projection", getContext()->ortho_projection); 
        
            ShaderSystem::vectoralCurve().setVec3("pos", glm::vec3(Settings::videoScale()->x/2.f, Settings::videoScale()->y/2.f, 0.9f));
            ShaderSystem::vectoralCurve().setVec2("scale", glm::vec2(Settings::videoScale()->x/2.f, Settings::videoScale()->y/2.f));

            ShaderSystem::vectoralCurve().setVec2("direction", stroke.offsetPoint.pos);

            ShaderSystem::vectoralCurve().setVec2("startPos", stroke.startPoint.pos);
            ShaderSystem::vectoralCurve().setVec2("destPos", stroke.endPoint.pos);

            ShaderSystem::vectoralCurve().setVec2("percScale", *Settings::videoScale());
            ShaderSystem::vectoralCurve().setInt("lineCapturingMode", 0);

            ShaderSystem::vectoralCurve().setFloat("EDGE", 0.0005f);

            LigidGL::makeDrawCall(GL_TRIANGLES, 0 , 6, "Texture field : Rotation button : Drawing line to cursor");

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
        srcVector.erase(srcVector.begin() + srcVectorI);
        srcVectorI--;
    }
    else if(this->scaleToTextureResolutionButton.clicked){
        registerTextureFieldAction("Texture field scaled to the texture resolution", srcVector);
        glm::vec2 prevScale = scale;
        twoDScaleAccordingToTextureRes();
        pos.x -= prevScale.x -scale.x; 
    }
    else if(this->flipHorizontalButton.clicked){
        registerTextureFieldAction("Texture field flipped horizontally", srcVector);
        this->texture.flipTexture(true, false);
        Settings::defaultFramebuffer()->FBO.bind();
        flippedH = !flippedH;
    }
    else if(this->flipVerticalButton.clicked){
        registerTextureFieldAction("Texture field flipped vertically", srcVector);
        this->texture.flipTexture(false, true);
        Settings::defaultFramebuffer()->FBO.bind();
        flippedV = !flippedV;
    }
    else if(this->cancelRotationButton.clicked){
        registerTextureFieldAction("Texture field rotation set to 0", srcVector);
        this->rotation = 0.f;
    }
    else if(this->wrapModeButton.clicked){
        registerTextureFieldAction("Texture field - Wrap", srcVector);
        this->wrapMode = true;
    }
}





















bool TextureField::is2DModeHovered(){
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
            this->wrapModeButton.hover ||
            this->rotationAngleDisplayButton.hover;
}

static glm::vec2 rotate2D(glm::vec2 v, float angleDegrees) {
    float angleRadians = glm::radians(angleDegrees);
    float cosA = cos(angleRadians);
    float sinA = sin(angleRadians);

    glm::mat2 rotationMatrix = glm::mat2(cosA, -sinA, sinA, cosA);

    return rotationMatrix * v;
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
    
    if(RB && !textureField_alreadyInteracted){
        textureField_alreadyInteracted = true;
        crsrOffset /= 1.75;
        pos.x += crsrOffset.x;
        pos.y += crsrOffset.y;
        scale.x += crsrOffset.x;
        scale.y += crsrOffset.y;
    }
    
    else if(LB && !textureField_alreadyInteracted){
        textureField_alreadyInteracted = true;
        crsrOffset /= 1.75;
        pos.x += crsrOffset.x;
        pos.y += crsrOffset.y;
        scale.x -= crsrOffset.x;
        scale.y += crsrOffset.y;
    }
    else if(RT && !textureField_alreadyInteracted){
        textureField_alreadyInteracted = true;
        crsrOffset /= 1.75;
        pos.x += crsrOffset.x;
        pos.y += crsrOffset.y;
        scale.x += crsrOffset.x;
        scale.y -= crsrOffset.y;
    }
    
    else if(LT && !textureField_alreadyInteracted){
        textureField_alreadyInteracted = true;
        crsrOffset /= 1.75;
        pos.x += crsrOffset.x;
        pos.y += crsrOffset.y;
        scale.x -= crsrOffset.x;
        scale.y -= crsrOffset.y;
    }
    
    else if(center && !textureField_alreadyInteracted){
        textureField_alreadyInteracted = true;
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

void TextureField::twoDScaleAccordingToTextureRes(){
    float txtrRatio = (float)this->texture.getResolution().x / (float)this->texture.getResolution().y;
    
    if((float)this->texture.getResolution().x < (float)this->texture.getResolution().y)
        txtrRatio = (float)this->texture.getResolution().y / (float)this->texture.getResolution().x;

    float videoRatio = (float)Settings::videoScale()->x / (float)Settings::videoScale()->y;
    
    if((float)this->texture.getResolution().x > (float)this->texture.getResolution().y)
        this->scale = glm::vec2(this->scale.x, this->scale.x / txtrRatio * videoRatio);
    else
        this->scale = glm::vec2(this->scale.y / txtrRatio / videoRatio, this->scale.y);
}