#ifndef LGDGL_H
#define LGDGL_H

struct WindowData{
	int windowMaxWidth;
	int windowMaxHeight;
	GLFWwindow* window;
};

class GlSet {
public:
	void drawArrays(std::vector<float>& vertices, bool isLine);
	void uiDataToShaders(glm::vec3 color);
	void bindFramebuffer(unsigned int FBO);
	void genFramebuffers(unsigned int& FBO);
	void deleteFramebuffers(unsigned int& FBO);
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
	void uniform4fv(unsigned int program, const char* target, glm::vec4 &vectorValue);
	void uniformMatrix4fv(unsigned int program, const char* target, glm::mat4x4 matrixVal);


	void enable(unsigned int target);
	void cullFace(unsigned int target);
	void disable(unsigned int target);
	void viewport(int width, int height);
	void blendFunc(unsigned int sfactor, unsigned int dfactor);

	WindowData getWindow();
	unsigned int createProgram(std::string path);


	void usePBRShader(unsigned int program,PBRShaderData data);
	void useSkyBoxShader(unsigned int program,SkyBoxShaderData data);
	void useBlurShader(unsigned int program, BlurShaderData data);
	void useSaturationValBoxShader(unsigned int program, SaturationValShaderData data);
	void useScreenDepthShader(unsigned int program, ScreenDepthShaderData data);
	void useHueShader(unsigned int program, HueShaderData data);
	void useAxisPointerShader(unsigned int program, AxisPointerShaderData data);
	void useOutShader(unsigned int program, OutShaderData data);

};
#endif // !RGDGL_H