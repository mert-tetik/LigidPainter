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

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"

Util::Util(/* args */){}

float Util::getPercent(float value, float percent){
    return value / 100.f * percent;
}
glm::vec2 Util::getPercent(glm::vec2 value, glm::vec2 percent){
    return glm::vec2(value.x / 100.f * percent.x,value.y / 100.f * percent.y);
}
glm::vec3 Util::getPercent(glm::vec3 value, glm::vec3 percent){
    return glm::vec3(value.x / 100.f * percent.x,value.y / 100.f * percent.y,value.z / 100.f * percent.z);
}

std::string Util::getLastWordBySeparatingWithChar(std::string s, char del)
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

std::string Util::removeLastWordBySeparatingWithChar(std::string s, char del)
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

std::string Util::removeExtension(std::string s){
	int i = 1;
	while (s[s.size() - i] != '.'){i++;}
	for (size_t j = 0; j < i; j++){s.pop_back();}
	return s;
}

std::vector<Material> Util::getTheMaterialsConnectedToTheMeshNode(std::vector<Node> &nodeScene,Library &library){
    std::vector<Material> materials;

    //Check all the inputs of the mesh node
    for (size_t i = 0; i < nodeScene[0].IOs.size(); i++)
    {
        int materialID = 1000;
        if(nodeScene[0].IOs[i].connections.size())
            materialID = nodeScene[nodeScene[0].IOs[i].connections[0].nodeIndex].materialID;
        else
            materials.push_back(Material());

        if(materialID != 1000){
            for (size_t matI = 0; matI < library.materials.size(); matI++)
            {
                if(library.materials[matI].ID == materialID)
                    materials.push_back(library.materials[matI]);
            }
        }
    }

    return materials;
}