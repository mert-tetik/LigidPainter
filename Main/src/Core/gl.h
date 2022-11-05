#ifndef RGDGL_H
#define RGDGL_H
class GlSet {
public:
	void drawArrays(std::vector<float>& vertices, bool isLine);
	void uiDataToShaders(glm::vec3 color);
	void meshDataToShaders();
	void axisPointerDataToShaders();
	void bindFramebuffer(unsigned int FBO);
	void genFramebuffers(unsigned int& FBO);
	void deleteFramebuffers(unsigned int& FBO);
	void setVertexAtribPointer();
	void bufferData(std::vector<float>& vertices);
	void activeTexture(unsigned int texture);
	void bindTexture(unsigned int texture);
	void genTextures(unsigned int& texture);
	void texImage(GLubyte* data, int width, int height, unsigned int channels);
	void generateMipmap();
	void genRenderbuffers(unsigned int& RBO);
	void bindRenderBuffer(unsigned int RBO);


	void uniform1i(unsigned int program, const char* target, int intValue);
	void uniform1f(unsigned int program, const char* target, float floatValue);
	void uniform3fv(unsigned int program, const char* target, glm::vec3& vectorValue);
	void uniform3f(unsigned int program, const char* target, float f1, float f2, float f3);
	void uniformMatrix4fv(unsigned int program, const char* target, glm::mat4x4 matrixVal);


	void enable(unsigned int target);
	void cullFace(unsigned int target);
	void disable(unsigned int target);
	void viewport(int width, int height);
	void blendFunc(unsigned int sfactor, unsigned int dfactor);
	void getProgram();
	void render(RenderData renderData, std::vector<float>& vertices, unsigned int FBO, PanelData panelData, bool cameraPosChanged, std::vector<float>& axisPointer, float colorBoxPickerValue_x,float colorBoxPickerValue_y, unsigned int paintingSqFBO, float colorBoxColorRangeBarValue);
	GLFWwindow* getWindow();
	void setMatrices();
	void updateViewMatrix(glm::vec3 cameraPos, glm::vec3 originPos);
	void getUnprojection(glm::vec3 vPos, glm::vec3 cameraPos, glm::vec3 originPos); //Not used
	void drawLightObject(glm::vec3 lightPos);
	unsigned int createScreenFrameBufferObject();
};
#endif // !RGDGL_H