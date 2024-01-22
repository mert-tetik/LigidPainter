/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Textures of the Texture Modifier's texture channel buttons are deleted (indexed specially)
    Remove the filter button is indexed specially

*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "GUI/Elements/Elements.hpp"
#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"
#include "ShaderSystem/Shader.hpp"
#include "SettingsSystem/Settings.hpp"

//0 = albedo
//1 = roughness
//2 = metallic 
//3 = normal map
//4 = height map
//5 = ambient Occlusion

static void channelPrep(Material &material, Mesh &mesh, int& textureResolution, int& curModI, glm::mat4& perspective, glm::mat4& view, int& channelI, Framebuffer& FBO, Texture& currentTexture, Texture& previousTexture){
    glDisable(GL_DEPTH_TEST);

    //Get the channel's texture from material
    
    if(channelI == 0){
        currentTexture = mesh.albedo;
    }
    if(channelI == 1){
        currentTexture = mesh.roughness;
    }
    if(channelI == 2){
        currentTexture = mesh.metallic;
    }
    if(channelI == 3){
        currentTexture = mesh.normalMap;
    }
    if(channelI == 4){
        currentTexture = mesh.heightMap;
    }
    if(channelI == 5){
        currentTexture = mesh.ambientOcclusion;
    }

    glm::ivec2 prevCurrentTextureRes = currentTexture.getResolution();

    /* ! Binds another framebuffer ! */
    if(!previousTexture.ID)
        previousTexture = Texture(nullptr, prevCurrentTextureRes.x, prevCurrentTextureRes.y);
    else
        previousTexture.update(nullptr, prevCurrentTextureRes.x, prevCurrentTextureRes.y);
    
    currentTexture.duplicateTextureSub(previousTexture);

    currentTexture.update(nullptr, textureResolution, textureResolution);    
    
    //That framebuffer will be used to get the results of the shader 
    FBO = Framebuffer(currentTexture, GL_TEXTURE_2D);

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
}

Texture textureCopy_bluring;
static void blurTheTexture(unsigned int& txtr, Mesh& mesh, int textureResolution, float blurVal, int algorithmI){
    Texture textureObject = Texture(txtr);
    
    if(!textureCopy_bluring.ID)
        textureCopy_bluring = Texture(nullptr, textureResolution, textureResolution);
    
    textureCopy_bluring.update(nullptr, textureResolution, textureResolution);
    
    textureObject.duplicateTextureSub(textureCopy_bluring);

    Framebuffer FBO = Framebuffer(txtr, GL_TEXTURE_2D);
    glViewport(0, 0, textureResolution, textureResolution);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    getBox()->bindBuffers();
    
    Shader blurShader;
    if(algorithmI == 0)
        blurShader = ShaderSystem::bluringShader();    
    else if(algorithmI == 1)
        blurShader = ShaderSystem::twoPassBlur();    

    glm::mat4 projection = glm::ortho(0.f, (float)textureResolution, (float)textureResolution, 0.f); 
    blurShader.use();
    blurShader.setInt("txtr", 0);
    blurShader.setInt("uvMask", 1);
    blurShader.setVec2("txtrRes", glm::vec2(textureResolution));
    blurShader.setMat4("projection"  ,       projection);
    blurShader.setMat4("projectedPosProjection"  ,       projection);
    blurShader.setVec3("pos"         ,       glm::vec3((float)textureResolution / 2.f, (float)textureResolution / 2.f, 0.9f));
    blurShader.setVec2("scale"       ,       glm::vec2((float)textureResolution / 2.f));
    blurShader.setFloat("blurVal"     ,     blurVal);
    if(algorithmI == 1)
        blurShader.setInt("horizontal", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureCopy_bluring.ID);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mesh.uvMask.ID);

    glDrawArrays(GL_TRIANGLES, 0 , 6);

    if(algorithmI == 1){
        textureObject.duplicateTextureSub(textureCopy_bluring);
        
        FBO.bind();
        glViewport(0, 0, textureResolution, textureResolution);
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        getBox()->bindBuffers();
        
        blurShader.setInt("horizontal", 1);
        glDrawArrays(GL_TRIANGLES, 0 , 6);
    }

    Settings::defaultFramebuffer()->FBO.bind();
    
    FBO.deleteBuffers(false, false);
}

static Framebuffer aoFBO;
static Texture aoDepthTxtr;
static Framebuffer aoDepthFBO;
static Texture aoTxtrDup;

