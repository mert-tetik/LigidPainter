#ifndef LGDGL_H
#define LGDGL_H
struct ExportData
{
	bool exportImage;
	const char* path;
};
struct FramebufferData
{
	unsigned int FBO;
	unsigned int Texture;
	unsigned int RBO;
};
struct UiData {
	bool loadModelButtonEnter;
	bool loadModelButtonPressed;

	bool modelFilePathTextBoxEnter;

	bool autoTriangulateCheckBoxEnter;
	bool autoTriangulateCheckBoxPressed;

	bool backfaceCullingCheckBoxEnter;
	bool backfaceCullingCheckBoxPressed;

	bool addPlaneButtonEnter;
	bool addPlaneButtonPressed;

	bool addSphereButtonEnter;
	bool addSphereButtonPressed;

	bool addImageButtonEnter;
	bool addImageButtonPressed;

	bool addMaskTextureButtonEnter;
	bool addMaskTextureButtonPressed;

	bool brushSizeRangeBarEnter;

	bool brushBlurRangeBarEnter;

	bool brushRotationRangeBarEnter;

	bool colorBoxPickerEnter;

	bool colorBoxColorRangeBarEnter;

	bool exportPathTextBoxEnter;

	bool exportExtJPGCheckBoxEnter;
	bool exportExtJPGCheckBoxPressed;

	bool exportExtPNGCheckBoxEnter;
	bool exportExtPNGCheckBoxPressed;

	bool exportDownloadButtonEnter;
	bool exportDownloadButtonPressed;
};
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
	void render(RenderData renderData, std::vector<float>& vertices, unsigned int FBOScreen, PanelData panelData, bool cameraPosChanged, std::vector<float>& axisPointer, ExportData exportData, float brushBlurRangeBarValue,UiData uidata,bool albedoTextureChanged , float brushRotationRangeBarValue);
	void getDepthTexture(std::vector<float>& vertices);
	GLFWwindow* getWindow();
	void setMatrices();
	void updateViewMatrix(glm::vec3 cameraPos, glm::vec3 originPos);
	void getUnprojection(glm::vec3 vPos, glm::vec3 cameraPos, glm::vec3 originPos); //Not used
	void drawLightObject(glm::vec3 lightPos);
	unsigned int createScreenFrameBufferObject();
};
#endif // !RGDGL_H