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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <filesystem>
#include <algorithm>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "LibrarySystem/Library.hpp"


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

std::string UTIL::getFileExtension(std::string s) {
    size_t dotIndex = s.find_last_of('.');
    if (dotIndex != std::string::npos) {
        return s.substr(dotIndex + 1);
    }
    return "";
}

std::string UTIL::toLowercase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return result;
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

bool UTIL::deleteFilesInFolder(const std::string folderPath) {
    try
    {
        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file()) {
                std::filesystem::remove(entry.path());
            }
        }
    }
    catch (const std::filesystem::filesystem_error& ex) {
        LGDLOG::start << "ERROR : Filesystem : Location ID 994456 " << ex.what() << LGDLOG::end;
        return false;
    }

    return true;
}

void UTIL::duplicateFolder(const std::string src, const std::string dest){
	
    try
    {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(src)){
            
            std::string file = entry.path().string();  

            std::string dst = dest + UTIL::folderDistinguisher() + UTIL::rmvPath(src, file);
            
            //If a folder then create a folder
            if(!std::filesystem::is_directory(file)){
                if(!std::filesystem::create_directories(dst))
                    LGDLOG::start<< "ERROR : Duplicating folder : " << src << " to " << dest << ". Couldn't create : " << dst <<LGDLOG::end;
            }

            //If a file then duplicate the file
            else{
                if(!std::filesystem::copy_file(file, dst))
                    LGDLOG::start<< "ERROR : Duplicating folder : " << src << " to " << dest << ". Copying file : " << file << " to " << dst << LGDLOG::end;
            }
        }
    }
    catch (const std::filesystem::filesystem_error& ex) {
        LGDLOG::start << "ERROR : Filesystem : Location ID 482555 " << ex.what() << LGDLOG::end;
    }
    
}

bool UTIL::uniqueName(std::string &s, std::vector<std::string> sArray){
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
    
    return isTheSame;
}

int UTIL::findCorrespondingIndex(const std::string& s, const std::vector<std::string> array){
    for (size_t i = 0; i < array.size(); i++)
    {
        if(s == array[i])
            return i;
    }
    
    return 0;
}