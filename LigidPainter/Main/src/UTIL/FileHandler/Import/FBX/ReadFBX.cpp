/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    IMPLEMENTED FROM THE BLENDER'S FBX FILE FORMAT DOCUMENTATION

    https://code.blender.org/2013/08/fbx-binary-file-format-specification/#binary-file-structure

    TODO : Process objects

*/

// GL Math
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

// System
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>

// LigidPainter
#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"

struct FbxProperty {
    char typeCode;
    std::vector<char> data;

    double singleDoubleVal = 0.;
    int singleIntVal = 0;
    long singleLongVal = 0;
};

struct FbxNode {
    std::string nodeType;
    std::vector<FbxProperty> properties;
    std::vector<FbxNode> nestedNodes;
};

struct FbxConnection {
    long startID = LONG_MAX; // Mesh
    long destionationID = LONG_MAX; // Material
};

struct FBXTransform{
    glm::vec3 translation = glm::vec3(FLT_MAX); //Lcl Translation  
    glm::vec3 rotation = glm::vec3(FLT_MAX); //Lcl Rotation
    glm::vec3 scale = glm::vec3(FLT_MAX); //Lcl Scaling

    std::string currentReadMode = ""; 
};

/*
*   0 : Don't print anything to the terminal
*   1 : Print node titles
*   2 : Print node titles with the property data
*/
#define LIGID_FBX_IMPORTER_PRINT_TEXT_STATE 0
#define LIGID_FBX_IMPORTER_TRIANGULATE true

// Forward declarations for the utilities
Model createModel(std::vector<std::vector<Vertex>> meshVertices, std::vector<std::vector<unsigned int>> meshIndices, std::vector<std::string> matTitles);
void seperateUnitedVertices(std::vector<std::vector<Vertex>>& unitedVertices, std::vector<std::vector<Vertex>>& meshVertices, std::vector<std::vector<unsigned int>>& meshIndices);
void calculateTangentBitangent(Vertex& v0, Vertex& v1, Vertex& v2);

int __node_counter = 0;
int _FBX_totalBitsRead = 0;
int objectI = -1;
int modelI = -1;
bool processingMesh = false;
bool processingShape = false;

// FBX 
/* Defines the ReadNestedNodes function */
#include "UTIL/FileHandler/Import/FBX/ReadNestedNodes.hpp"
/* Defines the ProcessNodeHierarchy function */
#include "UTIL/FileHandler/Import/FBX/ProcessNodeHierarchy.hpp"
/* Defines the parseFBXMeshData function */
#include "UTIL/FileHandler/Import/FBX/ParseMeshData.hpp"

