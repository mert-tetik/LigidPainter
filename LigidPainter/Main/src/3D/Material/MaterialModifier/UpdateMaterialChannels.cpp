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

#include "UTIL/Util.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/GL/GL.hpp"

#include "GUI/Elements.hpp"

#include "3D/ThreeD.hpp"


//0 = albedo
//1 = roughness
//2 = metallic 
//3 = normal map
//4 = height map
//5 = ambient Occlusion

static void channelPrep(Material &material, Mesh &mesh, int& textureResolution, int& curModI, glm::mat4& perspective, glm::mat4& view, int& channelI, Texture& currentTexture, Texture& previousTexture){
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
        previousTexture = Texture((char*)nullptr, prevCurrentTextureRes.x, prevCurrentTextureRes.y);
    else
        previousTexture.update((char*)nullptr, prevCurrentTextureRes.x, prevCurrentTextureRes.y);
    
    currentTexture.duplicateTextureSub(previousTexture);

    currentTexture.update((char*)nullptr, textureResolution, textureResolution);    
    

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
}

static void blurTheTexture(Texture txtr, Mesh& mesh, int textureResolution, float blurVal, int algorithmI){
    
    Texture textureCopy = txtr.get_temp_copy_txtr();
    
    Framebuffer FBO = FBOPOOL::requestFBO(txtr, "Bluring texture (material update)");
    
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    Shader blurShader;
    if(algorithmI == 0)
        blurShader = ShaderSystem::bluringShader();    
    else if(algorithmI == 1)
        blurShader = ShaderSystem::twoPassBlur();    

    blurShader.use();
    blurShader.setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
    blurShader.setMat4("projectedPosProjection", glm::ortho(0.f, 1.f, 1.f, 0.f));
    blurShader.setVec3("pos", glm::vec3(0.5f, 0.5f, 0.9f));
    blurShader.setVec2("scale", glm::vec2(0.5f));
    
    blurShader.setInt("txtr", GL::bindTexture_2D(textureCopy.ID, "UpdateMaterialChannel : blurTheTexture"));
    blurShader.setInt("uvMask", GL::bindTexture_2D(mesh.uvMask.ID, "UpdateMaterialChannel : blurTheTexture"));
    blurShader.setVec2("txtrRes", glm::vec2(textureResolution));
    blurShader.setFloat("blurVal", blurVal);
    
    if(algorithmI == 1)
        blurShader.setInt("horizontal", 0);

    getBox()->draw("blurTheTexture first draw");

    if(algorithmI == 1){
        txtr.duplicateTextureSub(textureCopy);
        
        FBO.bind();
        glViewport(0, 0, textureResolution, textureResolution);
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        blurShader.setInt("horizontal", 1);
        getBox()->draw("blurTheTexture second draw");
    }

    GL::releaseBoundTextures("UpdateMaterialChannel : blurTheTexture");
    FBOPOOL::releaseFBO(FBO);
}

static Texture aoDepthTxtr;
static Texture aoTxtrDup;

