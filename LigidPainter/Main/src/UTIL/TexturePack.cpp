/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>

#include "UTIL/Util.hpp"
#include "UTIL/GL/GL.hpp"
#include "GUI/GUI.hpp"

TexturePack::TexturePack(){

}


int TexturePack::load(std::string path){

    try
    {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            Texture txtr;
            txtr.load(entry.path().string().c_str());

            this->textures.push_back(txtr);
        }     
    }
    catch (const std::filesystem::filesystem_error& ex) {
        LGDLOG::start << "ERROR : Filesystem : Location ID 448993 " << ex.what() << LGDLOG::end;
    }
    
    if(this->title == "")
        this->title = UTIL::getLastWordBySeparatingWithChar(path, UTIL::folderDistinguisher());

    return 1;
}

static char* accessCharArray(char*& array, int arraySize, int i){
    if(i < arraySize)
        return &array[i];
    
    return &array[0];
}

struct Region{
    glm::ivec2 startPos;
    glm::ivec2 endPos;

    Texture sprite;
};

static bool checkSqr(Texture txtr, Texture alphaMap, char*& pixels, char*& opacityPixels, glm::ivec2& foundingPos, glm::ivec2 dest, bool& restrictX, bool& restrictY){
    
    if(dest.x >= txtr.getResolution().x)
        return false;

    if(dest.y >= txtr.getResolution().y)
        return false;
    
    float xPosC = foundingPos.x;
    int pixelIndex = 0; // Each pixel has 4 components (R, G, B, A)
    char pxAlpha = 6;
    
    int ind = xPosC;

    while (pxAlpha > 5)
    {
        xPosC--;
        
        if(pixelIndex < 0)
            break;

        pixelIndex = (dest.y * txtr.getResolution().x + xPosC) * 4; // Each pixel has 4 components (R, G, B, A)
        glm::ivec2 ratio = alphaMap.getResolution() / txtr.getResolution();
        int alphaPixelIndex = ((dest.y * ratio.y) * alphaMap.getResolution().x + (xPosC * ratio.x)) * 4; // Each pixel has 4 components (R, G, B, A)

        pxAlpha = *accessCharArray(pixels, txtr.getResolution().x * txtr.getResolution().y * 4, pixelIndex + 3);

        if(alphaMap.ID)
            pxAlpha = *accessCharArray(opacityPixels, alphaMap.getResolution().x * alphaMap.getResolution().y * 4, alphaPixelIndex);

        if(pxAlpha > 5)
            ind--;
    }

    if(foundingPos.x != ind){
        restrictX = false;
        restrictY = false;
        foundingPos.x = ind;
        dest = foundingPos;
    }


    bool opaquePixelDetectedX = false;
    bool a = false;
    for (size_t x = foundingPos.x; x < dest.x; x++)
    {
        a = true;

        int pixelIndex = (dest.y * txtr.getResolution().x + x) * 4; // Each pixel has 4 components (R, G, B, A)
        glm::ivec2 ratio = alphaMap.getResolution() / txtr.getResolution();
        int alphaPixelIndex = ((dest.y * ratio.y) * alphaMap.getResolution().x + (x * ratio.x)) * 4; // Each pixel has 4 components (R, G, B, A)
    
        char pxAlpha = *accessCharArray(pixels, txtr.getResolution().x * txtr.getResolution().y * 4, pixelIndex + 3);
        if(alphaMap.ID)
            pxAlpha = *accessCharArray(opacityPixels, alphaMap.getResolution().x * alphaMap.getResolution().y * 4, alphaPixelIndex);

        if(pxAlpha > 5)
            opaquePixelDetectedX = true; 
    }

    

    if(!opaquePixelDetectedX && a)
        restrictY = true;

    bool opaquePixelDetectedY = false;

    for (size_t y = foundingPos.y; y < dest.y; y++)
    {
        a = true;
        
        int pixelIndex = (y * txtr.getResolution().y + dest.x) * 4; // Each pixel has 4 components (R, G, B, A)
        glm::ivec2 ratio = alphaMap.getResolution() / txtr.getResolution();
        int alphaPixelIndex = ((y * ratio.y) * alphaMap.getResolution().x + (dest.x * ratio.x)) * 4; // Each pixel has 4 components (R, G, B, A)
    
        char pxAlpha = *accessCharArray(pixels, txtr.getResolution().x * txtr.getResolution().y * 4, pixelIndex + 3);
        if(alphaMap.ID)
            pxAlpha = *accessCharArray(opacityPixels, alphaMap.getResolution().x * alphaMap.getResolution().y * 4, alphaPixelIndex);

        if(pxAlpha > 5){
            opaquePixelDetectedY = true; 
        }
    }

    if(!opaquePixelDetectedY && a)
        restrictX = true;

    if((opaquePixelDetectedX && !restrictY) || !a)
        return true;
    
    if((opaquePixelDetectedY && !restrictX) || !a)
        return true;

    return false;
}