static void genAmbientOcclusion(
                                    Texture& aoTxtr, 
                                    Mesh& mesh, 
                                    Texture meshMask, 
                                    Texture selectedObjectPrimitivesTxtr, 
                                    Model& model,
                                    bool usePrevAO,
                                    bool singleMesh,
                                    float aoOffset,
                                    float smoothness,
                                    bool useSecondBlurAlgorithm,
                                    bool xP,
                                    bool xN,
                                    bool yP,
                                    bool yN,
                                    bool zP,
                                    bool zN
                                )
{ 

    if(!model.meshes.size())
        return;

    glm::ivec2 aoTxtrRes = aoTxtr.getResolution();
    
    if(!aoFBO.ID){
        aoFBO = Framebuffer(aoTxtr, GL_TEXTURE_2D);
        aoDepthTxtr = Texture(nullptr, 1024, 1024, GL_NEAREST, GL_RGBA, GL_RGBA32F);
        aoTxtrDup = Texture(nullptr, aoTxtrRes.x, aoTxtrRes.y);
        aoDepthFBO = Framebuffer(aoDepthTxtr, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, glm::ivec2(1024)));
    }

    aoTxtrDup.update(nullptr, aoTxtrRes.x, aoTxtrRes.y);

    if(usePrevAO)
        aoTxtr.duplicateTextureSub(aoTxtrDup);

    aoFBO.bind();
    aoFBO.setColorBuffer(aoTxtr, GL_TEXTURE_2D);

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, aoTxtrRes.x, aoTxtrRes.y);

    //viewport
    
    aoFBO.bind();

    for (size_t i = 0; i < 6; i++)
    {
        if(i == 0 && !xP)
            i++;
        if(i == 1 && !xN)
            i++;
        if(i == 2 && !yP)
            i++;
        if(i == 3 && !yN)
            i++;
        if(i == 4 && !zP)
            i++;
        if(i == 5 && !zN)
            break;
        
        aoTxtr.duplicateTextureSub(aoTxtrDup);
        
        glm::vec3 camPos = glm::vec3(5.f, 0.f, 0.f);

        if(i == 1)
            camPos = glm::vec3(-5.f, 0.f, 0.f);
        if(i == 2)
            camPos = glm::vec3(1.f, 5.f, 1.f);
        if(i == 3)
            camPos = glm::vec3(1.f, -5.f, 1.f);
        if(i == 4)
            camPos = glm::vec3(0.f, 0.f, 5.f);
        if(i == 5)
            camPos = glm::vec3(0.f, 0.f, -5.f);

        glm::mat4 view = glm::lookAt(camPos, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
        glm::mat4 projection = glm::perspective(glm::radians(35.f), 1.f, 0.1f, 100.f);
        //glm::mat4 projection = glm::ortho(0.f,5.f,0.f,5.f);

        aoDepthFBO.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, 1024, 1024);
        
        ShaderSystem::depth3D().use();
        ShaderSystem::depth3D().setMat4("view", view);
        ShaderSystem::depth3D().setMat4("projection", projection);
        ShaderSystem::depth3D().setMat4("modelMatrix", glm::mat4(1.f));

        ShaderSystem::depth3D().setInt("usingMeshSelection", false);
        ShaderSystem::depth3D().setInt("hideUnselected", false);
        ShaderSystem::depth3D().setInt("selectedPrimitiveIDS", 0);
        ShaderSystem::depth3D().setInt("meshMask", 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, selectedObjectPrimitivesTxtr.ID);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, meshMask.ID);

        if(singleMesh)
            mesh.Draw(false);
        else
            model.Draw();
        
        aoFBO.bind();
        glViewport(0, 0, aoTxtrRes.x, aoTxtrRes.y);
        
        ShaderSystem::AOGen().use();
        ShaderSystem::AOGen().setMat4("orthoProjection", glm::ortho(0.f, 1.f, 0.f, 1.f)); 
        ShaderSystem::AOGen().setMat4("perspectiveProjection", projection); 
        ShaderSystem::AOGen().setMat4("view", view); 
        
        ShaderSystem::AOGen().setInt("useMeshMask", selectedObjectPrimitivesTxtr.ID != 0);
        ShaderSystem::AOGen().setInt("selectedPrimitiveIDS", 2);
        ShaderSystem::AOGen().setInt("meshMask", 3);
        ShaderSystem::AOGen().setInt("primitiveCount", mesh.indices.size() / 3);
        
        ShaderSystem::AOGen().setInt("depthTexture", 0); 
        ShaderSystem::AOGen().setInt("srcTxtr", 1);
        ShaderSystem::AOGen().setFloat("aoOffset", aoOffset);
        

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, aoDepthTxtr.ID);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, aoTxtrDup.ID);
        
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, selectedObjectPrimitivesTxtr.ID);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, meshMask.ID);

        mesh.Draw(false);
    }
    
    if(smoothness > 0.1f)
        blurTheTexture(aoTxtr.ID, mesh, aoTxtrRes.x, smoothness, useSecondBlurAlgorithm);
    
    aoTxtr.removeSeams(mesh, aoTxtrRes);

    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();
}

static glm::vec2 getDirectionVector(float rotation) {
  // Convert rotation from degrees to radians
  float radians = glm::radians(rotation);

  // Calculate the x and y components of the direction vector
  float x = glm::cos(radians);
  float y = glm::sin(radians);

  return glm::vec2(x, y);
}

