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
#include "ColorPaletteSystem/ColorPalette.hpp"
#include "LibrarySystem/Library.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>

//forward declerations of the util functions
bool readMeshNodeSceneData(std::ifstream &rf);

#define READ_BITS(var, type, loc)    if(!rf.read(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                                    LGDLOG::start<< "ERROR : Reading ligid file. Failed to read at : " << loc << LGDLOG::end;\
                                                    return false; \
                                                }

#define READ_STR(str)   int strSize; \
                        READ_BITS(strSize, int, "Sring size") \
                        for (size_t i = 0; i < strSize; i++) \
                        {\
                            char c; \
                            READ_BITS(c, char, "String char") \
                            str.push_back(c); \
                        }

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
            LGDLOG::start<< "ERROR : Reading ligid file! Cannot open file : " << path << LGDLOG::end;
            return false;
        }

        // ---------- Description ------------
        uint64_t h1 = 0xBBBBBBBB; 
        uint64_t h2 = 0xCA4B6C78; 
        uint64_t h3 = 0x9A9A2C48; 
        
        uint64_t ch1; 
        uint64_t ch2; 
        uint64_t ch3; 

        READ_BITS(ch1, uint64_t, "Description header 1");
        READ_BITS(ch2, uint64_t, "Description header 2");
        READ_BITS(ch3, uint64_t, "Description header 3");

        if(ch1 != h1 || ch2 != h2 || ch3 != h3){
            LGDLOG::start<< "ERROR : Reading ligid file : Description header is not correct : " << path << LGDLOG::end;
            return false;
        }

        // ---------- Version Number ------------
        uint32_t versionNumber2000 = 2000;   
        uint32_t versionNumber2100 = 2100;   
        uint32_t versionNumber2200 = 2200;   
        
        uint32_t versionNumber; //2200  
        READ_BITS(versionNumber, uint32_t, "Version number");
        
        std::cout << "File Version : " << versionNumber << std::endl;
        
        if(versionNumber != versionNumber2200){
            LGDLOG::start<< "ERROR : Reading ligid file : Invalid version : " << versionNumber << LGDLOG::end; 
            return false;
        }

        // ---------- Dates ------------
        READ_BITS(creationDate, time_t, "Creation date");
        std::cout << "Creation date : " << creationDate << std::endl;

        READ_BITS(lastOpenedDate, time_t, "Last opened date");
        std::cout << "Last opening date : " << lastOpenedDate << std::endl;
        
        // ---------- 3D Model ------------
        std::string modelTitle = "";
        READ_STR(modelTitle)
        std::cout << "Active 3D model : " << modelTitle << std::endl;
        for (size_t i = 0; i < Library::getModelArraySize(); i++)
        {
            if(Library::getModelObj(i).title == modelTitle){
                *getModel() = Library::getModelObj(i);
            }
        }
        
        if(!getModel()->meshes.size())
            getModel()->loadModel("./LigidPainter/Resources/3D Models/sphere.fbx", true);
        
        // ---------- Mesh node scene ------------
        if(!readMeshNodeSceneData(rf))
            return false;

        // ---------- Settings ------------
        READ_BITS(Settings::properties()->textureRes, int, "Texture resolution");
        
        return true;
    }
}

