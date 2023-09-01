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
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, txtr.ID);
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
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &FBO);
    delete[] pixels;
}