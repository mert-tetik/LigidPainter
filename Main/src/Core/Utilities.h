#ifndef RGDUTIL_H
#define RGDUTIL_H
class Utilities {
public:
	std::string readFile(const char* path);
	std::string openFileDialog();
	std::string getLastWordBySeparatingWithChar(std::string s, char del);
	std::string rgbToHexGenerator(glm::vec3 rgbData);
};
#endif