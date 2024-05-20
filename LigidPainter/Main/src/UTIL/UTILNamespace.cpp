/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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
#include "UTIL/Library/Library.hpp"


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

bool UTIL::duplicateFolder(const std::string src, const std::string dest){
    try
    {
        if(!std::filesystem::exists(src)){
            LGDLOG::start << "ERROR : Folder duplication failed : Source folder doesn't exist! " << src << LGDLOG::end;
            return false;
        }
        
        if(!std::filesystem::exists(dest)){
            if(!std::filesystem::create_directories(dest)){
                LGDLOG::start << "ERROR : Folder duplication failed : Destination folder can't be created! " << dest << LGDLOG::end;
                return false;
            }
        }

        // Copy the contents from the src folder to dest folder recursively
        std::filesystem::copy(src, dest, std::filesystem::copy_options::recursive);
    }
    catch (const std::filesystem::filesystem_error& ex) {
        LGDLOG::start << "ERROR : Filesystem : Location ID 482555 " << ex.what() << LGDLOG::end;
        return false;
    }
    
    return true;
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
        for (size_t i = 0; i < 50; i++)
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

void UTIL::moveFilesToDestination(const std::string& src, const std::string& dst) {
    try {
        // Check if the source directory exists
        if (!std::filesystem::is_directory(src)) {
            LGDLOG::start << "ERROR : Moving files to the destination : Source directory does not exist or is not a directory " << LGDLOG::end;
        }

        // Check if the destination directory exists, and create it if not
        if (!std::filesystem::exists(dst)) {
            std::filesystem::create_directories(dst);
        }

        // Iterate through files in the source directory
        for (const auto& entry : std::filesystem::directory_iterator(src)) {
            if (std::filesystem::is_regular_file(entry)) {
                // Construct the destination file path
                std::filesystem::path destination = std::filesystem::path(dst) / entry.path().filename();

                // Move the file to the destination
                std::filesystem::rename(entry.path(), destination);
            }
        }
    } 
    catch (const std::filesystem::filesystem_error& ex) {
        LGDLOG::start << "ERROR : Filesystem : Location ID 772611 " << ex.what() << LGDLOG::end;
    }
}

std::string UTIL::environmentSpecificAppDataFolderPath(){
    const char* appDataPath = std::getenv("APPDATA");
    
    if (appDataPath) {
        std::string roamingFolderPath = appDataPath;
        roamingFolderPath += "\\";
        return roamingFolderPath;
    } 
    else 
    {
        LGDLOG::start << "ERROR : Unable to access the APPDATA environment variable." << LGDLOG::end;
    }

    return "";
}

void UTIL::correctFolderDistinguishers(std::string& path){
    char folderDist = UTIL::folderDistinguisher();
    for (size_t i = 0; i < path.size(); i++)
    {
        if(path[i] == '\\' || path[i] == '/'){
            path[i] = folderDist;
        }
    }
}

#define ABSOLUTE_DEST destinationFolder + fileTitle + fileExtension

void UTIL::copyFileToFolder(std::string file, std::string folder, int mode){

    if(UTIL::folderDistinguisher() == '\\'){
        for (size_t i = 0; i < file.size(); i++)
        {
            if(file[i] == '/')
                file[i] = '\\';
        }
    }
    else{
        for (size_t i = 0; i < file.size(); i++)
        {
            if(file[i] == '\\')
                file[i] = '/';
        }
    }

    if(!std::filesystem::exists(file)){
        LGDLOG::start << "ERROR : Requested file to copy doesn't exists : " << file << LGDLOG::end;
        return;
    }
    
    if(!std::filesystem::exists(folder)){
        LGDLOG::start << "ERROR : Requested folder to copy doesn't exists : " << folder << LGDLOG::end;
        return;
    }

    if(!std::filesystem::is_directory(folder)){
        LGDLOG::start << "ERROR : Requested folder to copy is not a directory : " << folder << LGDLOG::end;
        return;
    }
    
    if(!std::filesystem::is_regular_file(file)){
        LGDLOG::start << "ERROR : Requested file to copy is not a regular file : " << file << LGDLOG::end;
        return;
    }

    std::string destinationFolder = folder;
    if(destinationFolder[destinationFolder.size()-1] != '/' && destinationFolder[destinationFolder.size()-1] != '\\'){
        destinationFolder.push_back(UTIL::folderDistinguisher());
    }

    std::string fileTitle = UTIL::removeExtension(UTIL::getLastWordBySeparatingWithChar(file, UTIL::folderDistinguisher()));
    std::string fileExtension = UTIL::getLastWordBySeparatingWithChar(file, '.');

    if(fileExtension.size()){
        if(fileExtension[0] != '.'){
            fileExtension.insert(fileExtension.begin(), '.');
        }
    }

    try{
        if(mode == 0){
            if(std::filesystem::exists(ABSOLUTE_DEST)){
                LGDLOG::start << "WARNING! Copy request (Mode 1) aborted! " << fileTitle << " was already existing in the " << destinationFolder << LGDLOG::end; 
                return;
            }
        }
        else if(mode == 2){
            if(std::filesystem::exists(ABSOLUTE_DEST)){
                LGDLOG::start << "WARNING! Copy request (Mode 1)! " << ABSOLUTE_DEST << " was already existing in the " << destinationFolder << " !existing one was deleted!" << LGDLOG::end; 
                std::filesystem::remove(ABSOLUTE_DEST);
            }
        }
        else if(mode == 1){
            while (std::filesystem::exists(ABSOLUTE_DEST))
            {
                fileTitle.push_back('_');
            }
        }

        std::filesystem::copy_file(file, ABSOLUTE_DEST);
    }
    catch (const std::filesystem::filesystem_error& ex) {
        LGDLOG::start << "ERROR : Filesystem : Location ID 784586 " << ex.what() << LGDLOG::end;
    }
}

bool UTIL::createFolderIfDoesntExist(const std::string path){
    try{
        // Path doesn't exist
        if(!std::filesystem::exists(path)){
            // Create the folder if doesn't exists
            if(!std::filesystem::create_directories(path)){
                LGDLOG::start << "ERROR : Creating folder for nonexisting path : Creating directories : " << path << LGDLOG::end;
                return false;
            }
        }
    }
    catch (const std::filesystem::filesystem_error& ex) {
        // Filesystem error 
        LGDLOG::start << "ERROR : Filesystem : Location ID 6132189 " << ex.what() << LGDLOG::end;
        return false;
    }

    return true;
}

float UTIL::new_value_range(float value, float min, float max, float new_min, float new_max) {
    // Calculate the ratio of the value's position in the original range
    // to the size of the original range
    float ratio = (value - min) / (max - min);
    
    // Map the ratio to the new range and return the result
    return new_min + ratio * (new_max - new_min);
}

unsigned int UTIL::get_texture_slot_index(unsigned int slot){
    return slot - GL_TEXTURE0;
} 

bool UTIL::doHaveWordAsFirstWord(const std::string& str, const std::string& first_word) {
    // Find the first non-whitespace character
    auto it = std::find_if(str.begin(), str.end(), [](unsigned char c) { return !std::isspace(c); });
    
    if (it != str.end()) {
        // Extract the first word
        auto start = it;
        auto end = std::find_if(start, str.end(), [](unsigned char c) { return std::isspace(c); });
        std::string firstWord(start, end);
        
        // Convert the first word to uppercase for case-insensitive comparison
        std::transform(firstWord.begin(), firstWord.end(), firstWord.begin(), [](unsigned char c) { return std::toupper(c); });
        
        // Check if the first word is "ERROR"
        return firstWord == first_word;
    }
    return false;
}