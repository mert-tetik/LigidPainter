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
};
#endif