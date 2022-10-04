#pragma once
class MSHPApp {
public:
	GLFWwindow* init();
	unsigned int getProgram();
	void transformObject(unsigned int program, glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp, glm::vec3 originPos);
	void drawLightObject(unsigned int program, glm::vec3 lightPos);
	unsigned int getTexture(const char* path);
	std::string readFile(const char* path);
	void run();
};