/*
    HEADER

    Bytes 0 - 20: Kaydara FBX Binary  \x00 (file-magic, with 2 spaces at the end, then a NULL terminator).
    Bytes 21 - 22: [0x1A, 0x00] (unknown but all observed files show these bytes).
    Bytes 23 - 26: unsigned int, the version number. 7300 for version 7.3 for example.
*/
Model FileHandler::readFBXFile(std::string path) {
    _FBX_totalBitsRead = 0;
    __node_counter = 0;
    objectI = -1;
    modelI = -1;
    processingMesh = false;


    std::ifstream file(path, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "ERROR : Can't read FBX file. Can't open : " << path << LGDLOG::end;
        return Model();
    }

    // Read header
    char header[27];
    file.read(header, sizeof(header));
    _FBX_totalBitsRead += sizeof(header);

    // Extract the version number
    uint32_t version = *reinterpret_cast<uint32_t*>(&header[23]);

    if  ( /*      Supported Versions      */
            //version != 6000 && 
            version != 7100 &&
            version != 7200 &&
            version != 7300 &&
            version != 7400 
            //version != 7500 
        )
    {
        LGDLOG::start<< "ERROR Reading fbx file : unsupported fbx version : " << version << LGDLOG::end;
        return Model();
    }

    // Read top-level object record
    FbxNode topLevelObject;
    for (size_t i = 0; i < 10; i++)
    {
        ReadNestedNodes(file, path, topLevelObject.nestedNodes);
    }

    // Read header
    //file.read(header, sizeof(header));
    //_FBX_totalBitsRead += sizeof(header);

    std::vector<std::vector<glm::vec3>> positions;
    std::vector<std::vector<glm::vec2>> UVS;
    std::vector<std::vector<glm::vec3>> normals;
    std::vector<std::string> matTitles;
    std::vector<std::vector<int>> polygonVertexIndices;
    std::vector<std::vector<int>> edges;
    std::vector<std::vector<int>> uvIndices;
    std::vector<std::vector<int>> materials;
    std::vector<FBXTransform> transforms;
    std::vector<long> meshIDS;
    std::vector<long> materialIDS;
    std::vector<FbxConnection> connections;

    // Process the FBX data
    ProcessNodeHierarchy(topLevelObject.nestedNodes, positions, UVS, normals, polygonVertexIndices , uvIndices, edges, matTitles, materials, transforms, meshIDS, materialIDS, connections);

    for (size_t i = 0; i < connections.size(); i++)
    {
        std::cout << "CONNECTION START " << connections[i].startID << " CONNECTION DESTINATION " << connections[i].destionationID << std::endl;
    }

    for (size_t i = 0; i < meshIDS.size(); i++)
    {
        std::cout << "MESH ID " << meshIDS[i] << std::endl;
    }
    
    for (size_t i = 0; i < materialIDS.size(); i++)
    {
        std::cout << "MATERIAL ID " << materialIDS[i] << std::endl;
    }
    
    

    if(!positions.size() || !edges.size() || !polygonVertexIndices.size() || !uvIndices.size()){
        LGDLOG::start<< "ERROR : Processing the fbx node hierarchy : Can't detect enough vertex data to create a mesh" << LGDLOG::end;
        return Model();
    }

    std::vector<std::vector<Vertex>> meshVertices;
    std::vector<std::vector<unsigned int>> meshIndices;

    if(!matTitles.size())
        matTitles.push_back("FirstDefMat");

    for (size_t i = 0; i < matTitles.size(); i++)
    {
        meshVertices.push_back({});
        meshIndices.push_back({});
    }
    
    for (size_t i = 0; i < objectI + 1; i++)
    {
        std::vector<std::vector<Vertex>> in_meshVertices;
        std::vector<std::vector<unsigned int>> in_meshIndices;
        parseFBXMeshData(  
                            positions[i],
                            UVS[i],
                            normals[i],
                            polygonVertexIndices[i],
                            uvIndices[i],
                            edges[i],
                            matTitles,
                            materials[i],
                            in_meshVertices,
                            in_meshIndices,
                            transforms[i],
                            meshIDS[i],
                            materialIDS,
                            connections
                        );

        
        for (size_t i = 0; i < in_meshIndices.size(); i++)
        {
            for (size_t ii = 0; ii < in_meshIndices[i].size(); ii++)
            {
                meshIndices[i].push_back(in_meshIndices[i][ii] + meshVertices[i].size());
            }
        }
        for (size_t i = 0; i < in_meshVertices.size(); i++)
        {
            for (size_t ii = 0; ii < in_meshVertices[i].size(); ii++)
            {
                meshVertices[i].push_back(in_meshVertices[i][ii]);
            }
        }
    }
    


    if(meshVertices.size()){
        return createModel(meshVertices, meshIndices, matTitles);
    }
    else{
        LGDLOG::start<< "WARNING! Reading FBX file : No mesh data received" << LGDLOG::end;
        return Model();
    }
}









/*
            SEPERATED

    5 5 9 9 9 7
    Pos :
    vec3(-1.000000, -1.000000, 0.000000) vec3(1.000000, -1.000000, 0.000000) vec3(-1.000000, 1.000000, 0.000000) vec3(-0.208482, -0.552366, 1.007192) vec3(-0.211123, -0.559847, 1.005384)    
    
    UV :
    vec2(0.220108, 0.284449) vec2(0.862011, 0.284449) vec2(0.453018, 0.027691) vec2(0.778371, 0.871778) vec2(0.220108, 0.926352)
    
    Normal :
    vec3(0.000000, 0.000000, 1.000000) vec3(0.000000, 0.000000, 1.000000) vec3(0.000000, 0.000000, 1.000000) vec3(-0.623704, -0.623704, -0.471155) vec3(-0.623704, -0.623704, -0.471155) vec3(-0.623704, -0.623704, -0.471155) vec3(0.000000, 0.916058, -0.401046) vec3(0.000000, 0.916058, -0.401046) vec3(0.000000, 0.916058, -0.401046)
    
    Indices :
    1 2 -1   2 1 -4   1 0 -5
    
    UVIndices :
    0 1 2 4 5 7 8
    
    Edges :
    1 4 0  4 1 3  1 0 2


        UNIT

    4 5 9 9 9 6
    Pos :
    vec3(-1.000000, -1.000000, 0.000000) vec3(1.000000, -1.000000, 0.000000) vec3(-1.000000, 1.000000, 0.000000) vec3(-0.209803, -0.556107, 1.006288)

    UV :
    vec2(0.220108, 0.284449) vec2(0.862011, 0.284449) vec2(0.778371, 0.871778) vec2(0.453018, 0.027691) vec2(0.220108, 0.926352)

    Normal :
    vec3(0.000000, 0.000000, 1.000000) 
    vec3(0.000000, 0.000000, 1.000000) 
    vec3(0.000000, 0.000000, 1.000000) 
    vec3(-0.622656, -0.622656, -0.473918) 
    vec3(-0.622656, -0.622656, -0.473918) 
    vec3(-0.622656, -0.622656, -0.473918) 
    vec3(0.000000, 0.914937, -0.403597) 
    vec3(0.000000, 0.914937, -0.403597) 
    vec3(0.000000, 0.914937, -0.403597)

    Indices :
    1 2 -1   2 1 -4   1 0 -4

    UVIndices :
    0 1 2 4 5 7

    Edges :
    1 4 0   4 1 2   1 0 3

*/