static Region processRegion(Texture txtr, Texture alphaMap, char*& pixels, char*& opacityPixels, glm::ivec2 foundingPos){
    
    Region region;

    int i = 0;

    glm::ivec2 destPos = foundingPos;

    bool restrictX = false;
    bool restrictY = false;

    while (checkSqr(txtr, alphaMap, pixels, opacityPixels, foundingPos, destPos, restrictX, restrictY))
    {
        if(!restrictX)
            destPos.x++;
        if(!restrictY)
            destPos.y++;
        i++;
    }
    
    region.startPos = foundingPos;
    region.endPos = destPos;

    return region;
}

static bool isPixelIndexIsInAnyRegion(std::vector<Region> regions, glm::ivec2 pxPos){
    for (size_t i = 0; i < regions.size(); i++)
    {
        Region reg = regions[i];
        if(pxPos.x >= reg.startPos.x && pxPos.x <= reg.endPos.x){
            if(pxPos.y >= reg.startPos.y && pxPos.y <= reg.endPos.y){
                return true;
            }
        }
    }
    
    return false;
}

void TexturePack::saperateSprites(Texture txtr, Texture alphaMap){
   
    char* pixels = new char[txtr.getResolution().x * txtr.getResolution().y * 4];
    char* opacityPixels = new char[alphaMap.getResolution().x * alphaMap.getResolution().y * 4];
    
    alphaMap.getData(opacityPixels);
    txtr.getData(pixels);

    std::vector<Region> regions;

    for (size_t y = 0; y < txtr.getResolution().y; y++)
    {
        for (size_t x = 0; x < txtr.getResolution().x; x++)
        {
            int pixelIndex = (y * txtr.getResolution().x + x) * 4; // Each pixel has 4 components (R, G, B, A)
            glm::ivec2 ratio = alphaMap.getResolution() / txtr.getResolution();
            int alphaPixelIndex = ((y * ratio.y) * alphaMap.getResolution().x + (x * ratio.x)) * 4; // Each pixel has 4 components (R, G, B, A)
            
            char pxAlpha = *accessCharArray(pixels, txtr.getResolution().x * txtr.getResolution().y * 4, pixelIndex + 3); 
            if(alphaMap.ID){
                pxAlpha = *accessCharArray(opacityPixels, alphaMap.getResolution().x * alphaMap.getResolution().y * 4, alphaPixelIndex);
                *accessCharArray(pixels, txtr.getResolution().x * txtr.getResolution().y * 4, pixelIndex + 3) = pxAlpha;
            }

            bool pixelIndexIsInAnyRegion = isPixelIndexIsInAnyRegion(regions, glm::ivec2(x,y));

            // If opaque pixel found
            if(pxAlpha > 5 && !pixelIndexIsInAnyRegion){
                regions.push_back(processRegion(txtr, alphaMap, pixels, opacityPixels, glm::ivec2(x,y)));
            };

        }
    }

    if(alphaMap.ID){
        GL::bindTexture_2D(txtr.ID, "txtrPack");
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, txtr.getResolution().x, txtr.getResolution().y, 0, GL_RGBA, GL_BYTE, pixels);
    }

    unsigned int FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, txtr.ID, 0);

    for (size_t i = 0; i < regions.size(); i++)
    {
        int regionW = regions[i].endPos.x - regions[i].startPos.x;
        int regionH = regions[i].endPos.y - regions[i].startPos.y;

        char* regionPxs = new char[regionW * regionH * 4];

        glReadPixels(
                            regions[i].startPos.x, 
                            regions[i].startPos.y, 
                            regionW, 
                            regionH, 
                            GL_RGBA, 
                            GL_BYTE,
                            regionPxs
                        );

        this->textures.push_back(Texture(regionPxs, regionW, regionH));    
    }

    //Finish
    GL::releaseBoundTextures("txtrPack");
    Settings::defaultFramebuffer()->FBO.bind();
    glDeleteFramebuffers(1, &FBO);
    delete[] pixels;
}

