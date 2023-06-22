/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UTIL/Util.hpp"
#include "GUI/Elements.hpp"
#include "3D/ThreeD.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>

//Front declerations of the util functions
void readNodeSceneData(std::ifstream &rf, std::vector<Node> &nodeScene, std::vector<Node> &appNodes);

/// @brief 
/// @param path 
/// @param creationDate 
/// @param lastOpenedDate 
/// @param nodeScene 
/// @param appNodes 
/// @return true if is a ligid file
bool Project::readLigidFile(
                                std::string path,
                                time_t &creationDate,
                                time_t &lastOpenedDate, 
                                std::vector<Node> &nodeScene, 
                                std::vector<Node> &appNodes,
                                int& textureRes
                            ){ //Returns true if path is a ligid file
    
    if(path.size()){
        std::ifstream rf(path, std::ios::out | std::ios::binary);
		
        if(!rf) {
            std::cout << "ERROR WHILE READING LIGID FILE! Cannot open file : " << path << std::endl;
            return false;
        }

        //!HEADER

        //!Description
        uint64_t h1 = 0xBBBBBBBB; 
        uint64_t h2 = 0xCA4B6C78; 
        uint64_t h3 = 0x9A9A2C48; 
        
        uint64_t ch1; 
        uint64_t ch2; 
        uint64_t ch3; 

        rf.read(reinterpret_cast<char*>(   &ch1    ),sizeof(uint64_t));
        rf.read(reinterpret_cast<char*>(   &ch2    ),sizeof(uint64_t));
        rf.read(reinterpret_cast<char*>(   &ch3    ),sizeof(uint64_t));

        if(ch1 != h1 || ch2 != h2 || ch3 != h3){
            return false;
        }

        //! Read the creation date
        rf.read(reinterpret_cast<char*>(   &creationDate    ),sizeof(time_t));

        //! Read the last opened date
        rf.read(reinterpret_cast<char*>(   &lastOpenedDate    ),sizeof(time_t));

        //!NodeScene
        readNodeSceneData(rf, nodeScene, appNodes);

        //!Texture resolution
        rf.read(reinterpret_cast<char*>(   &textureRes    ),sizeof(int));
        
        std::cout << textureRes << std::endl;

        
        return true;
    }
}


void readNodeSceneData(std::ifstream &rf, std::vector<Node> &nodeScene, std::vector<Node> &appNodes){
    
    //Read the node size
    uint64_t nodeSize;
    rf.read(reinterpret_cast<char*>(   &nodeSize    )    , sizeof(uint64_t));

    if(nodeSize)
        nodeScene.clear();
    
    //For each node
    for (size_t i = 0; i < nodeSize; i++)
    {
        Node node;

        //Read the node index, (MATERIAL_NODE , MESH_NODE)
        int nodeIndex;
        rf.read(reinterpret_cast<char*>(   &nodeIndex    ), sizeof(int));

        if(nodeIndex == MESH_NODE)
            node = appNodes[0];
        if(nodeIndex == MATERIAL_NODE)
            node = appNodes[1];

        //Read the material ID 
        rf.read(reinterpret_cast<char*>(   &node.materialID    ), sizeof(int));

        //Read the node pos
        rf.read(reinterpret_cast<char*>(   &node.nodePanel.pos    ), sizeof(glm::vec3));

        //Read the IO size
        uint64_t IOSize;
        rf.read(reinterpret_cast<char*>(   &IOSize    )    , sizeof(uint64_t));
        
        //For each IO
        for (size_t IOI = 0; IOI < IOSize; IOI++)
        {   
            //Read the connection size
            uint64_t connectionSize;
            rf.read(reinterpret_cast<char*>(   &connectionSize    )    , sizeof(uint64_t));
            
            //For each connection of the IO
            for (size_t conI = 0; conI < connectionSize; conI++)
            {
                int inputIndex;
                int nodeIndex;
                rf.read(reinterpret_cast<char*>(   &inputIndex    )    , sizeof(int));
                rf.read(reinterpret_cast<char*>(   &nodeIndex     )    , sizeof(int));
            
                // NodeConnection connection(nodeIndex,inputIndex);

                // node.IOs[IOI].connections.push_back(connection);
            }
        }

        nodeScene.push_back(node);
    }
}