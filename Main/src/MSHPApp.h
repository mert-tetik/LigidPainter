#pragma once
#include <vector>
struct commonData {
	unsigned int program = 3;
};
struct colorData 
{
	glm::vec3 viewportBackColor = glm::vec3(0.23f, 0.23f, 0.23f);
	glm::vec3 panelColor = glm::vec3(0.15f, 0.15f, 0.15f);
	glm::vec3 buttonColor = glm::vec3(0.23f, 0.23f, 0.23f);
};
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
	void paintTexture(GLFWwindow* window, std::vector<float> &vertices, glm::vec3 cameraPos, glm::vec3 originPos,double mouseXpos, double mouseYpos, GLubyte* pixelsTxtr,GLubyte* pixelsMask,float panelLoc);
};
class texture {
public:
	unsigned int getTexture(const char* path, double imgX, double imgY);
	void drawTexture(const char* path,int width,int height, GLubyte* pixels,int channels);
	GLubyte* getTextureFromProgram(int texture,int width,int height,int channels);
};
class MSHPApp {
public:
	GLFWwindow* init();
	void getProgram();
	projectionData transformObject(glm::vec3 cameraPos, glm::vec3 originPos);
	void drawLightObject(glm::vec3 lightPos);
	glm::vec3 getUnprojection(glm::vec3 vPos, glm::vec3 cameraPos, glm::vec3 originPos);
	std::string readFile(const char* path);
	void run();
	void renderModel(std::vector<float>& vertices,float panelLoc);
};
class MSHP_userInterface {
public:
	void panel(float panelLoc);
	void button(float width, float position_x, float position_y);
	int* buttonGetScreenCoor(float width, float position_x, float position_y);
	void setViewportBgColor();
};
class glSet {
public:
	void drawArrays(std::vector<float>&vertices,bool isLine);
	void uiDataToShaders(glm::vec3 color);
	void meshDataToShaders();
	void axisPointerDataToShaders();
	void bindFrameBuffer();
};
class callback {
public:
	void scroll_callback(GLFWwindow* window, double scroll, double scrollx);
};