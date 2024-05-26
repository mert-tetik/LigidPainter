
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 (c) 2024 Mert Tetik. All rights reserved.

 Official GitHub Link : https://github.com/mert-tetik/LigidPainter
 Official Web Page : https:ligidtools.com/ligidpainter

 ---------------------------------------------------------------------------

    Display settings dialog.
    (Mostly manipulates the skybox )

 */
#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"

#include "3D/ThreeD.hpp"

#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "UTIL/Threads/Threads.hpp"
#include "UTIL/GL/GL.hpp"

#include <string>
#include <iostream>
#include <vector>

// Defined in the RenderPanel.cpp
extern bool updateThePreRenderedPanels;

static void action_RESIZE(Texture* receivedTexture, Texture* destTxtr, std::vector<Element> resizeElements);
static void action_BLUR(Texture* receivedTexture, Texture* destTxtr, std::vector<Element> blurElements);
static void action_COLORING(Texture* receivedTexture, Texture* destTxtr, std::vector<Element> coloringElements);
static void action_NORMALMAP(Texture* receivedTexture, Texture* destTxtr, std::vector<Element> normalMapElements);
static void action_DISTORTION(Texture* receivedTexture, Texture* destTxtr, std::vector<Element> distortionElements);
static void action_IMAGE(Texture* receivedTexture, Texture* destTxtr, Texture image);



void TextureEditorDialog::generate_result(Texture* receivedTexture, Texture destTxtr){
    
    if(this->selectedSection == 0){
        action_RESIZE(receivedTexture, &destTxtr, this->resizeElements);
    }
    else if(this->selectedSection == 1){
        action_BLUR(receivedTexture, &destTxtr, this->bluringElement);
    }
    else if(this->selectedSection == 2){
        action_COLORING(receivedTexture, &destTxtr, this->coloringElements);
    }
    else if(this->selectedSection == 3){
        action_NORMALMAP(receivedTexture, &destTxtr, this->normalMapElements);
    }
    else if(this->selectedSection == 4){
        action_DISTORTION(receivedTexture, &destTxtr, this->distortionElements);
    }
    else if(this->selectedSection == 5){
        action_IMAGE(receivedTexture, &destTxtr, *receivedTexture);

        if(this->filterBtn.filter.shader.ID){
            this->filterBtn.filter.applyFilter(destTxtr, appTextures.white, appTextures.white);
        }
    }
    else if(this->selectedSection == 6){
        action_IMAGE(receivedTexture, &destTxtr, this->textureBtn.texture);
    }
    
    destTxtr.mix(*receivedTexture, this->maskTextureButton.texture, false, false, true);
}




























static void action_RESIZE(Texture* receivedTexture, Texture* destTxtr, std::vector<Element> resizeElements){
    glm::vec2 txtrRes = receivedTexture->getResolution();
    glm::vec2 displayRes = receivedTexture->getResolution();
    if(resizeElements[5].textBox.text != "" && resizeElements[6].textBox.text != "")
        displayRes = glm::vec2(std::stoi(resizeElements[5].textBox.text), std::stoi(resizeElements[6].textBox.text));


    destTxtr->update((char*)nullptr, displayRes.x, displayRes.y);

    Framebuffer FBO = FBOPOOL::requestFBO(*destTxtr, "");
    glViewport(0, 0, displayRes.x, displayRes.y);

    //Clear the capture frame buffer(displaying texture) with color alpha zero
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);

    glm::vec2 originPoint = glm::vec2(resizeElements[3].rangeBar.value, resizeElements[4].rangeBar.value);

    glm::mat4 projection = glm::ortho(0.f, displayRes.x, displayRes.y, 0.f);
    glm::vec2 scale = glm::vec2(displayRes.x / 2.f, displayRes.y / 2.f);
    glm::vec3 pos = glm::vec3(displayRes.x / 2.f, displayRes.y / 2.f, 0.9f);
    if(resizeElements[0].comboBox.selectedIndex == 1 || resizeElements[0].comboBox.selectedIndex == 2)
        pos = glm::vec3(pos.x + (originPoint.x * displayRes.x), pos.y + (originPoint.y * displayRes.y), pos.z);

    ShaderSystem::txtrEditorResizeShader().use();
    ShaderSystem::txtrEditorResizeShader().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
    ShaderSystem::txtrEditorResizeShader().setVec2("scale", glm::vec2(0.5f));
    ShaderSystem::txtrEditorResizeShader().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.7f));

    if(resizeElements[0].comboBox.selectedIndex == 0)
        txtrRes = displayRes;

    ShaderSystem::txtrEditorResizeShader().setInt("txtr", GL::bindTexture_2D(receivedTexture->ID, "TextureEditorDialog::generate_result 0")); 
    ShaderSystem::txtrEditorResizeShader().setVec2("txtrResolution", txtrRes);
    ShaderSystem::txtrEditorResizeShader().setVec2("destTxtrResolution", displayRes);
    
    ShaderSystem::txtrEditorResizeShader().setInt("wrapingIndex", resizeElements[0].comboBox.selectedIndex);
    ShaderSystem::txtrEditorResizeShader().setVec3("wrapingColor", resizeElements[1].button.color);
    ShaderSystem::txtrEditorResizeShader().setVec2("originPoint", originPoint);

    unsigned int wrapParam = GL_REPEAT;

    if(resizeElements[0].comboBox.selectedIndex == 1){
        wrapParam = GL_CLAMP_TO_BORDER;
        GLfloat borderColor[] = { 0.f, 0.f, 0.f, 0.f };  // Replace r, g, b, a with the desired color values
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }
    if(resizeElements[0].comboBox.selectedIndex == 2){
        wrapParam = GL_CLAMP_TO_BORDER;
        GLfloat borderColor[] = { resizeElements[1].button.color.r, resizeElements[1].button.color.g, resizeElements[1].button.color.b, 1. };  // Replace r, g, b, a with the desired color values
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }
    if(resizeElements[0].comboBox.selectedIndex == 3)
        wrapParam = GL_REPEAT;
    else if(resizeElements[0].comboBox.selectedIndex == 4)
        wrapParam = GL_MIRRORED_REPEAT;
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapParam);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapParam);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapParam);

    getBox()->draw("Image editor dialog : Selected section 0");

    GL::releaseBoundTextures("TextureEditorDialog::generate_result 0");
    ShaderUTIL::release_bound_shader();

    FBOPOOL::releaseFBO(FBO);

}

