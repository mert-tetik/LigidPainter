#ifndef MSHPAPP_H
#define MSHPAPP_H

#include <vector>
struct CommonData {
	unsigned int program = 3;
};

struct CallbckData {
	bool loadModelButtonEnter;
	bool modelFilePathTextBoxEnter;
	glm::vec3 originPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraPos = glm::vec3(0.034906f, 0.000000f, -9.999939f);
	float panelLoc;
};
struct TextureData {
	unsigned int width;
	unsigned int height;
	unsigned char channels;
};
struct ColorData 
{
	glm::vec3 viewportBackColor = glm::vec3(0.23f, 0.23f, 0.23f);
	glm::vec3 panelColor = glm::vec3(0.15f, 0.15f, 0.15f);
	glm::vec3 panelHoldColor = glm::vec3(0.05f, 0.05f, 0.05f);
	glm::vec3 buttonColor = glm::vec3(0.23f, 0.23f, 0.23f);
	glm::vec3 menuBarColor = glm::vec3(0.05f, 0.05f, 0.05f);
	glm::vec3 textBoxColor = glm::vec3(0.05f, 0.05f, 0.05f);
};
struct ProjectionData
{
	glm::mat4 modelMat;
	glm::mat4 viewMat;
	glm::mat4 projMat;
};
struct DataOut
{
	std::string fInfo;
	std::vector <std::string> vVec;
	std::vector <std::string> vnVec;
	std::vector <std::string> vtVec;
};
class Model_Loader {
public:
	DataOut READ_OBJ_FILE(std::string path);
	std::vector<float> OBJ_getVertices(std::string path);
};
class Texture_Generator {
public:
	//Unusable state
	//Will be rewritten
	//void paintTexture(GLFWwindow* window, std::vector<float> &vertices, glm::vec3 cameraPos, glm::vec3 originPos,double mouseXpos, double mouseYpos, GLubyte* pixelsTxtr,GLubyte* pixelsMask,float panelLoc, maskTexture mT, albedoTexture aT);
};
class Texture {
public:
	unsigned int getTexture(std::string path, double imgX, double imgY);
	void drawTexture(const char* path,int width,int height, GLubyte* pixels,int channels);
	GLubyte* getTextureFromProgram(int texture,int width,int height,int channels);
	TextureData getTextureData(const char* path);
};
class Utilities {
public:
	std::string readFile(const char* path);
	std::string openFileDialog();
	std::string getLastWordBySeparatingWithChar(std::string s, char del);
};
class Fadenode {
public:
	void run();
};
class UserInterface {
public:
	void panel(float panelLoc, float movePanel_x);
	void box(float width, float height,float position_x, float position_y,std::string text, glm::vec3 color, float textRatio, bool isTextBox);
	bool isMouseOnButton(GLFWwindow* window, float width, float height, float position_x, float position_y, int mouseXpos, int mouseYpos);
	void setViewportBgColor();
	void renderText(unsigned int program, std::string text, float x, float y, float scale, glm::vec3 color);
	void uploadChars();
	void renderMenubar(GLFWwindow* window);
};
class GlSet {
public:
	void drawArrays(std::vector<float>&vertices,bool isLine);
	void uiDataToShaders(glm::vec3 color);
	void meshDataToShaders();
	void axisPointerDataToShaders();
	void bindFrameBuffer();
	void setVertexAtribPointer();
	void bufferData(std::vector<float>& vertices);
	void activeTexture(unsigned int texture);
	void bindTexture(unsigned int texture);

	void uniform1i(unsigned int program, const char* target, int intValue);
	void uniform1f(unsigned int program, const char* target, float floatValue);
	void uniform3fv(unsigned int program, const char* target, glm::vec3 &vectorValue);
	void uniform3f(unsigned int program, const char* target, float f1, float f2, float f3);
	void uniformMatrix4fv(unsigned int program, const char* target, glm::mat4x4 matrixVal);


	void enable(unsigned int target);
	void cullFace(unsigned int target);
	void disable(unsigned int target);
	void viewport(int width, int height);
	void blendFunc(unsigned int sfactor, unsigned int dfactor);
	void getProgram();
	void render(GLFWwindow* window, std::vector<float>& vertices, float panelLoc, std::string modelLoadFilePath);
	GLFWwindow* getWindow();
	ProjectionData setMatrices(glm::vec3 cameraPos, glm::vec3 originPos);
	glm::vec3 getUnprojection(glm::vec3 vPos, glm::vec3 cameraPos, glm::vec3 originPos);
	void drawLightObject(glm::vec3 lightPos);
};
class Callback {
public:
	CallbckData scroll_callback(GLFWwindow* window, double scroll, double scrollx);
	CallbckData mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void panelCheck(GLFWwindow* window, int mouseXpos, int screenSizeX);
	void buttonCheck(GLFWwindow* window, int mouseXPos, int mouseYPos);
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};
#endif // !MSHPAPP