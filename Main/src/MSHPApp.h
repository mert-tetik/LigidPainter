#ifndef MSHPAPP_H
#define MSHPAPP_H

#include <vector>
struct commonData {
	unsigned int program = 3;
};

struct callbckData {
	bool buttonEnter;
	glm::vec3 originPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraPos = glm::vec3(0.034906f, 0.000000f, -9.999939f);
	float panelLoc;
};

//Textures
struct textureData {
	unsigned int width;
	unsigned int height;
	unsigned char channels;
};
struct maskTexture {
	unsigned int width;
	unsigned int height;
	unsigned char channels;
};
struct albedoTexture {
	unsigned int width;
	unsigned int height;
	unsigned char channels;
};
//Texture

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
	void paintTexture(GLFWwindow* window, std::vector<float> &vertices, glm::vec3 cameraPos, glm::vec3 originPos,double mouseXpos, double mouseYpos, GLubyte* pixelsTxtr,GLubyte* pixelsMask,float panelLoc, maskTexture mT, albedoTexture aT);
};
class texture {
public:
	unsigned int getTexture(std::string path, double imgX, double imgY);
	void drawTexture(const char* path,int width,int height, GLubyte* pixels,int channels);
	GLubyte* getTextureFromProgram(int texture,int width,int height,int channels);
	textureData getTextureData(const char* path);
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
	callbckData scroll_callback(GLFWwindow* window, double scroll, double scrollx);
	callbckData mouse_callback(GLFWwindow* window, double xpos, double ypos);
};
#endif // !MSHPAPP