/*
        PLANE FBX
    
    4 4 6 6 6 5
    
    Pos :
    vec3(-1.000000, 0.000000, -1.000000)    vec3(1.000000, 0.000000, -1.000000) vec3(-1.000000, 0.000000, 1.000000) vec3(1.000000, 0.000000, 1.000000)

    UV :
    vec2(1.000000, 0.000000)                vec2(0.000000, 0.000000)            vec2(0.000000, 1.000000)            vec2(1.000000, 1.000000)

    Normal :
    vec3(0.000000, 0.000000, 1.000000)      vec3(0.000000, 0.000000, 1.000000)  vec3(0.000000, 0.000000, 1.000000)  vec3(0.000000, 0.000000, 1.000000)      vec3(0.000000, 0.000000, 1.000000)  vec3(0.000000, 0.000000, 1.000000)

    Indices :
    1 2 0   1 3 2

    UVIndices :
    0 1 2 3 4

    Edges :
    0 2 1 0 3 2

        PLANE OBJ



    Pos :
    vec3(1.000000, 0.000000, 1.000000)  vec3(-1.000000, 0.000000, -1.000000)    vec3(-1.000000, 0.000000, 1.000000) vec3(1.000000, 0.000000, -1.000000)
    
    UV :
    vec2(1.000000, 0.000000)            vec2(0.000000, 1.000000)                vec2(0.000000, 0.000000)            vec2(1.000000, 1.000000)
    
    Normal :
    vec3(0.000000, 1.000000, 0.000000)  vec3(0.000000, 1.000000, 0.000000)      vec3(0.000000, 1.000000, 0.000000)  vec3(0.000000, 1.000000, 0.000000)
    
    Indices :
    0 1 2   0 3 1

*/

/*
    PLANE :
    
    Pos :
    vec3(-1.000000, -1.000000, 0.000000) vec3(1.000000, -1.000000, 0.000000) vec3(-1.000000, 1.000000, 0.000000) vec3(1.000000, 1.000000, 0.000000)

    UV :
    vec2(0.000000, 1.000000) vec2(1.000000, 0.000000) vec2(0.000000, 0.000000) vec2(1.000000, 1.000000)

    Normal :
    vec3(0.000000, 0.000000, 1.000000) vec3(0.000000, 0.000000, 1.000000) vec3(0.000000, 0.000000, 1.000000) vec3(0.000000, 0.000000, 1.000000)

    Indices :
    0 1 3 -3

    UVIndices :
    0 1 2 3

    Edges :
    2 1 3 0


    TRIANGULATED PLANE :

    Pos :
    vec3(-1.000000, -1.000000, 0.000000) vec3(1.000000, -1.000000, 0.000000) vec3(-1.000000, 1.000000, 0.000000) vec3(1.000000, 1.000000, 0.000000)

    UV :
    vec2(1.000000, 0.000000) vec2(0.000000, 0.000000) vec2(0.000000, 1.000000) vec2(1.000000, 1.000000)

    Normal :
    vec3(0.000000, 0.000000, 1.000000) vec3(0.000000, 0.000000, 1.000000) vec3(0.000000, 0.000000, 1.000000) vec3(0.000000, 0.000000, 1.000000) vec3(0.000000, 0.000000, 1.000000) vec3(0.000000, 0.000000, 1.000000)

    Indices :
    1 2 -1 1 3 -3

    UVIndices :
    0 1 2 3 4

    Edges :
    0 2 1 0 3 2
*/