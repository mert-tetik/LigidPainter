/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

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

static void channelPrep(Material &material, Mesh &mesh, int& textureResolution, int& curModI, glm::mat4& perspective, glm::mat4& view, int& channelI, Framebuffer& FBO, Texture& currentTexture, Texture& previousTexture, Texture& prevDepth){
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

    prevDepth = mesh.heightMap.duplicateTexture();

    /* ! Binds another framebuffer ! */
    previousTexture = currentTexture.duplicateTexture();

    currentTexture.update(nullptr, textureResolution, textureResolution);    
    
    //That framebuffer will be used to get the results of the shader 
    FBO = Framebuffer(currentTexture, GL_TEXTURE_2D);

    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
}

static void blurTheTexture(unsigned int& txtr, Mesh& mesh, int textureResolution){
    
    Texture textureObject = Texture(txtr);
    unsigned int textureCopy = textureObject.duplicateTexture();

    Framebuffer FBO = Framebuffer(txtr, GL_TEXTURE_2D);
    glViewport(0, 0, textureResolution, textureResolution);

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    getBox()->bindBuffers();
    
    glm::mat4 projection = glm::ortho(0.f, (float)textureResolution, (float)textureResolution, 0.f); 
    ShaderSystem::bluringShader().use();
    ShaderSystem::bluringShader().setInt("txtr", 0);
    ShaderSystem::bluringShader().setInt("uvMask", 1);
    ShaderSystem::bluringShader().setVec2("txtrRes", glm::vec2(textureResolution));
    ShaderSystem::bluringShader().setMat4("projection"  ,       projection);
    ShaderSystem::bluringShader().setMat4("projectedPosProjection"  ,       projection);
    ShaderSystem::bluringShader().setVec3("pos"         ,       glm::vec3((float)textureResolution / 2.f, (float)textureResolution / 2.f, 0.9f));
    ShaderSystem::bluringShader().setVec2("scale"       ,       glm::vec2((float)textureResolution / 2.f));
    ShaderSystem::bluringShader().setFloat("blurVal"     ,     1.f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureCopy);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mesh.uvMask.ID);

    glDrawArrays(GL_TRIANGLES, 0 , 6);

    Settings::defaultFramebuffer()->FBO.bind();
    
    FBO.deleteBuffers(false, false);
    glDeleteTextures(1, &textureCopy);
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
                            Texture selectedObjectPrimitivesTxtr
                        )
{

    /* Channel Properties */
    modifierShader.setInt("state", channelI); //Set the state
    modifierShader.setInt("mask", 0); //Set the mask texture slot
    modifierShader.setInt("previousTxtr", 1); //Set the previous texture slot
    modifierShader.setInt( "depthTxtr" , 2); //Set the previous height map texture
    
    if(material.materialModifiers[curModI].modifierIndex != SOLID_MATERIAL_MODIFIER)
        modifierShader.setFloat( "opacity" , material.materialModifiers[curModI].sections[material.materialModifiers[curModI].sections.size()-3].elements[channelI].rangeBar.value);
    else
        modifierShader.setFloat( "opacity" , material.materialModifiers[curModI].sections[0].elements[channelI * 2 + 1].rangeBar.value);

    modifierShader.setFloat( "depthValue" , material.materialModifiers[curModI].sections[material.materialModifiers[curModI].sections.size()-2].elements[0].rangeBar.value);
    modifierShader.setInt("selectedPrimitiveIDS" , 3); //Set the previous height map texture
    modifierShader.setInt("meshMask" , 4); //Set the previous height map texture
    modifierShader.setInt("primitiveCount" , mesh.indices.size() / 3); //Set the previous height map texture
    modifierShader.setInt("useMeshMask" , selectedObjectPrimitivesTxtr.ID != 0); //Set the previous height map texture

    // Bind the mask texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, maskTexture_procedural);

    //Bind the previous texture
    glActiveTexture(GL_TEXTURE1);
    if(curModI != material.materialModifiers.size()-1)
        glBindTexture(GL_TEXTURE_2D, previousTexture.ID);
    else
        glBindTexture(GL_TEXTURE_2D, currentTexture.ID);
    
    //Bind the previous height map texture
    glActiveTexture(GL_TEXTURE2);
    if(curModI != material.materialModifiers.size()-1)
        glBindTexture(GL_TEXTURE_2D, prevDepthTexture.ID);
    else
        glBindTexture(GL_TEXTURE_2D, 0);

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

void MaterialModifier::updateMaterialChannels(Material &material, Mesh &mesh, int textureResolution, int curModI, Texture meshMask, Texture selectedObjectPrimitivesTxtr){
    
    Shader modifierShader = material.materialModifiers[curModI].shader;

    //Set the orthographic projection to render the uvs
    glm::mat4 projection = glm::ortho(0.f, 1.f, 0.f, 1.f);

    //TODO Don't create the shader in the modifier function

    Texture prevDepthTexture;
    prevDepthTexture = mesh.heightMap.duplicateTexture();
    //Disable the depth test (just in case)
    
    unsigned int maskTexture_procedural;
    maskTexture_procedural = material.materialModifiers[curModI].maskTexture.generateProceduralTexture(mesh, textureResolution);

    for (int channelI = 0; channelI < 6; channelI++){
        //Set the OpenGL viewport to the texture resolution
        glViewport(0,0,textureResolution,textureResolution);
    
        Framebuffer FBO;
        Texture currentTexture;
        Texture previousTexture;
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
                        previousTexture, 
                        prevDepthTexture
                    );
        
        unsigned int textureModifierSelectedTexture_procedural = 0;
        // Generate the procedural texture of the selected texture for the texture modifier
        if(material.materialModifiers[curModI].modifierIndex == TEXTURE_MATERIAL_MODIFIER){
            textureModifierSelectedTexture_procedural = material.materialModifiers[curModI].sections[0].elements[channelI + 1].button.texture.generateProceduralTexture(mesh, textureResolution);
            modifierShader.use();
        }

        // Bind the capture framebuffer after creating it in the channelPrep function
        FBO.bind();
        
        // Use the shader of the modifier
        modifierShader.use(); 
 
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
                        maskTexture_procedural, 
                        previousTexture, 
                        currentTexture, 
                        prevDepthTexture,
                        textureModifierSelectedTexture_procedural,
                        mesh,
                        meshMask,
                        selectedObjectPrimitivesTxtr
                    );
    
        // Render the result to the framebuffer
        mesh.Draw(false);
        
        //Delete the framebuffer after completing the channel
        FBO.deleteBuffers(false, false);

        //Generating the normal map based on the height map
        if(channelI == 4){
            // TODO HANDLE THIS
            //Blur the height map option
            if(material.materialModifiers[curModI].sections[material.materialModifiers[curModI].sections.size()-2].elements[1].checkBox.clickState1)
                blurTheTexture(mesh.heightMap.ID, mesh, textureResolution);
            
            //Generate the normal map
            mesh.heightMap.generateNormalMap(mesh.normalMap.ID, textureResolution, 10.f, false);
            
            //Remove the seams of the normal map texture
            mesh.normalMap.removeSeams(mesh, textureResolution);
        }

        glEnable(GL_DEPTH_TEST);

        // Remove the seams from the generated texture
        currentTexture.removeSeams(mesh,textureResolution);

        // Apply the filter to the albedo 
        if(channelI == 0 && material.materialModifiers[curModI].sections[material.materialModifiers[curModI].sections.size()-1].elements[0].button.filter.shader.ID){
            glActiveTexture(GL_TEXTURE0);

            // Generate the mask texture
            unsigned int proceduralAlbedoFilterMaskTexture = 0;
            if(material.materialModifiers[curModI].sections[material.materialModifiers[curModI].sections.size()-1].elements[1].button.texture.ID)
                proceduralAlbedoFilterMaskTexture = material.materialModifiers[curModI].sections[material.materialModifiers[curModI].sections.size()-1].elements[1].button.texture.generateProceduralTexture(mesh, textureResolution);
            
            // Apply the filter 
            getBox()->bindBuffers();
            material.materialModifiers[curModI].sections[material.materialModifiers[curModI].sections.size()-1].elements[0].button.filter.applyFilter(currentTexture.ID, proceduralAlbedoFilterMaskTexture, maskTexture_procedural);
            
            // Delete the generated procedural texture (if generated)
            if(proceduralAlbedoFilterMaskTexture)
                glDeleteTextures(1, &proceduralAlbedoFilterMaskTexture);
        }

        // Delete the duplicated previous texture
        glDeleteTextures(1, &previousTexture.ID);
        
        // Delete the generated selected procedural texture for the texture modifier (if generated (if texture modifier))
        if(textureModifierSelectedTexture_procedural)
            glDeleteTextures(1, &textureModifierSelectedTexture_procedural);
    }
    
    // Delete the duplicated previous height map texture 
    glDeleteTextures(1, &prevDepthTexture.ID);
    
    // Delete the generated mask texture 
    glDeleteTextures(1, &maskTexture_procedural);
}