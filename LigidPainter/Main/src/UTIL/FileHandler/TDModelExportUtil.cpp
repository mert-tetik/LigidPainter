/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Basic *.obj file exporter

    v - vertex pos
    vt - vertex texture coordinates
    vn - vertex normal vector
    f - face
    usemtl - indicate the material
    # - comment lines

*/

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"

bool containsVertex(const std::vector<Vertex>& vertexArray, const Vertex& targetVertex) {
    for (const auto& vertex : vertexArray) {
        if (vertex.Position == targetVertex.Position &&
            vertex.TexCoords == targetVertex.TexCoords &&
            vertex.Normal == targetVertex.Normal) {
            return true; // Vertex found in the array
        }
    }
    return false; // Vertex not found in the array
}

int getVertexIndex(const std::vector<Vertex>& vertexArray, const Vertex& targetVertex) {
    int i = 0;
    for (const auto& vertex : vertexArray) {
        if (vertex.Position == targetVertex.Position &&
            vertex.TexCoords == targetVertex.TexCoords &&
            vertex.Normal == targetVertex.Normal) {
            return i; // Vertex found in the array
        }
        i++;
    }
    return i; // Vertex not found in the array
}

void seperateUnitedVertices(std::vector<std::vector<Vertex>>& unitedVertices, std::vector<std::vector<Vertex>>& meshVertices, std::vector<std::vector<unsigned int>>& meshIndices){
    // Unique vertices
    for (size_t unMatI = 0; unMatI < unitedVertices.size(); unMatI++)
    {
        meshVertices.push_back({});
        for (size_t i = 0; i < unitedVertices[unMatI].size(); i++)
        {
            if(!containsVertex(meshVertices[unMatI], unitedVertices[unMatI][i])){
                meshVertices[unMatI].push_back(unitedVertices[unMatI][i]);
            }   
        }
    }
    
    // Face indices
    for (size_t unMatI = 0; unMatI < unitedVertices.size(); unMatI++)
    {
        meshIndices.push_back({});
        for (size_t i = 0; i < unitedVertices[unMatI].size(); i++)
        {
            meshIndices[unMatI].push_back(getVertexIndex(meshVertices[unMatI], unitedVertices[unMatI][i]));
        }
    }
}

Model createModel(std::vector<std::vector<Vertex>> meshVertices, std::vector<std::vector<unsigned int>> meshIndices, std::vector<std::string> matTitles){
    Model model;

    for (int i = 0; i < meshVertices.size(); i++)
    {
        std::string matTitle;
        
        if(matTitles.size()){
            if(matTitles[i].size())
                matTitle = matTitles[i];
            else
                matTitle = "DefaultMat";
        }
        else
            matTitle = "DefaultMat";

        model.meshes.push_back(Mesh(meshVertices[i], meshIndices[i], matTitle, i));
    }

    model.newModelAdded = true;

    return model;
}