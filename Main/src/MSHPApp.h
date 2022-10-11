#pragma once
#include <vector>
struct projectionData
{
	glm::mat4 modelMat;
	glm::mat4 viewMat;
	glm::mat4 projMat;
};
struct dataOut
{
	std::string fInfo;
	std::vector <std::string> vVec;
	std::vector <std::string> vnVec;
	std::vector <std::string> vtVec;
};
class MSHP_Model_Loader {
public:
	dataOut READ_OBJ_FILE();
	std::vector<float> OBJ_getVertices();
};
class MSHP_Texture_Generator {
public:
	void paintTexture(GLFWwindow* window, std::vector<float> &vertices, unsigned int shaderProgram, glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp, glm::vec3 originPos,double mouseXpos, double mouseYpos, GLubyte* pixelsTxtr,GLubyte* pixelsMask);
};
class MSHPApp {
public:
	GLFWwindow* init();
	unsigned int getProgram();
	projectionData transformObject(unsigned int program, glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp, glm::vec3 originPos);
	void drawLightObject(unsigned int program, glm::vec3 lightPos);
	unsigned int getTexture(const char* path, double imgX, double imgY);
	glm::vec3 getUnprojection(glm::vec3 vPos,unsigned int program, glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp, glm::vec3 originPos);
	std::string readFile(const char* path);
	void run();
	void renderModel(std::vector<float>& vertices, unsigned int shaderProgram);
};