#pragma once
class MSHPApp {
public:
	GLFWwindow* MHSPInit();
	unsigned int SetShadersAndProgram();
	void SetLocations(unsigned int program, glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp, glm::vec3 cameraDirection);
	void drawLigtObject(unsigned int program, glm::vec3 lightPos);
	unsigned int getTexture(const char* path);
	std::string READ_FILE(const char* path);
};