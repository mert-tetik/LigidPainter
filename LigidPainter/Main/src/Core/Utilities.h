#ifndef LGDUTIL_H
#define LGDUTIL_H

#include<iostream>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include <glm/gtx/string_cast.hpp>

#include "Core/Utilities.h"

#include "stb_image.h"
#include "stb_image_write.h"
#include "stb_image_resize.h"

#include "Core/Texture/Texture.h"
#include "Core/gl.h"

using namespace std;



class Utilities {
public:
double renderingSpeed = 1;

std::string readFile(const char* path) {
	string fullText;
	fstream my_file;

    std::fstream filein(path, std::ios::in);

    if(!filein.is_open()){
        std::cout << "Unable to open the file\n";
        std::cerr << "Error: " << strerror(errno) << std::endl;
    } 
	else {
        std::cout << "Loaded " << path << std::endl;
    }

	std::string res;			
	std::string line;
    while(std::getline(filein,line)){
		std::string processStr;
		for (size_t i = 0; i < line.size(); i++)
		{
			if((int)line[i] <= 127 && (int)line[i] >= 0) //extract non-ascii characters
				processStr+=line[i];
		}
		res += processStr;
		res += '\n';
		line.clear();
	}

	return res;
}
string getLastWordBySeparatingWithChar(string s, char del)
{
	std::vector<std::string> words;
	stringstream ss(s);
	string word;
	while (!ss.eof()) {
		getline(ss, word, del);
		words.push_back(word);
	}
	return words[words.size() - 1];
}
string removeLastWordBySeparatingWithChar(string s, char del)
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
string openFileDialog() { //!!Using Win header!! 

	////!!!! Not used in that project !!!!//
	//
	//OPENFILENAME ofn;       // common dialog box structure
	//char szFile[260];       // buffer for file name
	//HANDLE hf;              // file handle
//
	//// Initialize OPENFILENAME
	//ZeroMemory(&ofn, sizeof(ofn));
	//ofn.lStructSize = sizeof(ofn);
	//ofn.lpstrFile = szFile;
	//// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	//// use the contents of szFile to initialize itself.
	//ofn.lpstrFile[0] = '\0';
	//ofn.nMaxFile = sizeof(szFile);
	//ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
	//ofn.nFilterIndex = 1;
	//ofn.lpstrFileTitle = NULL;
	//ofn.nMaxFileTitle = 0;
	//ofn.lpstrInitialDir = NULL;
	//ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
//
	//// Display the Open dialog box. 
//
	//if (GetOpenFileName(&ofn) == TRUE)
	//	return (string)ofn.lpstrFile;
	//return "";
	return "";
}
std::string rgbToHexGenerator(glm::vec3 rgbData) {
	const char* chars = "abcdef";
	std::string hex = "#";
	for (size_t i = 0; i < 3; i++)
	{
		int left = floor(rgbData[i] / 16);
		int right = rgbData[i] - (16 * left);
		if (left < 10) {
			hex += std::to_string(left);
		}
		else {
			hex += chars[left - 10];
		}

		if (right < 10) {
			hex += std::to_string(right);
		}
		else {
			hex += chars[right - 10];
		}
	}
	return hex;
}

float restrictBetween(float value, float maxVal, float minVal) {
	if (value > maxVal) {
		value = maxVal;
	}
	else if (value < minVal) {
		value = minVal;
	}
	return value;
}


float transitionEffect(bool buttonEnter,float mixVal,float phaseDifference){
	LigidPainter lp;
	float time = lp.getRenderingSpeedInFPS()/2.f;
	if (buttonEnter && mixVal <= 1.0f) {mixVal += 1./(double)(time);}
	else if (!buttonEnter && mixVal >= 0.0f) {mixVal -= 1./(double)(time);}

	if(mixVal < 0.f)
		mixVal = 0.f;
	if(mixVal > 1.f)
		mixVal = 1.f;
		
	return mixVal;
} 

double lastTime = glfwGetTime();
int nbFrames = 0;
void printRenderingSpeed(bool debugMode,GLFWwindow* window) {
	double currentTime = glfwGetTime();
	nbFrames++;
	if (currentTime - lastTime >= 1.0) {
		if(debugMode){
				std::cout << 1000.0 / double(nbFrames) << " ms/frame    ";
				int width;
				int height;
				glfwGetFramebufferSize(window,&width,&height);
				
				int wwidth;
				int wheight;
				glfwGetWindowSize(window,&wwidth,&wheight);

			std::cout << 
			"Primary Monitor Size : " << glfwGetVideoMode(glfwGetPrimaryMonitor())->width << " , " <<glfwGetVideoMode(glfwGetPrimaryMonitor())->height
			<< "    Framebuffer Size : " << width << " , " << height
			<< "    Window Size : " << wwidth << " , " << wheight << std::endl;
		}
		else{
			renderingSpeed = (double)nbFrames;
			std::cout << renderingSpeed << " fps\n";
		}
		nbFrames = 0;
		lastTime += 1.0;
	}
}
glm::vec3 RGBToHSVGenerator(glm::vec3 rgbData){
	float maxV = max(max(rgbData.r,rgbData.g),rgbData.b);
	float minV = min(min(rgbData.r,rgbData.g),rgbData.b);
	float hue;
	float saturation;
	float value;

	value = maxV/255.0;
	if(maxV > 0){
		saturation = 1.0f - float(minV)/float(maxV);
	}
	else if(maxV == 0){
		saturation = 0;
	}
    if (maxV == rgbData.r) {
        hue = (rgbData.g - rgbData.b) / (maxV - minV);

    } else if (maxV == rgbData.g) {
        hue = 2.0f + (rgbData.b - rgbData.r) / (maxV - minV);

    } else {
        hue = 4.0f + (rgbData.r - rgbData.g) / (maxV - minV);
    }
    hue = hue * 60.0;
    if (hue < 0){
		hue = hue + 360.0;	
	} 
	
	if (minV == maxV) {
        hue = 0;
    }
	glm::vec3 hsvVal = glm::vec3(round(hue)/1.41176470588f,saturation*255.0f,value*255.0f); //0 - 255 value range for each component
	return hsvVal;
}
glm::vec3 hexToRGBConverter(std::string hex){ //takes hex : #000000 (# is required) 
	float r;
	float g;
	float b;
	int hexVal[6];
	for (int i = 1; i < 7; i++)
	{
		if(hex[i] == '0'){hexVal[i-1] = 0;}
		if(hex[i] == '1'){hexVal[i-1] = 1;}
		if(hex[i] == '2'){hexVal[i-1] = 2;}
		if(hex[i] == '3'){hexVal[i-1] = 3;}
		if(hex[i] == '4'){hexVal[i-1] = 4;}
		if(hex[i] == '5'){hexVal[i-1] = 5;}
		if(hex[i] == '6'){hexVal[i-1] = 6;}
		if(hex[i] == '7'){hexVal[i-1] = 7;}
		if(hex[i] == '8'){hexVal[i-1] = 8;}
		if(hex[i] == '9'){hexVal[i-1] = 9;}
		if(hex[i] == 'a'){hexVal[i-1] = 10;}
		if(hex[i] == 'b'){hexVal[i-1] = 11;}
		if(hex[i] == 'c'){hexVal[i-1] = 12;}
		if(hex[i] == 'd'){hexVal[i-1] = 13;}
		if(hex[i] == 'e'){hexVal[i-1] = 14;}
		if(hex[i] == 'f'){hexVal[i-1] = 15;}
	}
	
	r = hexVal[0] * 16 + hexVal[1];
	g = hexVal[2] * 16 + hexVal[3];
	b = hexVal[4] * 16 + hexVal[5];
	glm::vec3 result =  glm::vec3(r,g,b);
	return result;
}
std::string cropString(std::string text,int maxSize){
	std::string croppedString;
	bool overload = false;
	for (size_t i = 0; i < text.size(); i++)
	{
		croppedString += text[i];
		if(i == maxSize){overload = true;break;}
	}
	if(overload){
		for (size_t i = 0; i < 3; i++){croppedString += '.';}
	}
	return croppedString;
}

bool shortCut(GLFWwindow* window,unsigned int input1,unsigned int input2,unsigned int input3,unsigned int input4){ //Max 4 inputs

	bool state1 = true;
	bool state2 = true;
	bool state3 = true;
	bool state4 = true;

	if(input1 != NULL){
		if(glfwGetKey(window, input1) == GLFW_PRESS){state1 = true;}else{state1 = false;}
	}
	if(input2 != NULL){
		if(glfwGetKey(window, input2) == GLFW_PRESS){state2 = true;}else{state2 = false;}
	}
	if(input3 != NULL){
		if(glfwGetKey(window, input3) == GLFW_PRESS){state3 = true;}else{state3 = false;}
	}
	if(input4 != NULL){
		if(glfwGetKey(window, input4) == GLFW_PRESS){state4 = true;}else{state4 = false;}
	}

	bool result = state1 && state2 && state3 && state4;

	return result;
}


float keepTheRatio(double value1,double value2, double desiredRatio){
	double maxV = max(value1,value2);
	double minV = min(value1,value2);

	double ratio = (double)maxV / (double)minV;

	double ratioRatio = ratio/desiredRatio;

	double result = maxV / ratioRatio;

	return result;
}

void printError(){
	GLenum err;
	while((err = glGetError()) != GL_NO_ERROR)
	{
	  	if(err == GL_INVALID_ENUM){
			std::cout << "ERROR : GL_INVALID_ENUM";
	  	}
	  	if(err == GL_INVALID_VALUE){
			std::cout << "ERROR : GL_INVALID_VALUE";
		}
		if(err == GL_INVALID_OPERATION){
			std::cout << "ERROR : GL_INVALID_OPERATION";
		}
		if(err == GL_OUT_OF_MEMORY){
			std::cout << "ERROR : GL_OUT_OF_MEMORY";
		}
		if(err == GL_INVALID_FRAMEBUFFER_OPERATION){
			std::cout << "ERROR : GL_INVALID_FRAMEBUFFER_OPERATION";
		}
		std::cout << '\n';
	}
}

std::string removeExtension(std::string s){
	int i = 1;
	while (s[s.size() - i] != '.'){i++;}
	for (size_t j = 0; j < i; j++){s.pop_back();}
	return s;
}

std::string uniqueName(std::string s,std::vector<std::string> albedoTextures){
	bool isTheSame = false;
	for (size_t i = 0; i < albedoTextures.size(); i++)
	{
		if(albedoTextures[i] == s){
			isTheSame = true;
		}
	}

	if(isTheSame){
		for (size_t i = 0; i < 1000; i++)
		{
			bool matched = false;
			for (size_t txtrI = 0; txtrI < albedoTextures.size(); txtrI++)
			{
				if(albedoTextures[txtrI] == s + '(' + std::to_string(i) + ')'){
					matched = true;
				}
			}
			if(!matched){
				s += '(' + std::to_string(i) + ')';
				break;
			}
		}
	}
	return s;
}

bool illegalCharCheck(std::string s){
	for (size_t i = 0; i < s.size(); i++)
	{
		if((int)s[i] <= 127 && (int)s[i] >= 0){}
		else{return true;}
	}
	return false;
}

bool ccw(glm::vec2 A,glm::vec2 B,glm::vec2 C){
    return (C.y-A.y) * (B.x-A.x) > (B.y-A.y) * (C.x-A.x);
}
bool intersect(glm::vec2 A,glm::vec2 B,glm::vec2 C,glm::vec2 D){
   	return (bool)(ccw(A,C,D) != ccw(B,C,D) && ccw(A,B,C) != ccw(A,B,D));
}

bool isMatch(std::string o,std::string t){
	int matchCounter = 0;
	int startIndex = 0;
	if(t == "")
		return false;
	if(t.size() > o.size())
		return false;

	//Convert to lowercase
	for (size_t i = 0; i < o.size(); i++)
	{
		if(isupper(o[i])){
			o[i]+=32;
		}
	}
	for (size_t i = 0; i < t.size(); i++)
	{
		if(isupper(t[i])){
			t[i]+=32;
		}
	}



	for (size_t i = 0; i < o.size(); i++)
	{
		if(t[0] == o[i]){
			startIndex = i;
			break;
		}
	}
	for (size_t i = 0; i < t.size(); i++)
	{
		if(startIndex+i == o.size())
			return false;
		if(t[i] == o[i+startIndex]){
			matchCounter++;
		}
	}
	return matchCounter == t.size();
}


std::vector<std::string> seperateFilePaths(std::string paths,char seperator){
	std::vector<std::string> pathVector;

	std::string path;
	for (size_t i = 0; i < paths.size(); i++)
	{
		if(paths[i] == seperator){
			pathVector.push_back(path);
			path = "";	
		}
		else
			path += paths[i];
	}
	pathVector.push_back(path);
	return pathVector;	
} 

std::vector<glm::vec3> getMirrorVectors(bool x,bool y,bool z){
	std::vector<glm::vec3> vectors;
	if(x)
		vectors.push_back(glm::vec3(-1,1,1));
	if(y)
		vectors.push_back(glm::vec3(1,-1,1));
	if(z)
		vectors.push_back(glm::vec3(1,1,-1));
	
	if(x && y)
		vectors.push_back(glm::vec3(-1,-1,1));
	if(y && z)
		vectors.push_back(glm::vec3(1,-1,-1));
	if(z && x)
		vectors.push_back(glm::vec3(-1,1,-1));
	
	if(x && y && z)
		vectors.push_back(glm::vec3(-1,-1,-1));

	return vectors;
}

char* processLiFile(const char * path,uint64_t& len,int &res){
	
	std::ifstream rf(path, std::ios::binary);

	if(!rf){
		std::cout << "CANT OPEN THE FILE : " << path << std::endl;
		res = 0;
		rf.close();
		return {};
	}

	uint64_t h1 = 0xAFCF438ABFFCFF7E;
	uint64_t hr;
	
	rf.read(reinterpret_cast<char*>(&hr), sizeof(uint64_t));

	if(h1 != hr){
		res = 0;
		rf.close();
		return {};
	}
	else{
		rf.read(reinterpret_cast<char*>(&len), sizeof(uint64_t));
        char* data = new char[len];
        rf.read(data, len);
		res = 1;
		rf.close();
		return data;
	}
}

long long getFolderSizeInBytes(std::string path){
	long long folderSize = 0;

	std::vector<std::string> folders;
	folders.push_back(path);
    for (size_t i = 0; i < folders.size(); i++)
	{
		for (const auto & entry : std::filesystem::directory_iterator(folders[i])){
			std::string filePath = entry.path().string();

			if(std::filesystem::is_directory(filePath))
				folders.push_back(filePath);
			else
				folderSize += std::filesystem::file_size(filePath);
		}
	}
	return folderSize;
}

int getElementSizeOfTheFolder(std::vector<int> albedoTextures,int folderIndex){
	int counter = 0;
	for (size_t i = 0; i < albedoTextures.size(); i++)
	{
		if(albedoTextures[i] == folderIndex)
			counter++;
	}
	return counter;
}

bool extensionCheckForTexture(std::string &name){
	
	//JPG || PNG
	bool isPNG = false;
	
	if(name[name.size()-1] == 'g' || name[name.size()-1] == 'G')
		if(name[name.size()-2] == 'n' || name[name.size()-2] == 'N')		
			if(name[name.size()-3] == 'p' || name[name.size()-3] == 'P')		
				if(name[name.size()-4] == '.')		
					isPNG = true;
	
	if(!isPNG){
		int i;
		
		for (i = 0; i < name.size(); i++)
		{
			if(name[i] == '.')
				break;
		}
		
		if(i != name.size()-1)
			name.erase(name.begin()+i,name.end());
		
		name += ".png"; 
	}
	return true;
}
bool checkIfPNG(std::string name){
	
	//JPG || PNG
	bool isPNG = false;
	
	if(name[name.size()-1] == 'g' || name[name.size()-1] == 'G')
		if(name[name.size()-2] == 'n' || name[name.size()-2] == 'N')		
			if(name[name.size()-3] == 'p' || name[name.size()-3] == 'P')		
				if(name[name.size()-4] == '.')		
					isPNG = true;
	return isPNG;
}

bool checkIfTextureIsInsideOfAFolderUsingFolderName(aTexture &texture,std::vector<aTexture> textures,std::string folderName){
	if(texture.folderIndex == 10000)
		return false;
	return (textures[texture.folderIndex].name == folderName);
}

std::string getTMPFilePath(aTexture texture,int txtrIndex,std::vector<aTexture> albedoTextures){
	std::string filePath = "./tmp/";
	if(texture.folderIndex != 10000){
		if(albedoTextures[texture.folderIndex].folderIndex != 10000){
			if(albedoTextures[albedoTextures[texture.folderIndex].folderIndex].folderIndex != 10000){
				if(albedoTextures[albedoTextures[albedoTextures[texture.folderIndex].folderIndex].folderIndex].folderIndex != 10000){
					if(albedoTextures[albedoTextures[albedoTextures[albedoTextures[texture.folderIndex].folderIndex].folderIndex].folderIndex].folderIndex != 10000){
						if(albedoTextures[albedoTextures[albedoTextures[albedoTextures[albedoTextures[texture.folderIndex].folderIndex].folderIndex].folderIndex].folderIndex].folderIndex != 10000){
							//6 Folder
							filePath += (std::to_string(albedoTextures[albedoTextures[albedoTextures[albedoTextures[albedoTextures[albedoTextures[texture.folderIndex].folderIndex].folderIndex].folderIndex].folderIndex].folderIndex].folderIndex));
							filePath += '_';
						}
						//5 Folder
						filePath+=(std::to_string(albedoTextures[albedoTextures[albedoTextures[albedoTextures[albedoTextures[texture.folderIndex].folderIndex].folderIndex].folderIndex].folderIndex].folderIndex));
						filePath += '_';
					}
					//4 Folder
					filePath+=(std::to_string(albedoTextures[albedoTextures[albedoTextures[albedoTextures[texture.folderIndex].folderIndex].folderIndex].folderIndex].folderIndex));
					filePath += '_';
				}
				//3 Folder
				filePath+=(std::to_string(albedoTextures[albedoTextures[albedoTextures[texture.folderIndex].folderIndex].folderIndex].folderIndex));
				filePath += '_';
			}
			//2 Folder
			filePath+=(std::to_string(albedoTextures[albedoTextures[texture.folderIndex].folderIndex].folderIndex));
			filePath += '_';
		}
		//1 Folder
		filePath+=(std::to_string(albedoTextures[texture.folderIndex].folderIndex));
		filePath += '_';
	}
	//0 Folder
	filePath+=(std::to_string(texture.folderIndex));
	filePath += '_';
	filePath+=std::to_string(txtrIndex);

	return filePath;
}
bool writeTMPFile(aTexture texture,int txtrIndex,std::vector<aTexture> albedoTextures){
	std::string filePath = getTMPFilePath(texture,txtrIndex,albedoTextures);


	#if defined(_WIN32) || defined(_WIN64)
    	char folderDistinguisher = '\\';
	#else
		char folderDistinguisher = '/'; 
	#endif

	int historyI = 0;
	for (const auto & entry : std::filesystem::directory_iterator("./tmp")){
		std::string tmpFilePath = entry.path().string();
		std::string orgFile = filePath;		
		tmpFilePath = removeLastWordBySeparatingWithChar(tmpFilePath,'-');
		tmpFilePath = getLastWordBySeparatingWithChar(tmpFilePath,folderDistinguisher);
		orgFile = getLastWordBySeparatingWithChar(orgFile,'/');

		if(tmpFilePath == orgFile)
			historyI++;
	}
	
	filePath += '-';
	filePath += std::to_string(historyI);
	filePath+=".tmp";
    
	std::ofstream wf = std::ofstream(filePath, std::ios::out | std::ios::binary);

	if(!std::filesystem::is_directory("./tmp"))
		std::filesystem::create_directories("./tmp");
	
	if(!wf){

		std::cout << "ERROR While creating the tmp file" << std::endl;
		return false;
	}

	glActiveTexture(GL_TEXTURE28);
	GlSet glset;
	glset.bindTexture(texture.id);
	
	char* pixels = new char[texture.width*texture.height*4];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_BYTE, pixels); 
	
