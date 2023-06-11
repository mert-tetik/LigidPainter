
#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"

#include <vector>
#include <string>


void Library::uniqueName(std::string &s,std::vector<std::string> sArray){
	    bool isTheSame = false;
	    for (size_t i = 0; i < sArray.size(); i++)
	    {
	    	if(sArray[i] == s){
	    		isTheSame = true;
	    	}
	    }

	    if(isTheSame){
	    	for (size_t i = 0; i < 1000; i++)
	    	{
	    		bool matched = false;
	    		for (size_t txtrI = 0; txtrI < sArray.size(); txtrI++)
	    		{
	    			if(sArray[txtrI] == s + '(' + std::to_string(i) + ')'){
	    				matched = true;
	    			}
	    		}
	    		if(!matched){
	    			s += '(' + std::to_string(i) + ')';
	    			break;
	    		}
	    	}
	    }
    }

    void Library::uniqueNameControl(){
        for (size_t i = 0; i < textures.size(); i++)
        {
            std::vector<std::string> texturesStr;
            for (size_t istr = 0; istr < textures.size(); istr++)
            {
                if(i != istr)
                    texturesStr.push_back(textures[istr].title);
            }

            uniqueName(textures[i].title,texturesStr);    
        }

        for (size_t i = 0; i < materials.size(); i++)
        {
            std::vector<std::string> materialsStr;
            for (size_t istr = 0; istr < materials.size(); istr++)
            {
                if(i != istr)
                    materialsStr.push_back(materials[istr].title);
            }

            uniqueName(materials[i].title,materialsStr);    
        }

        for (size_t i = 0; i < brushes.size(); i++)
        {
            std::vector<std::string> brushesStr;
            for (size_t istr = 0; istr < brushes.size(); istr++)
            {
                if(i != istr)
                    brushesStr.push_back(brushes[istr].title);
            }

            uniqueName(brushes[i].title,brushesStr);    
        }
        
        for (size_t i = 0; i < TDModels.size(); i++)
        {
            std::vector<std::string> TDModelsStr;
            for (size_t istr = 0; istr < TDModels.size(); istr++)
            {
                if(i != istr)
                    TDModelsStr.push_back(TDModels[istr].title);
            }

            uniqueName(TDModels[i].title,TDModelsStr);    
        }
    }