static void action_BLUR(Texture* receivedTexture, Texture* destTxtr, std::vector<Element> blurElements)
{
    Framebuffer FBO = FBOPOOL::requestFBO(*destTxtr, "");
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);

    ShaderSystem::txtrEditorBlurShader().use();
    ShaderSystem::txtrEditorBlurShader().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
    ShaderSystem::txtrEditorBlurShader().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.7f));
    ShaderSystem::txtrEditorBlurShader().setVec2("scale", glm::vec2(0.5f));

    ShaderSystem::txtrEditorBlurShader().setVec2("txtrResolution", receivedTexture->getResolution());
    ShaderSystem::txtrEditorBlurShader().setInt("txtr", GL::bindTexture_2D(receivedTexture->ID, "TextureEditorDialog::generate_result 1")); 
    ShaderSystem::txtrEditorBlurShader().setInt("blurIndex", blurElements[0].comboBox.selectedIndex);
    ShaderSystem::txtrEditorBlurShader().setFloat("directionalDirection", blurElements[1].rangeBar.value);
    ShaderSystem::txtrEditorBlurShader().setVec2("radialPos", glm::vec2(blurElements[2].rangeBar.value, blurElements[3].rangeBar.value));
    ShaderSystem::txtrEditorBlurShader().setFloat("strength", blurElements[4].rangeBar.value);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

    getBox()->draw("Image editor dialog : Selected section 1");

    GL::releaseBoundTextures("TextureEditorDialog::generate_result 1");
    ShaderUTIL::release_bound_shader();
    FBOPOOL::releaseFBO(FBO);
}

static void action_COLORING(Texture* receivedTexture, Texture* destTxtr, std::vector<Element> coloringElements)
{
    Framebuffer FBO = FBOPOOL::requestFBO(*destTxtr, "");
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);

    ShaderSystem::txtrEditorColoringShader().use();
    ShaderSystem::txtrEditorColoringShader().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
    ShaderSystem::txtrEditorColoringShader().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.7f));
    ShaderSystem::txtrEditorColoringShader().setVec2("scale", glm::vec2(0.5f));

    ShaderSystem::txtrEditorColoringShader().setVec2("txtrResolution", receivedTexture->getResolution());
    ShaderSystem::txtrEditorColoringShader().setInt("txtr", GL::bindTexture_2D(receivedTexture->ID, "TextureEditorDialog::generate_result 2")); 

    ShaderSystem::txtrEditorColoringShader().setInt("coloringIndex", coloringElements[0].comboBox.selectedIndex);
    ShaderSystem::txtrEditorColoringShader().setVec3("hsv", glm::vec3(coloringElements[1].rangeBar.value, coloringElements[2].rangeBar.value, coloringElements[3].rangeBar.value));
    ShaderSystem::txtrEditorColoringShader().setInt("singleColorIndex", coloringElements[4].comboBox.selectedIndex);
    ShaderSystem::txtrEditorColoringShader().setVec3("singleColor", coloringElements[5].button.color);
    ShaderSystem::txtrEditorColoringShader().setFloat("brightness", coloringElements[6].rangeBar.value);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

    getBox()->draw("Image editor dialog : Selected section 2");

    GL::releaseBoundTextures("TextureEditorDialog::generate_result 2");
    ShaderUTIL::release_bound_shader();
    FBOPOOL::releaseFBO(FBO);
}