static float hash(glm::vec2 uv, glm::vec2 seed){
    // This is a simple hash function that takes a 2D vector as input and returns a pseudo-random float in the range [0, 1].
    // You can replace 17 and 19 with any prime numbers you like.
    float h = glm::dot(seed, uv);
    return glm::fract(sin(h) * 43758.5453123);
}

static bool doRegionsCollide(const Region& region1, const Region& region2) {
    if(region1.startPos.x >= region2.startPos.x && region1.startPos.x <= region2.endPos.x){
        if(region1.startPos.y >= region2.startPos.y && region1.startPos.y <= region2.endPos.y){
            return true;
        }
    }

    return false;
}

static bool doesRegionCollideWithRegions(const Region& region, const std::vector<Region>& regions) {
    for (const Region& otherRegion : regions) {
        if (doRegionsCollide(region, otherRegion)) {
            return true; // Collision detected
        }
    }
    return false; // No collision detected with any of the regions
}

static void assertSprite(Texture& srcTxtr, Texture sprite, std::vector<Region>& regions){
    
    for (size_t y = 0; y < srcTxtr.getResolution().x; y++){
        for (size_t x = 0; x < srcTxtr.getResolution().y; x++){
            if(x + sprite.getResolution().x <= srcTxtr.getResolution().x){
                if(y + sprite.getResolution().y <= srcTxtr.getResolution().y){
                    Region reg;
                    reg.sprite = sprite;
                    reg.startPos = glm::ivec2(x, y);
                    reg.endPos = glm::ivec2(x + sprite.getResolution().x, y + sprite.getResolution().y);
                    if(!doesRegionCollideWithRegions(reg, regions)){
                        regions.push_back(reg);
                        return;
                    }
                }
            }
        }
    }

        
    glm::ivec2 srcTxtrRes = srcTxtr.getResolution();
    glm::ivec2 spriteTxtrRes = sprite.getResolution();
    
    GL::bindTexture_2D(srcTxtr.ID, "txtrPack");

    glTexImage2D(
                    GL_TEXTURE_2D, 
                    0, 
                    GL_RGBA, 
                    srcTxtrRes.x + spriteTxtrRes.x, 
                    srcTxtrRes.y + spriteTxtrRes.y, 
                    0, 
                    GL_RGBA, 
                    GL_UNSIGNED_BYTE, 
                    nullptr
                );
    
    assertSprite(srcTxtr, sprite, regions);

    GL::releaseBoundTextures("txtrPack");
}

