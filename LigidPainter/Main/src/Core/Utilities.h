#ifndef LGDUTIL_H
#define LGDUTIL_H

class Utilities {
public:
	std::string readFile(const char* path);
	std::string openFileDialog();
	std::string getLastWordBySeparatingWithChar(std::string s, char del);
	std::string rgbToHexGenerator(glm::vec3 rgbData);
	glm::vec3 RGBToHSVGenerator(glm::vec3 rgbData);
	float restrictBetween(float value, float maxVal, float minVal);
	void printRenderingSpeed();
	
	glm::vec3 hexToRGBConverter(std::string hex);
	std::string cropString(std::string text,int maxSize);
	bool shortCut(GLFWwindow* window,unsigned int input1,unsigned int input2,unsigned int input3,unsigned int input4);
	float transitionEffect(bool buttonEnter,float mixVal,float phaseDifference);
	float keepTheRatio(double value1,double value2, double desiredRatio);
	void printError();
	std::string removeExtension(std::string s);
	std::string uniqueName(std::string s,std::vector<std::string> albedoTextures);
	bool illegalCharCheck(std::string s);
	bool intersect(glm::vec2 A,glm::vec2 B,glm::vec2 C,glm::vec2 D);
	bool isMatch(std::string o,std::string t);
	unsigned int createQRCode(const char* path,glm::vec3 color);
	std::vector<std::string> seperateFilePaths(std::string paths,char seperator);
	std::vector<glm::vec3> getMirrorVectors(bool x,bool y,bool z);
};
#endif