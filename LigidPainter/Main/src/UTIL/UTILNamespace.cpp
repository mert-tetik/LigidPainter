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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <filesystem>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"


float UTIL::getPercent(float value, float percent){
    return value / 100.f * percent;
}
glm::vec2 UTIL::getPercent(glm::vec2 value, glm::vec2 percent){
    return glm::vec2(value.x / 100.f * percent.x,value.y / 100.f * percent.y);
}
glm::vec3 UTIL::getPercent(glm::vec3 value, glm::vec3 percent){
    return glm::vec3(value.x / 100.f * percent.x,value.y / 100.f * percent.y,value.z / 100.f * percent.z);
}

std::string UTIL::getLastWordBySeparatingWithChar(std::string s, char del)
{
	if(s == "" || s.size() == 1 || s.size() == 2)
		return "";

	std::vector<std::string> words;
	std::stringstream ss(s);
	std::string word;
	while (!ss.eof()) {
		std::getline(ss, word, del);
		words.push_back(word);
	}
	return words[words.size() - 1];
}

std::string UTIL::removeLastWordBySeparatingWithChar(std::string s, char del)
{
	for (int i = s.size()-1; i >= 0; i--)
	{
		if(s[i] == del){
			s.erase(s.begin()+i,s.end());
			break;
		}
	}
	return s;
}

std::string UTIL::removeExtension(std::string s){
	int i = 1;
	while (s[s.size() - i] != '.'){i++;}
	for (size_t j = 0; j < i; j++){s.pop_back();}
	return s;
}

std::vector<Material> UTIL::getTheMaterialsConnectedToTheMeshNode(std::vector<Node> &nodeScene,Library &library,int textureRes){
    std::vector<Material> materials;
    
    //Check all the inputs of the mesh node
    for (size_t i = 0; i < nodeScene[0].IOs.size(); i++)
    {
        int materialID = 1000;
        if(nodeScene[0].IOs[i].connections.size())
            materialID = nodeScene[nodeScene[0].IOs[i].connections[0].nodeIndex].materialID;
        else{
			if(textureRes == 0)
            	materials.push_back(Material());
			else
            	materials.push_back(Material(textureRes,"autoGenerated",123456));
		}

        if(materialID != 1000){
            for (size_t matI = 0; matI < library.materials.size(); matI++)
            {
                if(library.materials[matI].uniqueID == materialID)
                    materials.push_back(library.materials[matI]);
            }
        }
    }

    return materials;
}

void UTIL::updateAllTheNodeConnections(std::vector<Node> &nodeScene){
	//Check all the nodes
	for (size_t ii = 0; ii < nodeScene.size(); ii++)
	{
		for (size_t IOI = 0; IOI < nodeScene[ii].IOs.size(); IOI++)
		{
			for (size_t conI = 0; conI < nodeScene[ii].IOs[IOI].connections.size(); conI++)
			{
				NodeConnection connection = nodeScene[ii].IOs[IOI].connections[conI];
				
				//If the input or the node is disapeared 
				if(connection.inputIndex >= nodeScene[connection.nodeIndex].IOs.size() || connection.nodeIndex >= nodeScene.size()){
					//Severe the connection
					nodeScene[ii].IOs[IOI].connections.erase(nodeScene[ii].IOs[IOI].connections.begin() + conI);
					conI--;
				}
			}
		}
	}
}

void UTIL::deleteNode(std::vector<Node>& nodeScene, int index){
	//If the deleted node is a material node
    if(nodeScene[index].nodeIndex == MATERIAL_NODE){
        
        //Remove the related input connections
        for (size_t IOI = 0; IOI < nodeScene[index].IOs.size(); IOI++)
        {
            for (size_t conI = 0; conI < nodeScene[index].IOs[IOI].connections.size(); conI++)
            {
                int nodeI = nodeScene[index].IOs[IOI].connections[conI].nodeIndex; 
                int inputI = nodeScene[index].IOs[IOI].connections[conI].inputIndex; 
                
                //Remove the connection from the connected node/IO
                nodeScene[nodeI].IOs[inputI].connections.clear();
            }
        }
        
        nodeScene.erase(nodeScene.begin() + index);
    }
}


std::string UTIL::rmvPath(std::string startingPath, std::string fullPath) 
{
	// look for starting path in the fullpath
	int index = fullPath.find(startingPath);

	if (index != std::string::npos)
	{
		fullPath = fullPath.erase(0, startingPath.length());
	}

	return fullPath;
}


void UTIL::openInFileExplorer(const char* path) {
#ifdef _WIN32
    // Windows
    std::string command = std::string("explorer ") + path;
    std::system(command.c_str());
#elif __linux__
    // Linux
    std::string command = std::string("xdg-open ") + path;
    std::system(command.c_str());
#elif __APPLE__
    // macOS
    std::string command = std::string("open ") + path;
    std::system(command.c_str());
#else
    // Unsupported platform
    // Add error handling or alternative implementation here
#endif
}

void UTIL::giveUniqueId(int &ID ,const std::vector<int> otherIDs){
	int newID = 0;

    // Find the next available ID that is not in the otherIDs vector
    while (std::find(otherIDs.begin(), otherIDs.end(), newID) != otherIDs.end()) {
        newID++;
    }

    // Assign the unique ID to the reference variable
    ID = newID;
}

void UTIL::giveUniqueId(int &ID ,const std::vector<Texture> textures){
	std::vector<int> IDArray;

	for (size_t i = 0; i < textures.size(); i++)
	{
		IDArray.push_back(textures[i].uniqueId);
	}
	
	giveUniqueId(ID, IDArray);
}

void UTIL::giveUniqueId(int &ID ,const std::vector<Material> materials){
	std::vector<int> IDArray;

	for (size_t i = 0; i < materials.size(); i++)
	{
		IDArray.push_back(materials[i].uniqueID);
	}

	giveUniqueId(ID, IDArray);
}

char* const* UTIL::convertStringArray(const std::vector<std::string> strings){
    char** convertedArray = new char*[strings.size()];

    for (size_t i = 0; i < strings.size(); i++)
    {
        convertedArray[i] = const_cast<char*>(strings[i].c_str());
    }

    return convertedArray;
}

char UTIL::folderDistinguisher(){
	//If the user is on a windows OS
	#if defined(_WIN32) || defined(_WIN64)
		return '\\';
	//If the user is on a unix based OS
	#else
		return '/'; 
	#endif
}

void UTIL::deleteFilesInFolder(const std::string folderPath) {
    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            std::filesystem::remove(entry.path());
        }
    }
}