static void setUniforms(
                            Shader modifierShader, 
                            Material material, 
                            int channelI, 
                            int curModI, 
                            unsigned int maskTexture_procedural,
                            Texture previousTexture,
                            Texture currentTexture,
                            Texture prevDepthTexture,
                            unsigned int textureModifierSelectedTexture_procedural,
                            Mesh& mesh,
                            Texture meshMask,
                            Texture selectedObjectPrimitivesTxtr,
                            bool noPrevTxtrMode
                        )
{

    /* Channel Properties */
    modifierShader.setInt("state", channelI); //Set the state
    modifierShader.setInt("mask", 0); //Set the mask texture slot
    modifierShader.setInt("previousTxtr", 1); //Set the previous texture slot
    modifierShader.setInt( "depthTxtr" , 2); //Set the previous height map texture
    
    if(material.materialModifiers[curModI].modifierIndex != SOLID_MATERIAL_MODIFIER)
        modifierShader.setFloat( "opacity" , material.materialModifiers[curModI].sections[material.materialModifiers[curModI].sections.size()-4].elements[channelI].rangeBar.value);
    else
        modifierShader.setFloat( "opacity" , material.materialModifiers[curModI].sections[0].elements[channelI * 2 + 1].rangeBar.value);

    modifierShader.setFloat( "depthValue" , material.materialModifiers[curModI].sections[material.materialModifiers[curModI].sections.size()-3].elements[0].rangeBar.value);
    modifierShader.setInt("selectedPrimitiveIDS" , 3); //Set the previous height map texture
    modifierShader.setInt("meshMask" , 4); //Set the previous height map texture
    modifierShader.setInt("primitiveCount" , mesh.indices.size() / 3); //Set the previous height map texture
    modifierShader.setInt("useMeshMask" , selectedObjectPrimitivesTxtr.ID != 0); //Set the previous height map texture

    // Bind the mask texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, maskTexture_procedural);

    //Bind the previous texture
    glActiveTexture(GL_TEXTURE1);
    if(curModI != material.materialModifiers.size()-1 && !noPrevTxtrMode)
        glBindTexture(GL_TEXTURE_2D, previousTexture.ID);
    else
        glBindTexture(GL_TEXTURE_2D, appTextures.black.ID);
    
    //Bind the previous height map texture
    glActiveTexture(GL_TEXTURE2);
    if(curModI != material.materialModifiers.size()-1)
        glBindTexture(GL_TEXTURE_2D, prevDepthTexture.ID);
    else
        glBindTexture(GL_TEXTURE_2D, appTextures.black.ID);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, selectedObjectPrimitivesTxtr.ID);
    
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, meshMask.ID);


    if(material.materialModifiers[curModI].modifierIndex == TEXTURE_MATERIAL_MODIFIER){
        /* Selected texture */
        modifierShader.setInt("theTexture", 5); 
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, textureModifierSelectedTexture_procedural);
    }
    else if(material.materialModifiers[curModI].modifierIndex == DUST_MATERIAL_MODIFIER){
        /* Noise */
        modifierShader.setFloat("size", material.materialModifiers[curModI].sections[0].elements[0].rangeBar.value); 
        modifierShader.setFloat("offsetIntensity", material.materialModifiers[curModI].sections[0].elements[1].rangeBar.value); 
        modifierShader.setFloat("rotation", material.materialModifiers[curModI].sections[0].elements[2].rangeBar.value); 
        modifierShader.setFloat("brightness", material.materialModifiers[curModI].sections[0].elements[3].rangeBar.value); 
        /* Blur */
        modifierShader.setFloat("blurIntensity", material.materialModifiers[curModI].sections[1].elements[0].rangeBar.value);
        modifierShader.setVec2("blurDirection", getDirectionVector(material.materialModifiers[curModI].sections[1].elements[1].rangeBar.value));
        /* Scratches */
        modifierShader.setFloat("scratchesWavyness", material.materialModifiers[curModI].sections[2].elements[0].rangeBar.value); 
        modifierShader.setVec2("scratchesScale", glm::vec2(material.materialModifiers[curModI].sections[2].elements[1].rangeBar.value));
        modifierShader.setVec2("scratchesBaseFrequency", glm::vec2(material.materialModifiers[curModI].sections[2].elements[2].rangeBar.value));
        modifierShader.setVec2("scratchesFrequencyStep", glm::vec2(material.materialModifiers[curModI].sections[2].elements[3].rangeBar.value));
        /* Droplets */
        modifierShader.setFloat("dropletsCount", material.materialModifiers[curModI].sections[3].elements[0].rangeBar.value); 
        modifierShader.setFloat("dropletsOpacityJitter", material.materialModifiers[curModI].sections[3].elements[1].rangeBar.value);
        modifierShader.setFloat("dropletsSize", material.materialModifiers[curModI].sections[3].elements[2].rangeBar.value);
        /* Colors */
        modifierShader.setVec3("color1", material.materialModifiers[curModI].sections[4].elements[0].button.color); 
        modifierShader.setVec3("color2", material.materialModifiers[curModI].sections[4].elements[1].button.color);
        modifierShader.setVec3("color3", material.materialModifiers[curModI].sections[4].elements[2].button.color);
        /* Element property */
        modifierShader.setFloat("wetness", material.materialModifiers[curModI].sections[5].elements[0].rangeBar.value);
        modifierShader.setFloat("metallic", material.materialModifiers[curModI].sections[5].elements[1].rangeBar.value);
        modifierShader.setFloat("height", material.materialModifiers[curModI].sections[5].elements[2].rangeBar.value);
    }
    else if(material.materialModifiers[curModI].modifierIndex == SOLID_MATERIAL_MODIFIER){
        /* Solid Color Value */
        modifierShader.setVec3( "value" , glm::vec3(
                                                material.materialModifiers[curModI].sections[0].elements[channelI * 2].button.color.r, 
                                                material.materialModifiers[curModI].sections[0].elements[channelI * 2].button.color.g, 
                                                material.materialModifiers[curModI].sections[0].elements[channelI * 2].button.color.b
                                            )); //Set the channel color value
    }
    else if(material.materialModifiers[curModI].modifierIndex == ASPHALT_MATERIAL_MODIFIER){
        /* Colors */
        modifierShader.setVec3("asphaltColor", material.materialModifiers[curModI].sections[0].elements[0].button.color);
        modifierShader.setVec3("asphaltColor2", material.materialModifiers[curModI].sections[0].elements[1].button.color);
        /* Dirt */
        modifierShader.setFloat("dirtScale", material.materialModifiers[curModI].sections[1].elements[0].rangeBar.value);
        modifierShader.setFloat("dirtStrength", material.materialModifiers[curModI].sections[1].elements[1].rangeBar.value);
        /* 2nd Color */
        modifierShader.setFloat("colorNoiseScale", material.materialModifiers[curModI].sections[2].elements[0].rangeBar.value);
        modifierShader.setFloat("colorNoiseStrength", material.materialModifiers[curModI].sections[2].elements[1].rangeBar.value);
        /* Noise */ 
        modifierShader.setFloat("noiseStrength", material.materialModifiers[curModI].sections[3].elements[0].rangeBar.value);
        /* Element property */
        modifierShader.setFloat("wetness", material.materialModifiers[curModI].sections[4].elements[0].rangeBar.value);
        modifierShader.setFloat("metallic", material.materialModifiers[curModI].sections[4].elements[1].rangeBar.value);
        modifierShader.setFloat("height", material.materialModifiers[curModI].sections[4].elements[2].rangeBar.value);
        /* Perlin Noise Properties*/
        modifierShader.setInt("firstOctave", material.materialModifiers[curModI].sections[5].elements[0].rangeBar.value);
        modifierShader.setInt("octaves", material.materialModifiers[curModI].sections[5].elements[1].rangeBar.value); 
        modifierShader.setFloat("persistence", material.materialModifiers[curModI].sections[5].elements[2].rangeBar.value); 
    }
    else if(material.materialModifiers[curModI].modifierIndex == LIQUID_MATERIAL_MODIFIER){
        /* Scaling */ 
        modifierShader.setFloat("scale", material.materialModifiers[curModI].sections[0].elements[0].rangeBar.value);
        modifierShader.setFloat("yScale", material.materialModifiers[curModI].sections[0].elements[1].rangeBar.value);
        /* Noise */
        modifierShader.setInt("octaves", material.materialModifiers[curModI].sections[1].elements[0].rangeBar.value);
        modifierShader.setFloat("fbmXOffset", material.materialModifiers[curModI].sections[1].elements[1].rangeBar.value);
        modifierShader.setFloat("fbmYOffset", material.materialModifiers[curModI].sections[1].elements[2].rangeBar.value);
        modifierShader.setFloat("fbmXScale", material.materialModifiers[curModI].sections[1].elements[3].rangeBar.value);
        modifierShader.setFloat("fbmYScale", material.materialModifiers[curModI].sections[1].elements[4].rangeBar.value);
        modifierShader.setFloat("roughness", material.materialModifiers[curModI].sections[1].elements[5].rangeBar.value);
        modifierShader.setFloat("frequencyAcceleration", material.materialModifiers[curModI].sections[1].elements[6].rangeBar.value);
        modifierShader.setFloat("fbmTotal", material.materialModifiers[curModI].sections[1].elements[7].rangeBar.value);
        modifierShader.setFloat("seed", material.materialModifiers[curModI].sections[1].elements[8].rangeBar.value);
        /* General */
        modifierShader.setFloat("noiseInfluence", material.materialModifiers[curModI].sections[2].elements[0].rangeBar.value);
        modifierShader.setFloat("positionInfluence", material.materialModifiers[curModI].sections[2].elements[1].rangeBar.value);
        /* Colors */
        modifierShader.setVec3("color1", material.materialModifiers[curModI].sections[3].elements[0].button.color);
        modifierShader.setVec3("color2", material.materialModifiers[curModI].sections[3].elements[1].button.color);
        modifierShader.setVec3("color3", material.materialModifiers[curModI].sections[3].elements[2].button.color);
        modifierShader.setVec3("color4", material.materialModifiers[curModI].sections[3].elements[3].button.color);
        /* Element property */
        modifierShader.setFloat("wetness", material.materialModifiers[curModI].sections[4].elements[0].rangeBar.value);
        modifierShader.setFloat("metallic", material.materialModifiers[curModI].sections[4].elements[1].rangeBar.value);
        modifierShader.setFloat("height", material.materialModifiers[curModI].sections[4].elements[2].rangeBar.value);
        modifierShader.setFloat("ambientOcclusion", material.materialModifiers[curModI].sections[4].elements[3].rangeBar.value);
    }
    else if(material.materialModifiers[curModI].modifierIndex == WOODEN_MATERIAL_MODIFIER){
        /* Colors */
        modifierShader.setVec3("woodColor1", material.materialModifiers[curModI].sections[0].elements[0].button.color);
        modifierShader.setVec3("woodColor2", material.materialModifiers[curModI].sections[0].elements[1].button.color);
        modifierShader.setVec3("woodColor3", material.materialModifiers[curModI].sections[0].elements[2].button.color);
        /* Properties */
        modifierShader.setFloat("scale", material.materialModifiers[curModI].sections[1].elements[0].rangeBar.value);
        modifierShader.setFloat("noiseOffset", material.materialModifiers[curModI].sections[1].elements[1].rangeBar.value);
        modifierShader.setFloat("seed", material.materialModifiers[curModI].sections[1].elements[2].rangeBar.value);
        /* Perlin Properties */
        modifierShader.setInt("maxOctaves", material.materialModifiers[curModI].sections[2].elements[0].rangeBar.value); 
        modifierShader.setFloat("persistance", material.materialModifiers[curModI].sections[2].elements[1].rangeBar.value);
        /* Musgrave Properties */
        modifierShader.setFloat("musgraveLacunarity", material.materialModifiers[curModI].sections[3].elements[0].rangeBar.value);
        modifierShader.setFloat("musgraveStrength", material.materialModifiers[curModI].sections[3].elements[1].rangeBar.value);
        modifierShader.setFloat("musgraveNoise", material.materialModifiers[curModI].sections[3].elements[2].rangeBar.value);
        /* Base */
        modifierShader.setFloat("baseNoiseStrength", material.materialModifiers[curModI].sections[4].elements[0].rangeBar.value);
        modifierShader.setFloat("baseColorSaturation", material.materialModifiers[curModI].sections[4].elements[1].rangeBar.value);
        /* FBM Properties */
        modifierShader.setFloat("fbmFrequency", material.materialModifiers[curModI].sections[5].elements[0].rangeBar.value);
        /* Element property */
        modifierShader.setFloat("woodGamma", material.materialModifiers[curModI].sections[6].elements[0].rangeBar.value);
        modifierShader.setFloat("shininess", material.materialModifiers[curModI].sections[6].elements[1].rangeBar.value);
        modifierShader.setFloat("metallic", material.materialModifiers[curModI].sections[6].elements[2].rangeBar.value);
        modifierShader.setFloat("height", material.materialModifiers[curModI].sections[6].elements[3].rangeBar.value);
    }
    else if(material.materialModifiers[curModI].modifierIndex == MOSS_MATERIAL_MODIFIER){
        /* Color */
        modifierShader.setVec3("mossColorBack", material.materialModifiers[curModI].sections[0].elements[0].button.color);
        modifierShader.setVec3("mossColorFront", material.materialModifiers[curModI].sections[0].elements[1].button.color);
        modifierShader.setVec3("dirtColor", material.materialModifiers[curModI].sections[0].elements[2].button.color);
        /* Droplets */
        modifierShader.setFloat("dropletsCount", material.materialModifiers[curModI].sections[1].elements[0].rangeBar.value);
        modifierShader.setFloat("dropletsOpacityJitter", material.materialModifiers[curModI].sections[1].elements[1].rangeBar.value);
        modifierShader.setFloat("dropletsSize", material.materialModifiers[curModI].sections[1].elements[2].rangeBar.value);
        /* Front Layer */
        modifierShader.setFloat("frontLayerStrength", material.materialModifiers[curModI].sections[2].elements[0].rangeBar.value);
        modifierShader.setFloat("frontLayerScale", material.materialModifiers[curModI].sections[2].elements[1].rangeBar.value);
        /* Lighting */
        modifierShader.setFloat("lightStrength", material.materialModifiers[curModI].sections[3].elements[0].rangeBar.value);
        modifierShader.setFloat("darkeningStrength", material.materialModifiers[curModI].sections[3].elements[1].rangeBar.value);
        /* Noise */
        modifierShader.setFloat("noiseStrength", material.materialModifiers[curModI].sections[4].elements[0].rangeBar.value);
        /* Moss Properties*/
        modifierShader.setFloat("scale", material.materialModifiers[curModI].sections[5].elements[0].rangeBar.value);
        /* Element property */
        modifierShader.setFloat("wetness", material.materialModifiers[curModI].sections[6].elements[0].rangeBar.value);
        modifierShader.setFloat("metallic", material.materialModifiers[curModI].sections[6].elements[1].rangeBar.value);
        modifierShader.setFloat("height", material.materialModifiers[curModI].sections[6].elements[2].rangeBar.value);
    }
    else if(material.materialModifiers[curModI].modifierIndex == RUST_MATERIAL_MODIFIER){
        /* Colors */
        modifierShader.setVec3("color1", material.materialModifiers[curModI].sections[0].elements[0].button.color);
        modifierShader.setVec3("color2", material.materialModifiers[curModI].sections[0].elements[1].button.color);
        modifierShader.setVec3("color3", material.materialModifiers[curModI].sections[0].elements[2].button.color);
        modifierShader.setVec3("color4", material.materialModifiers[curModI].sections[0].elements[3].button.color);
        /*Properties*/
        modifierShader.setFloat("scale", material.materialModifiers[curModI].sections[1].elements[0].rangeBar.value);
        /* Perlin Properties */
        modifierShader.setInt("firstOctave", material.materialModifiers[curModI].sections[2].elements[0].rangeBar.value); 
        modifierShader.setInt("octaves", material.materialModifiers[curModI].sections[2].elements[1].rangeBar.value);
        modifierShader.setFloat("persistence", material.materialModifiers[curModI].sections[2].elements[2].rangeBar.value); 
        /* FBM Properties*/
        modifierShader.setInt("fbmOctaves", material.materialModifiers[curModI].sections[3].elements[0].rangeBar.value); 
        modifierShader.setFloat("fbmRoughness", material.materialModifiers[curModI].sections[3].elements[1].rangeBar.value);
        /* Rust Properties */
        modifierShader.setFloat("rustRadius", material.materialModifiers[curModI].sections[4].elements[0].rangeBar.value);
        modifierShader.setFloat("batteringStrength", material.materialModifiers[curModI].sections[4].elements[1].rangeBar.value);
        /* Noise Properties*/
        modifierShader.setFloat("noiseStrength", material.materialModifiers[curModI].sections[5].elements[0].rangeBar.value);
        /* Element property */
        modifierShader.setFloat("wetness", material.materialModifiers[curModI].sections[6].elements[0].rangeBar.value);
        modifierShader.setFloat("metallic", material.materialModifiers[curModI].sections[6].elements[1].rangeBar.value);
        modifierShader.setFloat("height", material.materialModifiers[curModI].sections[6].elements[2].rangeBar.value);
    }
    else if(material.materialModifiers[curModI].modifierIndex == SKIN_MATERIAL_MODIFIER){
        /* Droplets */
        modifierShader.setFloat("dropletsCount", material.materialModifiers[curModI].sections[0].elements[0].rangeBar.value);
        modifierShader.setFloat("dropletsOpacityJitter", material.materialModifiers[curModI].sections[0].elements[1].rangeBar.value);
        modifierShader.setFloat("dropletsSize", material.materialModifiers[curModI].sections[0].elements[2].rangeBar.value);
        /* Veins */
        modifierShader.setFloat("veinsScale", material.materialModifiers[curModI].sections[1].elements[0].rangeBar.value);
        modifierShader.setFloat("veinsStrength", material.materialModifiers[curModI].sections[1].elements[1].rangeBar.value);
        /* Blushing */
        modifierShader.setFloat("blushingStrength", material.materialModifiers[curModI].sections[2].elements[0].rangeBar.value);
        /* Skin Prints */
        modifierShader.setFloat("skinPrintsScale", material.materialModifiers[curModI].sections[3].elements[0].rangeBar.value);
        modifierShader.setFloat("skinPrintsStrength", material.materialModifiers[curModI].sections[3].elements[1].rangeBar.value);
        /* Noise */
        modifierShader.setFloat("noiseStrength", material.materialModifiers[curModI].sections[4].elements[0].rangeBar.value);
        /* Skin Properties*/
        modifierShader.setVec3("skinColor", material.materialModifiers[curModI].sections[5].elements[0].button.color);
        modifierShader.setVec3("veinColor", material.materialModifiers[curModI].sections[5].elements[1].button.color);
        modifierShader.setFloat("skinScale", material.materialModifiers[curModI].sections[5].elements[2].rangeBar.value);
        modifierShader.setFloat("skinWetness", material.materialModifiers[curModI].sections[5].elements[3].rangeBar.value);
        modifierShader.setFloat("skinMetallic", material.materialModifiers[curModI].sections[5].elements[4].rangeBar.value);
        modifierShader.setFloat("skinHeight", material.materialModifiers[curModI].sections[5].elements[5].rangeBar.value);
        modifierShader.setFloat("skinAmbientOcclusion", material.materialModifiers[curModI].sections[5].elements[6].rangeBar.value);
    }
}