static void genAmbientOcclusion(
                                    Texture& aoTxtr, 
                                    Mesh& mesh, 
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
    
    if(!aoDepthTxtr.ID){
        aoDepthTxtr = Texture((char*)nullptr, 1024, 1024, GL_NEAREST, GL_RGBA, GL_RGBA32F);
        aoTxtrDup = Texture((char*)nullptr, aoTxtrRes.x, aoTxtrRes.y);
    }

    aoTxtrDup.update((char*)nullptr, aoTxtrRes.x, aoTxtrRes.y);

    if(usePrevAO)
        aoTxtr.duplicateTextureSub(aoTxtrDup);

    aoTxtr.update((char*)nullptr, aoTxtr.getResolution().x, aoTxtr.getResolution().y);

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

        {

            Framebuffer FBO = FBOPOOL::requestFBO(aoDepthTxtr, "aoDepthFBO genAmbientOcclusion");

            glClearColor(0,0,0,0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            ShaderSystem::depth3D().use();
            ShaderSystem::depth3D().setMat4("view", view);
            ShaderSystem::depth3D().setMat4("projection", projection);
            ShaderSystem::depth3D().setMat4("modelMatrix", glm::mat4(1.f));

            ShaderUTIL::set_shader_struct_face_selection_data(ShaderSystem::depth3D(), Mesh());

            if(singleMesh)
                mesh.Draw();
            else
                model.Draw();

            GL::releaseBoundTextures("UpdateMaterialChannel : genAmbientOcclusion");
            FBOPOOL::releaseFBO(FBO);
        }

        {
            Framebuffer FBO = FBOPOOL::requestFBO(aoTxtr, "aoDepthFBO genAmbientOcclusion");
            
            ShaderSystem::AOGen().use();
            ShaderSystem::AOGen().setMat4("orthoProjection", glm::ortho(0.f, 1.f, 0.f, 1.f)); 
            ShaderSystem::AOGen().setMat4("perspectiveProjection", projection); 
            ShaderSystem::AOGen().setMat4("view", view); 
            
            ShaderUTIL::set_shader_struct_face_selection_data(ShaderSystem::AOGen(), mesh);
            
            ShaderSystem::AOGen().setInt("depthTexture", GL::bindTexture_2D(aoDepthTxtr.ID, "UpdateMaterialChannel : genAmbientOcclusion"));
            ShaderSystem::AOGen().setInt("srcTxtr", GL::bindTexture_2D(aoTxtrDup.ID, "UpdateMaterialChannel : genAmbientOcclusion"));
            ShaderSystem::AOGen().setFloat("aoOffset", aoOffset);
            
            mesh.Draw();

            GL::releaseBoundTextures("UpdateMaterialChannel : genAmbientOcclusion");
            FBOPOOL::releaseFBO(FBO);
        }
    }
    
    if(smoothness > 0.1f)
        blurTheTexture(aoTxtr, mesh, aoTxtrRes.x, smoothness, useSecondBlurAlgorithm);
    
    aoTxtr.removeSeams(mesh);

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
                            bool noPrevTxtrMode
                        )
{

    /* Channel Properties */
    modifierShader.setInt("state", channelI); //Set the state
    modifierShader.setInt("mask", GL::bindTexture_2D(maskTexture_procedural, "UpdateMaterialChannel : setUniforms"));
    modifierShader.setInt("previousTxtr", GL::bindTexture_2D((curModI != material.materialModifiers.size()-1 && !noPrevTxtrMode) ? previousTexture.ID : appTextures.black.ID, "UpdateMaterialChannel : setUniforms"));
    modifierShader.setInt( "depthTxtr" , GL::bindTexture_2D((curModI != material.materialModifiers.size()-1) ? prevDepthTexture.ID : appTextures.black.ID, "UpdateMaterialChannel : setUniforms"));
    
    if(material.materialModifiers[curModI].modifierIndex != SOLID_MATERIAL_MODIFIER)
        modifierShader.setFloat( "opacity" , material.materialModifiers[curModI].sections[material.materialModifiers[curModI].sections.size()-4].elements[channelI].rangeBar.value);
    else
        modifierShader.setFloat( "opacity" , material.materialModifiers[curModI].sections[0].elements[channelI * 2 + 1].rangeBar.value);

    modifierShader.setFloat( "depthValue" , material.materialModifiers[curModI].sections[material.materialModifiers[curModI].sections.size()-3].elements[0].rangeBar.value);

    ShaderUTIL::set_shader_struct_face_selection_data(modifierShader, mesh);

    if(material.materialModifiers[curModI].modifierIndex == TEXTURE_MATERIAL_MODIFIER){
        /* Selected texture */
        modifierShader.setInt("theTexture", GL::bindTexture_2D(textureModifierSelectedTexture_procedural, "UpdateMaterialChannel : setUniforms"));
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

void MaterialModifier::updateMaterialChannels(Material &material, int curModI, Model& model, Mesh &mesh, int textureResolution, bool noPrevTxtrMode)
{
    if(this->hide)
        return;

    Shader modifierShader = material.materialModifiers[curModI].shader;

    //Set the orthographic projection to render the uvs
    glm::mat4 projection = glm::ortho(0.f, 1.f, 0.f, 1.f);

    glm::ivec2 prevPrevDepthTextureRes = mesh.heightMap.getResolution(); 

    if(!prevDepthTexture.ID)
        prevDepthTexture = Texture((char*)nullptr, prevPrevDepthTextureRes.x, prevPrevDepthTextureRes.y);
    else
        prevDepthTexture.update((char*)nullptr, prevPrevDepthTextureRes.x, prevPrevDepthTextureRes.y);

    mesh.heightMap.duplicateTextureSub(prevDepthTexture);
    
    Texture maskTexture_procedural = material.materialModifiers[curModI].maskTexture.generateProceduralTexture(mesh, textureResolution);

    for (int channelI = 0; channelI < 6; channelI++){
        
        //Set the OpenGL viewport to the texture resolution
        glViewport(0,0,textureResolution,textureResolution);
    
        Texture currentTexture;
        // Create the FBO & set the current texture and previous texture
        channelPrep(
                        material, 
                        mesh, 
                        textureResolution, 
                        curModI, 
                        getScene()->projectionMatrix, 
                        getScene()->camera.viewMatrix, 
                        channelI, 
                        currentTexture, 
                        previousTexture
                    );

        
        // Generate the procedural texture of the selected texture for the texture modifier
        if(material.materialModifiers[curModI].modifierIndex == TEXTURE_MATERIAL_MODIFIER){
            if(textureModifierSelectedTexture_procedural.ID == 0)
                textureModifierSelectedTexture_procedural = Texture((char*)nullptr, textureResolution, textureResolution);
            
            textureModifierSelectedTexture_procedural.update((char*)nullptr, textureResolution, textureResolution);

            Texture txtr = material.materialModifiers[curModI].sections[0].elements[channelI + 1].button.texture;
            txtr.generateProceduralTexture(mesh, textureModifierSelectedTexture_procedural, textureResolution);
            
            modifierShader.use();
        }

        // Bind the capture framebuffer after creating it in the channelPrep function
        Framebuffer FBO = FBOPOOL::requestFBO(currentTexture, "MaterialModifier::updateMaterialChannels");
        
        // Use the shader of the modifier
        modifierShader.use(); 
 
        if(material.materialModifiers[curModI].modifierIndex != MATH_MATERIAL_MODIFIER){
            
            // Vertex shader
            modifierShader.setMat4("orthoProjection", projection);
            modifierShader.setMat4("perspectiveProjection", getScene()->projectionMatrix);
            modifierShader.setMat4("view", getScene()->camera.viewMatrix);
            
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
                            noPrevTxtrMode
                        );
            
            // Render the result to the framebuffer
            mesh.Draw();

            //Delete the framebuffer after completing the channel
            FBOPOOL::releaseFBO(FBO);

            //Generating the normal map based on the height map
            if(channelI == 4){
                //Blur the height map option
                if(material.materialModifiers[curModI].sections[material.materialModifiers[curModI].sections.size()-3].elements[1].checkBox.clickState1)
                    blurTheTexture(mesh.heightMap, mesh, textureResolution, 1.f, 0);
                
                //Generate the normal map
                mesh.heightMap.generateNormalMap(mesh.normalMap, 10.f, false, false);
                
                //Remove the seams of the normal map texture
                mesh.normalMap.removeSeams(mesh);
            }

            glEnable(GL_DEPTH_TEST);

            // Remove the seams from the generated texture
            currentTexture.removeSeams(mesh);

            
            // Apply the filter to the albedo 
            if(channelI == 0 && material.materialModifiers[curModI].sections[material.materialModifiers[curModI].sections.size()-2].elements[0].button.filter.shader.ID){
                // Generate the mask texture
                Texture maskTxtr = material.materialModifiers[curModI].sections[material.materialModifiers[curModI].sections.size()-2].elements[1].button.texture; 
                
                if(albedoFilterMaskTexture_procedural.ID == 0)
                    albedoFilterMaskTexture_procedural = Texture((char*)nullptr, textureResolution, textureResolution);
                
                albedoFilterMaskTexture_procedural.update((char*)nullptr, textureResolution, textureResolution);

                if(maskTxtr.ID){
                    maskTxtr.generateProceduralTexture(mesh, albedoFilterMaskTexture_procedural, textureResolution);
                }


                Filter filter = material.materialModifiers[curModI].sections[material.materialModifiers[curModI].sections.size()-2].elements[0].button.filter;

                // Apply the filter 
                filter.applyFilter(currentTexture, albedoFilterMaskTexture_procedural, maskTexture_procedural);
            }

            GL::releaseBoundTextures("MaterialModifier::updateMaterialChannels::setUniforms");
        }
        else{

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

            modifierShader.setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
            modifierShader.setVec3("pos", glm::vec3(0.5f, 0.5f, 0.7f));
            modifierShader.setVec2("scale", glm::vec2(0.5f));
            
            modifierShader.setInt( "operationIndex" , material.materialModifiers[curModI].sections[0].elements[0].comboBox.selectedIndex);
            modifierShader.setInt( "useRightSideTxtr" , material.materialModifiers[curModI].sections[0].elements[4].checkBox.clickState1);
            modifierShader.setFloat( "rightSideVal" , material.materialModifiers[curModI].sections[0].elements[3].rangeBar.value);
            modifierShader.setInt( "previousTxtr" , GL::bindTexture_2D((curModI != material.materialModifiers.size()-1 && !noPrevTxtrMode) ? previousTexture.ID : appTextures.black.ID, "MaterialModifier::updateMaterialChannels"));
            modifierShader.setInt( "rightSideTxtr" , GL::bindTexture_2D(mathModifierTexture_procedural.ID, "MaterialModifier::updateMaterialChannels"));
            modifierShader.setInt( "mask" , GL::bindTexture_2D(maskTexture_procedural.ID, "MaterialModifier::updateMaterialChannels"));
            modifierShader.setFloat( "opacity" , material.materialModifiers[curModI].sections[1].elements[channelI].rangeBar.value);
        
            {
                if(mathModifierTexture_procedural.ID == 0)
                    mathModifierTexture_procedural = Texture((char*)nullptr, textureResolution, textureResolution);
                
                mathModifierTexture_procedural.update((char*)nullptr, textureResolution, textureResolution);

                Texture txtr = material.materialModifiers[curModI].sections[0].elements[5].button.texture;
                txtr.generateProceduralTexture(mesh, mathModifierTexture_procedural, textureResolution);
                
                modifierShader.use();

                FBO.bind();
            }
            
            // Render the result to the framebuffer
            getBox()->draw("MaterialModifier::updateMaterialChannels for math layer");

            GL::releaseBoundTextures("MaterialModifier::updateMaterialChannels");

            FBOPOOL::releaseFBO(FBO);
        }
        
        if(material.materialModifiers[curModI].modifierIndex != MATH_MATERIAL_MODIFIER){
            if(material.materialModifiers[curModI].sections[sections.size() - 1].elements[0].checkBox.clickState1){
                genAmbientOcclusion(
                                    mesh.ambientOcclusion, 
                                    mesh, 
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
    }
}