static void action_NORMALMAP(Texture* receivedTexture, Texture* destTxtr, std::vector<Element> normalMapElements){
    Framebuffer FBO = FBOPOOL::requestFBO(*destTxtr, "");
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);

    ShaderSystem::txtrEditorNormalMapShader().use();
    ShaderSystem::txtrEditorNormalMapShader().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
    ShaderSystem::txtrEditorNormalMapShader().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.7f));
    ShaderSystem::txtrEditorNormalMapShader().setVec2("scale", glm::vec2(0.5f));

    ShaderSystem::txtrEditorNormalMapShader().setVec2("txtrResolution", receivedTexture->getResolution());
    ShaderSystem::txtrEditorNormalMapShader().setInt("txtr", GL::bindTexture_2D(receivedTexture->ID, "TextureEditorDialog::generate_result 3")); 
    ShaderSystem::txtrEditorNormalMapShader().setInt("mode", normalMapElements[0].comboBox.selectedIndex);
    ShaderSystem::txtrEditorNormalMapShader().setInt("grayScale", normalMapElements[1].comboBox.selectedIndex);
    ShaderSystem::txtrEditorNormalMapShader().setFloat("strength", normalMapElements[2].rangeBar.value);
    ShaderSystem::txtrEditorNormalMapShader().setFloat("blurVal", normalMapElements[3].rangeBar.value);
    ShaderSystem::txtrEditorNormalMapShader().setFloat("rot", normalMapElements[4].rangeBar.value);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

    getBox()->draw("Image editor dialog : Selected section 3");
    
    GL::releaseBoundTextures("TextureEditorDialog::generate_result 3");
    ShaderUTIL::release_bound_shader();

    FBOPOOL::releaseFBO(FBO);
}

static void action_DISTORTION(Texture* receivedTexture, Texture* destTxtr, std::vector<Element> distortionElements){
    Framebuffer FBO = FBOPOOL::requestFBO(*destTxtr, "");
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);

    ShaderSystem::txtrEditorDistortionShader().use();
    ShaderSystem::txtrEditorDistortionShader().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
    ShaderSystem::txtrEditorDistortionShader().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.7f));
    ShaderSystem::txtrEditorDistortionShader().setVec2("scale", glm::vec2(0.5f));

    ShaderSystem::txtrEditorDistortionShader().setVec2("txtrResolution", receivedTexture->getResolution());
    ShaderSystem::txtrEditorDistortionShader().setInt("txtr", GL::bindTexture_2D(receivedTexture->ID, "TextureEditorDialog::generate_result 4")); 
    ShaderSystem::txtrEditorDistortionShader().setInt("distortionTxtr", GL::bindTexture_2D(distortionElements[8].button.texture.ID, "TextureEditorDialog::generate_result 4")); 
    ShaderSystem::txtrEditorDistortionShader().setInt("distortionIndex", distortionElements[0].comboBox.selectedIndex);
    ShaderSystem::txtrEditorDistortionShader().setFloat("fbmRoughness", distortionElements[1].rangeBar.value);
    ShaderSystem::txtrEditorDistortionShader().setInt("fbmOctaves", distortionElements[2].rangeBar.value);
    ShaderSystem::txtrEditorDistortionShader().setFloat("xStrength", distortionElements[3].rangeBar.value);
    ShaderSystem::txtrEditorDistortionShader().setFloat("yStrength", distortionElements[4].rangeBar.value);
    ShaderSystem::txtrEditorDistortionShader().setFloat("effectRadius", distortionElements[5].rangeBar.value);
    ShaderSystem::txtrEditorDistortionShader().setFloat("effectAngle", distortionElements[6].rangeBar.value);
    ShaderSystem::txtrEditorDistortionShader().setFloat("rainScale", distortionElements[7].rangeBar.value);
    ShaderSystem::txtrEditorDistortionShader().setFloat("strength", distortionElements[9].rangeBar.value);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

    getBox()->draw("Image editor dialog : Selected section 4");
    
    GL::releaseBoundTextures("TextureEditorDialog::generate_result 4");
    ShaderUTIL::release_bound_shader();

    FBOPOOL::releaseFBO(FBO);
}

static void action_IMAGE(Texture* receivedTexture, Texture* destTxtr, Texture image){
    Framebuffer FBO = FBOPOOL::requestFBO(*destTxtr, "");
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);

    ShaderSystem::textureRenderingShader().use();
    ShaderSystem::textureRenderingShader().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
    ShaderSystem::textureRenderingShader().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.7f));
    ShaderSystem::textureRenderingShader().setVec2("scale", glm::vec2(0.5f));
    ShaderSystem::textureRenderingShader().setInt("txtr", GL::bindTexture_2D(image.ID, "action_IMAGE"));
    ShaderSystem::textureRenderingShader().setFloat("opacity", 1.f);
    ShaderSystem::textureRenderingShader().setFloat("rotation", 0.f);
    ShaderSystem::textureRenderingShader().setFloat("depthToleranceValue", 0);

    getBox()->draw("action_IMAGE");

    GL::releaseBoundTextures("action_IMAGE");
    ShaderUTIL::release_bound_shader();

    FBOPOOL::releaseFBO(FBO);
}