static Texture textureModifierSelectedTexture_procedural;
static Texture mathModifierTexture_procedural;
static Texture albedoFilterMaskTexture_procedural;
static Texture previousTexture;
static Texture prevDepthTexture;

void MaterialModifier::updateMaterialChannels(Material &material, Mesh &mesh, int textureResolution, int curModI, Texture meshMask, Texture selectedObjectPrimitivesTxtr, bool noPrevTxtrMode, Model& model){
    Debugger::block("updateMaterialChannels : 4894352132135");
    Debugger::block("updateMaterialChannels : 4894352132135");
    
    Debugger::block("updateMaterialChannels : 897645122316454");
    if(this->hide)
        return;

    Shader modifierShader = material.materialModifiers[curModI].shader;

    //Set the orthographic projection to render the uvs
    glm::mat4 projection = glm::ortho(0.f, 1.f, 0.f, 1.f);
    Debugger::block("updateMaterialChannels : 897645122316454");

    Debugger::block("updateMaterialChannels : 78978734211231447");
    glm::ivec2 prevPrevDepthTextureRes = mesh.heightMap.getResolution(); 

    if(!prevDepthTexture.ID)
        prevDepthTexture = Texture(nullptr, prevPrevDepthTextureRes.x, prevPrevDepthTextureRes.y);
    else
        prevDepthTexture.update(nullptr, prevPrevDepthTextureRes.x, prevPrevDepthTextureRes.y);

    mesh.heightMap.duplicateTextureSub(prevDepthTexture);
    Debugger::block("updateMaterialChannels : 78978734211231447");
    
    Debugger::block("updateMaterialChannels : 112313148498432121");
    Texture maskTexture_procedural = material.materialModifiers[curModI].maskTexture.generateProceduralTexture(mesh, textureResolution);
    Debugger::block("updateMaterialChannels : 112313148498432121");

    for (int channelI = 0; channelI < 6; channelI++){
        Debugger::block("updateMaterialChannels : 7894413213151");
        //Set the OpenGL viewport to the texture resolution
        glViewport(0,0,textureResolution,textureResolution);
    
        Framebuffer FBO;
        Texture currentTexture;
        // Create the FBO & set the current texture and previous texture
        channelPrep(
                        material, 
                        mesh, 
                        textureResolution, 
                        curModI, 
                        getScene()->projectionMatrix, 
                        getScene()->viewMatrix, 
                        channelI, 
                        FBO, 
                        currentTexture, 
                        previousTexture
                    );
        Debugger::block("updateMaterialChannels : 7894413213151");
        
        Debugger::block("updateMaterialChannels : 798742313214649684");
        // Generate the procedural texture of the selected texture for the texture modifier
        if(material.materialModifiers[curModI].modifierIndex == TEXTURE_MATERIAL_MODIFIER){
            if(textureModifierSelectedTexture_procedural.ID == 0)
                textureModifierSelectedTexture_procedural = Texture(nullptr, textureResolution, textureResolution);
            
            textureModifierSelectedTexture_procedural.update(nullptr, textureResolution, textureResolution);

            Texture txtr = material.materialModifiers[curModI].sections[0].elements[channelI + 1].button.texture;
            txtr.generateProceduralTexture(mesh, textureModifierSelectedTexture_procedural, textureResolution);
            
            modifierShader.use();
        }
        Debugger::block("updateMaterialChannels : 798742313214649684");

        Debugger::block("updateMaterialChannels : 11123132189897");
        // Bind the capture framebuffer after creating it in the channelPrep function
        FBO.bind();
        
        // Use the shader of the modifier
        modifierShader.use(); 
 
        Debugger::block("updateMaterialChannels : 11123132189897");
        if(material.materialModifiers[curModI].modifierIndex != MATH_MATERIAL_MODIFIER){
            
            Debugger::block("updateMaterialChannels : 8979878974156");
            
            // Vertex shader
            modifierShader.setMat4("orthoProjection", projection);
            modifierShader.setMat4("perspectiveProjection", getScene()->projectionMatrix);
            modifierShader.setMat4("view", getScene()->viewMatrix);
            
            // Fragment shader
            setUniforms(
                            modifierShader, 
                            material, 
                            channelI, 
                            curModI, 
                            maskTexture_procedural.ID, 
                            previousTexture, 
                            currentTexture, 
                            prevDepthTexture,
                            textureModifierSelectedTexture_procedural.ID,
                            mesh,
                            meshMask,
                            selectedObjectPrimitivesTxtr,
                            noPrevTxtrMode
                        );
            Debugger::block("updateMaterialChannels : 8979878974156");
            
            Debugger::block("updateMaterialChannels : 44564164846451");
            // Render the result to the framebuffer
            mesh.Draw(false);

            //Delete the framebuffer after completing the channel
            FBO.deleteBuffers(false, false);

            Debugger::block("updateMaterialChannels : 44564164846451");
            Debugger::block("updateMaterialChannels : 13212319789877");
            //Generating the normal map based on the height map
            if(channelI == 4){
                //Blur the height map option
                if(material.materialModifiers[curModI].sections[material.materialModifiers[curModI].sections.size()-3].elements[1].checkBox.clickState1)
                    blurTheTexture(mesh.heightMap.ID, mesh, textureResolution, 1.f, 0);
                
                //Generate the normal map
                mesh.heightMap.generateNormalMap(mesh.normalMap.ID, textureResolution, 10.f, false);
                
                //Remove the seams of the normal map texture
                mesh.normalMap.removeSeams(mesh, textureResolution);
            }
            Debugger::block("updateMaterialChannels : 13212319789877");

            Debugger::block("updateMaterialChannels : 4889798741231");
            glEnable(GL_DEPTH_TEST);

            // Remove the seams from the generated texture
            currentTexture.removeSeams(mesh,textureResolution);

            Debugger::block("updateMaterialChannels : 4889798741231");
            
            Debugger::block("updateMaterialChannels : 9798723131134854");
            // Apply the filter to the albedo 
            if(channelI == 0 && material.materialModifiers[curModI].sections[material.materialModifiers[curModI].sections.size()-2].elements[0].button.filter.shader.ID){
                glActiveTexture(GL_TEXTURE0);

                // Generate the mask texture
                Texture maskTxtr = material.materialModifiers[curModI].sections[material.materialModifiers[curModI].sections.size()-2].elements[1].button.texture; 
                
                if(albedoFilterMaskTexture_procedural.ID == 0)
                    albedoFilterMaskTexture_procedural = Texture(nullptr, textureResolution, textureResolution);
                
                albedoFilterMaskTexture_procedural.update(nullptr, textureResolution, textureResolution);

                if(maskTxtr.ID){
                    maskTxtr.generateProceduralTexture(mesh, albedoFilterMaskTexture_procedural, textureResolution);
                }


                Filter filter = material.materialModifiers[curModI].sections[material.materialModifiers[curModI].sections.size()-2].elements[0].button.filter;

                // Apply the filter 
                getBox()->bindBuffers();
                filter.applyFilter(currentTexture.ID, albedoFilterMaskTexture_procedural, maskTexture_procedural);
            }
            Debugger::block("updateMaterialChannels : 9798723131134854");
        }
        else{

            Debugger::block("updateMaterialChannels : 46544623131");
            int opI = material.materialModifiers[curModI].sections[0].elements[0].comboBox.selectedIndex;

            // Operation uses both sides
            if(opI == 0 || opI == 1 || opI == 2 || opI == 3 || opI == 4 ||  opI == 6 || opI == 7 || opI == 13 || opI == 14 || opI == 15){
                
                if(material.materialModifiers[curModI].sections[0].elements[4].checkBox.clickState1)
                    sections[0].elements[2].button.text = "Right Side : From The Texture";
                else
                    sections[0].elements[2].button.text = "Right Side : From The Slide Bar";
                
                material.materialModifiers[curModI].sections[0].elements[3].scale.y = 1.5f;
                material.materialModifiers[curModI].sections[0].elements[4].scale.y = 2.f;
                material.materialModifiers[curModI].sections[0].elements[5].scale.y = 4.f;
            }
            // Only left side
            else{
                sections[0].elements[2].button.text = "Right Side : Not used";

                material.materialModifiers[curModI].sections[0].elements[3].scale.y = 0.f;
                material.materialModifiers[curModI].sections[0].elements[4].scale.y = 0.f;
                material.materialModifiers[curModI].sections[0].elements[5].scale.y = 0.f;
            }
            Debugger::block("updateMaterialChannels : 46544623131");
            Debugger::block("updateMaterialChannels : 46848423123131");

            if(opI == 0 || opI == 1 || opI == 6 || opI == 7){
                material.materialModifiers[curModI].sections[0].elements[3].rangeBar.maxValue = 1.f;
                material.materialModifiers[curModI].sections[0].elements[3].rangeBar.minValue = -1.f;
                
                if(
                    material.materialModifiers[curModI].sections[0].elements[3].rangeBar.value > 1.f || 
                    material.materialModifiers[curModI].sections[0].elements[3].rangeBar.value < -1.f
                )
                    material.materialModifiers[curModI].sections[0].elements[3].rangeBar.value = 0.f;
            }
            else{
                material.materialModifiers[curModI].sections[0].elements[3].rangeBar.maxValue = 10.f;
                material.materialModifiers[curModI].sections[0].elements[3].rangeBar.minValue = -10.f;
            }
            Debugger::block("updateMaterialChannels : 46848423123131");

            Debugger::block("updateMaterialChannels : 847894231321");
            modifierShader.setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
            modifierShader.setVec3("pos", glm::vec3(0.5f, 0.5f, 0.7f));
            modifierShader.setVec2("scale", glm::vec2(0.5f));
            
            modifierShader.setInt( "operationIndex" , material.materialModifiers[curModI].sections[0].elements[0].comboBox.selectedIndex);
            modifierShader.setInt( "useRightSideTxtr" , material.materialModifiers[curModI].sections[0].elements[4].checkBox.clickState1);
            modifierShader.setFloat( "rightSideVal" , material.materialModifiers[curModI].sections[0].elements[3].rangeBar.value);
            modifierShader.setInt( "previousTxtr" , 0);
            modifierShader.setInt( "rightSideTxtr" , 1);
            modifierShader.setInt( "mask" , 2);
            modifierShader.setFloat( "opacity" , material.materialModifiers[curModI].sections[1].elements[channelI].rangeBar.value);
        
            {
                if(mathModifierTexture_procedural.ID == 0)
                    mathModifierTexture_procedural = Texture(nullptr, textureResolution, textureResolution);
                
                mathModifierTexture_procedural.update(nullptr, textureResolution, textureResolution);

                Texture txtr = material.materialModifiers[curModI].sections[0].elements[5].button.texture;
                txtr.generateProceduralTexture(mesh, mathModifierTexture_procedural, textureResolution);
                
                modifierShader.use();

                FBO.bind();
            }
            Debugger::block("updateMaterialChannels : 847894231321");
            
            Debugger::block("updateMaterialChannels : 44113213213211");
            glActiveTexture(GL_TEXTURE0);
            if(curModI != material.materialModifiers.size()-1 && !noPrevTxtrMode)
                glBindTexture(GL_TEXTURE_2D, previousTexture.ID);
            else
                glBindTexture(GL_TEXTURE_2D, appTextures.black.ID);

        
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, mathModifierTexture_procedural.ID);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, maskTexture_procedural.ID);

            getBox()->bindBuffers();
            // Render the result to the framebuffer
            glDrawArrays(GL_TRIANGLES, 0, 6);

            FBO.deleteBuffers(false, false);
            Debugger::block("updateMaterialChannels : 44113213213211");
        }
        
        Debugger::block("updateMaterialChannels : 4423132199999");
        if(material.materialModifiers[curModI].modifierIndex != MATH_MATERIAL_MODIFIER){
            if(material.materialModifiers[curModI].sections[sections.size() - 1].elements[0].checkBox.clickState1){
                genAmbientOcclusion(
                                    mesh.ambientOcclusion, 
                                    mesh, 
                                    meshMask, 
                                    selectedObjectPrimitivesTxtr, 
                                    model,
                                    material.materialModifiers[curModI].sections[sections.size() - 1].elements[1].checkBox.clickState1,
                                    material.materialModifiers[curModI].sections[sections.size() - 1].elements[2].checkBox.clickState1,
                                    material.materialModifiers[curModI].sections[sections.size() - 1].elements[3].rangeBar.value,
                                    material.materialModifiers[curModI].sections[sections.size() - 1].elements[4].rangeBar.value,
                                    material.materialModifiers[curModI].sections[sections.size() - 1].elements[5].checkBox.clickState1,
                                    material.materialModifiers[curModI].sections[sections.size() - 1].elements[6].checkBox.clickState1,
                                    material.materialModifiers[curModI].sections[sections.size() - 1].elements[7].checkBox.clickState1,
                                    material.materialModifiers[curModI].sections[sections.size() - 1].elements[8].checkBox.clickState1,
                                    material.materialModifiers[curModI].sections[sections.size() - 1].elements[9].checkBox.clickState1,
                                    material.materialModifiers[curModI].sections[sections.size() - 1].elements[10].checkBox.clickState1,
                                    material.materialModifiers[curModI].sections[sections.size() - 1].elements[11].checkBox.clickState1
                                );
            }
        }
        
        getBox()->bindBuffers();
        Debugger::block("updateMaterialChannels : 4423132199999");
    }
}