    if(!wf.write(reinterpret_cast<char*>(&texture.width),sizeof(int))){
		std::cout << "ERROR While writing the tmp texture file's width data" << std::endl;
		return false;
	}
    if(!wf.write(reinterpret_cast<char*>(&texture.height),sizeof(int))){
		std::cout << "ERROR While writing the tmp texture file's height data" << std::endl;
		return false;
	}

	if(!wf.write(pixels , texture.width*texture.height*4)){
		std::cout << "ERROR While writing the tmp file (can't write the texture data)" << std::endl;
		return false;
	}	

	delete[] pixels;
	return true;
}
bool readTMPFile(aTexture texture,int txtrIndex,std::vector<aTexture> albedoTextures){
	std::string filePath = getTMPFilePath(texture,txtrIndex,albedoTextures);

	#if defined(_WIN32) || defined(_WIN64)
    	char folderDistinguisher = '\\';
	#else
		char folderDistinguisher = '/'; 
	#endif

	
	int historyI = 0;
	for (const auto & entry : std::filesystem::directory_iterator("./tmp")){
		std::string tmpFilePath = entry.path().string();
		std::string orgFile = filePath;		
		tmpFilePath = removeLastWordBySeparatingWithChar(tmpFilePath,'-');
		tmpFilePath = getLastWordBySeparatingWithChar(tmpFilePath,folderDistinguisher);
		orgFile = getLastWordBySeparatingWithChar(orgFile,'/');

		if(tmpFilePath == orgFile)
			historyI++;
	}
	
	if(historyI == 0)
		return false;
	
	filePath += '-';
	filePath += std::to_string(historyI-1);
	filePath+=".tmp";
    
	std::ifstream rf = std::ifstream(filePath, std::ios::binary);

	if(!rf){
		std::cout << "ERROR While opening the tmp file : " << filePath << std::endl;
		return false;
	}
	
	int width;
	int height;
    if(!rf.read(reinterpret_cast<char*>(&width),sizeof(int))){
		std::cout << "ERROR While reading the tmp texture file's width data" << std::endl;
		return false;
	}
    if(!rf.read(reinterpret_cast<char*>(&height),sizeof(int))){
		std::cout << "ERROR While reading the tmp texture file's height data" << std::endl;
		return false;
	}
	
	char* pixels = new char[width * height * 4];
	
	if(!rf.read(pixels , width * height * 4)){
		std::cout << "ERROR While reading the tmp file (can't write the texture data)" << std::endl;
		return false;
	}	

	rf.close();
	std::filesystem::remove(filePath);

	glActiveTexture(GL_TEXTURE28);
	GlSet glset;
	glset.bindTexture(texture.id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_BYTE, pixels);

	delete[] pixels;
	return true;
}
};
#endif