bool readMeshNodeSceneData(std::ifstream &rf){
    
    //Read the node size
    uint64_t nodeSize;
    READ_BITS(nodeSize, uint64_t, "Node count");

    std::cout << "Node count : " << nodeSize << std::endl;

    if(nodeSize)
        NodeScene::clearArray();

    std::vector<std::vector<std::vector<NodeConnection>>> connections;

    //For each node
    for (size_t i = 0; i < nodeSize; i++)
    {

        //Read the node index, (MATERIAL_NODE , MESH_NODE)
        int nodeIndex;
        READ_BITS(nodeIndex, int, "Node index");

        Node node = Node(
                            nodeIndex, 
                            0
                        );

        if(nodeIndex == MESH_NODE)
            node.uploadNewIOs();
        
        else if(nodeIndex == MATERIAL_MASK_NODE || nodeIndex == MATERIAL_ID_NODE)
            node.nodePanel.sections[0].elements[0].button.texture.readTextureData(rf, true);

        if(nodeIndex == MATERIAL_MASK_NODE){
            READ_BITS(node.nodePanel.sections[0].elements[1].rangeBar.value, int, "node.nodePanel.sections[0].elements[1].rangeBar.value");
        }

        else if(nodeIndex == MATERIAL_ID_NODE){
            std::vector<NodeIO> inputs;
            std::vector<NodeIO> outputs;
            std::vector<glm::vec3> palette;
            palette = node.nodePanel.sections[0].elements[0].button.texture.getMaterialIDPalette();

            inputs.push_back(NodeIO("Texture", node.nodePanel.sections[0].elements[0].button, ColorPalette::mainColor,1));

            for (size_t i = 0; i < palette.size(); i++)
            {
                inputs.push_back(NodeIO("Input1",Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1), "Input1", Texture(), 2.f,false)), ColorPalette::mainColor,0));
                inputs[inputs.size() - 1].element.button.color = glm::vec4(palette[i], 1.f);
            }

            outputs.push_back(NodeIO("Output",Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1), "Output", Texture(), 2.f,false)), ColorPalette::mainColor,2));

            node.uploadNewIOs(inputs, outputs);
        }

        //Read the material title 
        std::string matTitle;
        READ_STR(matTitle)
        
        if(nodeIndex == MATERIAL_NODE){
            bool anyMatch = false;
            for (size_t i = 0; i < Library::getMaterialArraySize(); i++)
            {
                if(Library::getMaterialObj(i).title == matTitle){
                    node.nodePanel.sections[0].elements[0].button.texture = Library::getMaterialObj(i).displayingTexture;
                    node.materialID = Library::getMaterialObj(i).uniqueID;
                    node.barButton.text = Library::getMaterialObj(i).title;
                    anyMatch = true;
                }
            }
            if(!anyMatch){
                LGDLOG::start << "ERROR : Reading ligid file : Couldn't match any materials for the material node. Material was renamed or deleted." << LGDLOG::end;
            }
        }

        //Read the node pos
        READ_BITS(node.nodePanel.pos, glm::vec3, "Node panel pos");

        //Read the IO size
        uint64_t IOSize;
        READ_BITS(IOSize, uint64_t, "IOSize");
        
        std::vector<std::vector<NodeConnection>> nodeConnectionsVec;
        
        //For each IO
        for (size_t IOI = 0; IOI < IOSize; IOI++)
        {   
            //Read the connection size
            uint64_t connectionSize;
            READ_BITS(connectionSize, uint64_t, "connectionSize");
            
            std::vector<NodeConnection> inputConnectionsVec;

            //For each connection of the IO
            for (size_t conI = 0; conI < connectionSize; conI++)
            {
                int inputIndex;
                int nodeIndex;
                READ_BITS(inputIndex, int, "inputIndex");
                READ_BITS(nodeIndex, int, "nodeIndex");
            
                inputConnectionsVec.push_back(NodeConnection(nodeIndex, inputIndex));
            }
            nodeConnectionsVec.push_back(inputConnectionsVec);
        }
        
        connections.push_back(nodeConnectionsVec);
        NodeScene::addNode(node);
    }

    // Create the node connections after loading all the nodes
    for (size_t nodeI = 0; nodeI < connections.size(); nodeI++)
    {
        for (size_t inputI = 0; inputI < connections[nodeI].size(); inputI++)
        {
            for (size_t conI = 0; conI < connections[nodeI][inputI].size(); conI++){
                NodeScene::getNode(nodeI)->createConnection(connections[nodeI][inputI][conI].nodeIndex, connections[nodeI][inputI][conI].inputIndex, nodeI, inputI);            
            }
        }
    }

    NodeScene::updateNodeResults(512, -1);

    return true;
}
