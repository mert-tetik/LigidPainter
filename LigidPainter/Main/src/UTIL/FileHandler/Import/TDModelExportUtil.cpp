/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

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

#include <zlib/zlib.h>

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
    
    float big = 1.f;
    for (size_t i = 0; i < meshVertices.size(); i++)
    {
        for (size_t vi = 0; vi < meshVertices[i].size(); vi++)
        {
            if(big < abs(meshVertices[i][vi].Position.x))
                big = abs(meshVertices[i][vi].Position.x);
            
            if(big < abs(meshVertices[i][vi].Position.y))
                big = abs(meshVertices[i][vi].Position.y);
            
            if(big < abs(meshVertices[i][vi].Position.z))
                big = abs(meshVertices[i][vi].Position.z);
        }
    }
    
    
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

        
        //Make the material/mesh title/name unique
        std::vector<std::string> meshTitles;
        for (size_t i = 0; i < model.meshes.size(); i++)
            meshTitles.push_back(model.meshes[i].materialName);
        
        UTIL::uniqueName(matTitle, meshTitles);
        

        if(meshVertices[i].size())
            model.meshes.push_back(Mesh(meshVertices[i], meshIndices[i], matTitle));
    }

    model.newModelAdded = true;
    return model;
}

void calculateTangentBitangent(Vertex& v0, Vertex& v1, Vertex& v2) {
    // Calculate posiztion differences
    glm::vec3 deltaPos1 = v1.Position - v0.Position;
    glm::vec3 deltaPos2 = v2.Position - v0.Position;

    // Calculate texture coordinate differences
    glm::vec2 deltaUV1 = v1.TexCoords - v0.TexCoords;
    glm::vec2 deltaUV2 = v2.TexCoords - v0.TexCoords;

    // Calculate the reciprocal of the determinant
    float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

    // Calculate tangent and bitangent
    glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
    glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

    // Accumulate tangent and bitangent vectors
    v0.Tangent += tangent;
    v1.Tangent += tangent;
    v2.Tangent += tangent;

    v0.Bitangent += bitangent;
    v1.Bitangent += bitangent;
    v2.Bitangent += bitangent;
}

std::vector<char> DecompressZlibChar(const std::vector<char>& compressedData, size_t uncompressedSize) {
    
    std::vector<char> uncompressedData(uncompressedSize, 0);
    
    z_stream stream;
    stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressedData.data()));
    stream.avail_in = static_cast<uInt>(compressedData.size());
    stream.next_out = reinterpret_cast<Bytef*>(uncompressedData.data());
    stream.avail_out = static_cast<uInt>(uncompressedData.size());

    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    if (inflateInit(&stream) != Z_OK) {
        std::cerr << "Failed to initialize zlib for decompression." << LGDLOG::end;
        return {};
    }

    // Decompress the data
    int result = inflate(&stream, Z_FINISH);
    inflateEnd(&stream);

    if (result != Z_STREAM_END) {
        std::cerr << "Failed to decompress zlib data. Error code: " << result << LGDLOG::end;
        return {};
    }

    return uncompressedData;
}

std::vector<float> DecompressZlibFloat(const std::vector<char>& compressedData, size_t numFloats) {
    std::vector<float> uncompressedFloats(numFloats);

    z_stream stream;
    stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressedData.data()));
    stream.avail_in = static_cast<uInt>(compressedData.size());
    stream.next_out = reinterpret_cast<Bytef*>(uncompressedFloats.data());
    stream.avail_out = static_cast<uInt>(uncompressedFloats.size() * sizeof(float));

    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    if (inflateInit(&stream) != Z_OK) {
        std::cerr << "Failed to initialize zlib for decompression." << LGDLOG::end;
        return {};
    }

    // Decompress the data
    int result = inflate(&stream, Z_FINISH);
    inflateEnd(&stream);

    if (result != Z_STREAM_END) {
        std::cerr << "Failed to decompress zlib data. Error code: " << result << LGDLOG::end;
        return {};
    }

    // Calculate the actual number of floats decompressed
    size_t numDecompressedFloats = stream.total_out / sizeof(float);
    uncompressedFloats.resize(numDecompressedFloats);

    return uncompressedFloats;
}

std::vector<double> DecompressZlibDouble(const std::vector<char>& compressedData, size_t numDoubles) {
    std::vector<double> uncompressedDoubles(numDoubles);

    z_stream stream;
    stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressedData.data()));
    stream.avail_in = static_cast<uInt>(compressedData.size());
    stream.next_out = reinterpret_cast<Bytef*>(uncompressedDoubles.data());
    stream.avail_out = static_cast<uInt>(uncompressedDoubles.size() * sizeof(double));

    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    if (inflateInit(&stream) != Z_OK) {
        std::cerr << "Failed to initialize zlib for decompression." << LGDLOG::end;
        return {};
    }

    // Decompress the data
    int result = inflate(&stream, Z_FINISH);
    inflateEnd(&stream);

    if (result != Z_STREAM_END) {
        std::cerr << "Failed to decompress zlib data. Error code: " << result << LGDLOG::end;
        return {};
    }

    // Calculate the actual number of doubles decompressed
    size_t numDecompressedDoubles = stream.total_out / sizeof(double);
    uncompressedDoubles.resize(numDecompressedDoubles);

    return uncompressedDoubles;
}

std::vector<long long> DecompressZlibLongLong(const std::vector<char>& compressedData, size_t numLongLongs) {
    std::vector<long long> uncompressedLongLongs(numLongLongs);

    z_stream stream;
    stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressedData.data()));
    stream.avail_in = static_cast<uInt>(compressedData.size());
    stream.next_out = reinterpret_cast<Bytef*>(uncompressedLongLongs.data());
    stream.avail_out = static_cast<uInt>(uncompressedLongLongs.size() * sizeof(long long));

    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    if (inflateInit(&stream) != Z_OK) {
        std::cerr << "Failed to initialize zlib for decompression." << LGDLOG::end;
        return {};
    }

    // Decompress the data
    int result = inflate(&stream, Z_FINISH);
    inflateEnd(&stream);

    if (result != Z_STREAM_END) {
        std::cerr << "Failed to decompress zlib data. Error code: " << result << LGDLOG::end;
        return {};
    }

    // Calculate the actual number of long longs decompressed
    size_t numDecompressedLongLongs = stream.total_out / sizeof(long long);
    uncompressedLongLongs.resize(numDecompressedLongLongs);

    return uncompressedLongLongs;
}

std::vector<int> DecompressZlibInt(const std::vector<char>& compressedData, size_t numInts) {
    std::vector<int> uncompressedInts(numInts);

    z_stream stream;
    stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressedData.data()));
    stream.avail_in = static_cast<uInt>(compressedData.size());
    stream.next_out = reinterpret_cast<Bytef*>(uncompressedInts.data());
    stream.avail_out = static_cast<uInt>(uncompressedInts.size() * sizeof(int));

    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    if (inflateInit(&stream) != Z_OK) {
        std::cerr << "Failed to initialize zlib for decompression." << LGDLOG::end;
        return {};
    }

    // Decompress the data
    int result = inflate(&stream, Z_FINISH);
    inflateEnd(&stream);

    if (result != Z_STREAM_END) {
        std::cerr << "Failed to decompress zlib data. Error code: " << result << LGDLOG::end;
        return {};
    }

    // Calculate the actual number of ints decompressed
    size_t numDecompressedInts = stream.total_out / sizeof(int);
    uncompressedInts.resize(numDecompressedInts);

    return uncompressedInts;
}