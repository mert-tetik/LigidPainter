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

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "NodeSystem/Node/Node.hpp"
#include "SettingsSystem/Settings.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>

//forward declerations of the util functions
void readmeshNodeSceneData(std::ifstream &rf);

//Returns true if path is a ligid file
bool Project::readLigidFile(
                                std::string path,
                                time_t &creationDate,
                                time_t &lastOpenedDate
                            )
{ 
    
    if(path.size()){
        std::ifstream rf(path, std::ios::out | std::ios::binary);
		
        if(!rf) {
            LGDLOG::start<< "ERROR WHILE READING LIGID FILE! Cannot open file : " << path << LGDLOG::end;
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

        //!meshNodeScene
        readmeshNodeSceneData(rf);

        //!Texture resolution
        rf.read(reinterpret_cast<char*>(   &Settings::properties()->textureRes    ),sizeof(int));
        
        return true;
    }
}

void readmeshNodeSceneData(std::ifstream &rf){
    
    //Read the node size
    uint64_t nodeSize;
    rf.read(reinterpret_cast<char*>(   &nodeSize    )    , sizeof(uint64_t));

    if(nodeSize)
        NodeScene::clearArray();

    std::vector<std::vector<std::vector<NodeConnection>>> connections;
    
    //For each node
    for (size_t i = 0; i < nodeSize; i++)
    {

        //Read the node index, (MATERIAL_NODE , MESH_NODE)
        int nodeIndex;
        rf.read(reinterpret_cast<char*>(   &nodeIndex    ), sizeof(int));

        Node node = Node(
                            nodeIndex, 
                            0
                        );

        if(nodeIndex == MESH_NODE)
            node.uploadNewIOs();

        //Read the material ID 
        rf.read(reinterpret_cast<char*>(   &node.materialID    ), sizeof(int));

        //Read the node pos
        rf.read(reinterpret_cast<char*>(   &node.nodePanel.pos    ), sizeof(glm::vec3));

        //Read the IO size
        uint64_t IOSize;
        rf.read(reinterpret_cast<char*>(   &IOSize    )    , sizeof(uint64_t));
        
        std::vector<std::vector<NodeConnection>> nodeConnectionsVec;
        
        //For each IO
        for (size_t IOI = 0; IOI < IOSize; IOI++)
        {   
            //Read the connection size
            uint64_t connectionSize;
            rf.read(reinterpret_cast<char*>(   &connectionSize    )    , sizeof(uint64_t));
            
            std::vector<NodeConnection> inputConnectionsVec;

            //For each connection of the IO
            for (size_t conI = 0; conI < connectionSize; conI++)
            {
                int inputIndex;
                int nodeIndex;
                rf.read(reinterpret_cast<char*>(   &inputIndex    )    , sizeof(int));
                rf.read(reinterpret_cast<char*>(   &nodeIndex     )    , sizeof(int));
            
                inputConnectionsVec.push_back(NodeConnection(nodeIndex, inputIndex));
            }
            nodeConnectionsVec.push_back(inputConnectionsVec);
        }
        
        connections.push_back(nodeConnectionsVec);
        NodeScene::addNode(node);
    }

    /*
    for (size_t nodeI = 0; nodeI < connections.size(); nodeI++)
    {
        for (size_t inputI = 0; inputI < connections[nodeI].size(); inputI++)
        {
            for (size_t conI = 0; conI < connections[nodeI][inputI].size(); conI++){
                NodeScene::getNode(nodeI)->createConnection(connections[nodeI][inputI][conI].nodeIndex, connections[nodeI][inputI][conI].inputIndex, nodeI, inputI);            
            }
        }
    }
    */
}