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
};
#endif