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
#include <LigidGL/LigidGL.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <mutex>

#include "3D/Mesh/Mesh.hpp"
#include "UTIL/Settings/Settings.hpp"

static std::vector<Vertex> calculate_height_mapped_vertices(Texture& heightMap, std::vector<Vertex>& originalVertices, float strength);

void Mesh::processHeightMap(){
    if(LigidGL::getBoundContext() == nullptr){
        LGDLOG::start << "ERROR : Mesh::processHeightMap no context is bound" << LGDLOG::end;
        return;
    }
    
    // Calculate the new vertices
    this->vertices = calculate_height_mapped_vertices(this->material_channels.heightMap, this->vertices, getScene()->heightMapStrength);

    // Update the vertex buffers after calculating the new vertices
    this->update_vertex_buffers();
}

static std::vector<Vertex> calculate_height_mapped_vertices(Texture& heightMap, std::vector<Vertex>& originalVertices, float strength){
    std::vector<Vertex> newVertArray = originalVertices;
    
    char* txtr = new char[heightMap.getResolution().x * heightMap.getResolution().y * 4];
    heightMap.getData(txtr);
    
    for (size_t i = 0; i < newVertArray.size(); i++)
    {
        glm::vec2 uv = newVertArray[i].TexCoords;
        glm::ivec2 pixelDest = newVertArray[i].TexCoords * glm::vec2(heightMap.getResolution()); 
        
        int pixelIndex = (pixelDest.y * heightMap.getResolution().x + pixelDest.x) * 4; // Each pixel has 4 components (R, G, B, A)
        if(pixelIndex < heightMap.getResolution().x * heightMap.getResolution().y * 4){
            char red = txtr[pixelIndex];
            char green = txtr[pixelIndex + 1];
            char blue = txtr[pixelIndex + 2];
            char alpha = txtr[pixelIndex + 3]; 
            
            newVertArray[i].Position += glm::vec3(((float)red / 127.f - 0.5) * strength) * newVertArray[i].Normal;
        }
    }
    
    delete[] txtr;

    return newVertArray;
}