Texture TexturePack::generateSpriteTexture(){
    Texture txtr = Texture((char*)nullptr, this->textures[0].getResolution().x, this->textures[0].getResolution().y);

    std::vector<Region> regions;

    for (size_t i = 0; i < this->textures.size(); i++)
    {
        assertSprite(txtr, this->textures[i], regions);
    }

    glm::ivec2 txtrRes = txtr.getResolution();

    unsigned int FBO;
    glGenFramebuffers(1,&FBO);
    glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, txtr.ID, 0);
    glViewport(0, 0, txtrRes.x, txtrRes.y);

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ShaderSystem::textureRenderingShader().use();

    for (size_t i = 0; i < regions.size(); i++)
    {
        glm::ivec2 scale = (regions[i].endPos - regions[i].startPos) / glm::ivec2(2);
        glm::ivec2 pos = regions[i].startPos;
        pos.x += scale.x;
        pos.y += scale.y;
        
        ShaderSystem::textureRenderingShader().setMat4("projection", glm::ortho(0.f, (float)txtrRes.x, (float)txtrRes.y, 0.f));
        ShaderSystem::textureRenderingShader().setVec3("pos", glm::vec3(pos, 0.1));
        ShaderSystem::textureRenderingShader().setVec2("scale", scale);
        ShaderSystem::textureRenderingShader().setFloat("rotation", 0.f);
        ShaderSystem::textureRenderingShader().setFloat("opacity", 1.f);
        ShaderSystem::textureRenderingShader().setInt("txtr", GL::bindTexture_2D(regions[i].sprite.ID, "txtrPack"));
        ShaderSystem::textureRenderingShader().setFloat("depthToleranceValue", 0);

        getBox()->draw("TexturePack::generateSpriteTexture : Drawing a region");
    }

    GL::releaseBoundTextures("txtrPack");
    
    Settings::defaultFramebuffer()->FBO.bind();
    glDeleteFramebuffers(1, &FBO);

    //Set the OpenGL viewport to default
        Settings::defaultFramebuffer()->setViewport();

    return txtr;
}

void TexturePack::apply(Texture txtr, float scale, float count, float rotationJitter, float sizeJitter, float opacityJitter, float allignmentCorruption){
    
    if(!this->textures.size())
        return;

    glm::ivec2 txtrRes = txtr.getResolution();
    
    unsigned int FBO;
    glGenFramebuffers(1,&FBO);
    glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, txtr.ID, 0);
    glViewport(0, 0, txtrRes.x, txtrRes.y);

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const int range = 100;

    const float spawnChance = count;

    ShaderSystem::textureRenderingShader().use();

    for (size_t x = 0; x < range; x++)
    {
        for (size_t y = 0; y < range; y++){
            float rand = hash(glm::vec2((float)x / (float)range, (float)y / (float)range), glm::vec2(17.0, 19.0));
            float randX = hash(glm::vec2((float)x / (float)range, (float)y / (float)range), glm::vec2(35.0, 14.0));

            rand = glm::mix((glm::fract((float)x * spawnChance) + glm::fract((float)y * spawnChance)) / 2.f, rand, allignmentCorruption);
            
            if(rand < spawnChance){
                
                int intHash = ((float)randX * (float)range);

                Texture& sprite = this->textures[intHash % this->textures.size()]; 

                glActiveTexture(GL_TEXTURE0);
                ShaderSystem::textureRenderingShader().setMat4("projection", glm::ortho(0.f, (float)range, 0.f, (float)range));
                ShaderSystem::textureRenderingShader().setVec3("pos", glm::vec3((float)x, (float)y, 0.1));
                ShaderSystem::textureRenderingShader().setVec2("scale", glm::vec2(glm::mix((float)intHash / 20.f, range / 20.f, sizeJitter) * (scale / 10.f)));
                ShaderSystem::textureRenderingShader().setFloat("rotation", glm::mix((float)randX * 360.f, 0.f, rotationJitter));
                ShaderSystem::textureRenderingShader().setFloat("opacity", glm::mix((float)randX, 1.f, opacityJitter));
                ShaderSystem::textureRenderingShader().setInt("txtr", GL::bindTexture_2D(sprite.ID, "txtrPack"));

                getBox()->draw("TexturePack::apply : Drawing a sprite");
            }

            GL::releaseBoundTextures("txtrPack");
        }
    }

    Settings::defaultFramebuffer()->FBO.bind();
    glDeleteFramebuffers(1, &FBO);

    //Set the OpenGL viewport to default
        Settings::defaultFramebuffer()->setViewport();
}

