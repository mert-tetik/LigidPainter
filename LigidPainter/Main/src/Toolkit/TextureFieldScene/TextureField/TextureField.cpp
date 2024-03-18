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


TextureField::TextureField(Texture texture){
    this->texture = texture;
    this->scale = glm::vec2(10.f);
    this->twoDScaleAccordingToTextureRes();
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
    rotateBtn.radius = rotateBtn.scale.x / 2.f;
    rotateBtn.color.a /= 2.f;

    deleteButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1),"Delete", appTextures.trashIcon, 1.f,false);
    deleteButton.textColor2 = glm::vec4(1.f);
    scaleToTextureResolutionButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1), "Scale to texture resolution", appTextures.resizingIcon, 1.f,false);
    scaleToTextureResolutionButton.textColor2 = glm::vec4(1.f);
    flipHorizontalButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1), "Flip horizontal", appTextures.flipH, 1.f,false);
    flipHorizontalButton.textColor2 = glm::vec4(1.f);
    flipVerticalButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1), "Flip vertical", appTextures.flipV, 1.f,false);
    flipVerticalButton.textColor2 = glm::vec4(1.f);
    cancelRotationButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1), "Cancel rotation", appTextures.X, 1.f,false);
    cancelRotationButton.textColor2 = glm::vec4(1.f);
    rotationAngleDisplayButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1), "90.0", Texture(), 1.f,false);
    rotationAngleDisplayButton.textColor2 = glm::vec4(1.f);
    wrapModeButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1), "Wrap", Texture(), 1.f,false);
    wrapModeButton.textColor2 = glm::vec4(1.f);
    
    wrap_deleteButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(3,1),"Delete", appTextures.trashIcon, 1.f,false);
    wrap_deleteButton.textColor2 = glm::vec4(1.f);
    wrap_flipHorizontalButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(3,1), "Flip X", appTextures.flipH, 1.f,false);
    wrap_flipHorizontalButton.textColor2 = glm::vec4(1.f);
    wrap_flipVerticalButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(3,1), "Flip Y", appTextures.flipV, 1.f,false);
    wrap_flipVerticalButton.textColor2 = glm::vec4(1.f);
    wrap_unwrapModeButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(3,1), "Unwrap", Texture(), 1.f,false);
    wrap_unwrapModeButton.textColor2 = glm::vec4(1.f);
    wrap_detailModeButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(3,1), "Detailed", Texture(), 1.f, true);
    wrap_detailModeButton.textColor2 = glm::vec4(1.f);
    
    textureDisplayingButton = Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),"", Texture(), 1.f,false);
    textureDisplayingButton.stretchTexture = true;
    textureDisplayingButton.color = glm::vec4(0.f);
    textureDisplayingButton.color2 = glm::vec4(0.f);
    textureDisplayingButton.outline = false;

    textureDisplayingButtonIOutline = Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),"", Texture(), 1.f,false);
}

/* 
    This global flag is used to prevent collision between multiple texture fields
    Set to true inside of the Render2DMode.cpp or RenderWrapMode.cpp
    Set to false in the renderGUI before rendering the texture fields
*/
bool textureField_alreadyInteracted = false;

void TextureField::render(
                            Timer& timer, 
                            Painter& painter, 
                            std::vector<TextureField>& srcVector, 
                            int& srcVectorI, 
                            bool editMode, 
                            bool generatingTextureMode, 
                            bool doMouseTracking
                        )
{
    if(this->wrapMode){
        this->renderWrappedTextureField(timer, painter, srcVector, srcVectorI, editMode, generatingTextureMode, doMouseTracking);
    }

    else{
        this->render2DTextureField(timer, painter, srcVector, srcVectorI, editMode, generatingTextureMode, doMouseTracking);
    }
}