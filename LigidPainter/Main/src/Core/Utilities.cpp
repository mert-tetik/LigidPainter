#include<iostream>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include <glm/gtx/string_cast.hpp>

#include "Core/Utilities.h"

#include "stb_image.h"
#include "stb_image_write.h"

using namespace std;

std::string Utilities::readFile(const char* path) {
	string fullText;
	fstream my_file;
	my_file.open(path, ios::in);
	if (my_file.is_open()) {
		char mychar;
		while (my_file) {
			mychar = my_file.get();
			if((int)mychar <= 127 && (int)mychar >= 0) //extract non-ascii characters
				fullText += mychar;
		}
	}
	my_file.close();
	return fullText;
}
string Utilities::getLastWordBySeparatingWithChar(string s, char del)
{
	vector<string> words;
	stringstream ss(s);
	string word;
	while (!ss.eof()) {
		getline(ss, word, del);
		words.push_back(word);
	}
	return words[words.size() - 1];
}
string Utilities::openFileDialog() { //!!Using Win header!! 

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
std::string Utilities::rgbToHexGenerator(glm::vec3 rgbData) {
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

float Utilities::restrictBetween(float value, float maxVal, float minVal) {
	if (value > maxVal) {
		value = maxVal;
	}
	else if (value < minVal) {
		value = minVal;
	}
	return value;
}

double lastTime = glfwGetTime();
int nbFrames = 0;
void Utilities::printRenderingSpeed() {
	double currentTime = glfwGetTime();
	nbFrames++;
	if (currentTime - lastTime >= 1.0) {
		printf("%f ms/frame\n", 1000.0 / double(nbFrames));
		nbFrames = 0;
		lastTime += 1.0;
	}
}
glm::vec3 Utilities::RGBToHSVGenerator(glm::vec3 rgbData){
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
glm::vec3 Utilities::hexToRGBConverter(std::string hex){ //takes hex : #000000 (# is required) 
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
std::string Utilities::cropString(std::string text,int maxSize){
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

bool Utilities::shortCut(GLFWwindow* window,unsigned int input1,unsigned int input2,unsigned int input3,unsigned int input4){ //Max 4 inputs

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

float Utilities::transitionEffect(bool buttonEnter,float mixVal,float phaseDifference){

	if (buttonEnter && mixVal <= 1.0f) {mixVal += phaseDifference;}
	else if (!buttonEnter && mixVal >= 0.0f) {mixVal -= phaseDifference;}
	return mixVal;
} 

float Utilities::keepTheRatio(double value1,double value2, double desiredRatio){
	double maxV = max(value1,value2);
	double minV = min(value1,value2);

	double ratio = (double)maxV / (double)minV;

	double ratioRatio = ratio/desiredRatio;

	double result = maxV / ratioRatio;

	return result;
}