#ifndef LGDTEXTURE_H
#define LGDTEXTURE_H
struct TextureData {
	unsigned int width;
	unsigned int height;
	unsigned char channels;
};
struct ScreenPaintingReturnData {
	unsigned int normalId;
	unsigned int mirroredId;
};
struct InitializedTextures{
	unsigned int depthTexture;
	unsigned int mirroredDepthTexture;
	unsigned int enlargedTexture;
	unsigned int modifiedMaskTexture;
	unsigned int rawMaskTexture;
	unsigned int albedoTxtr;
};
class Texture {
public:
	unsigned int getTexture(std::string path, unsigned int desiredWidth, unsigned int desiredHeight,bool update);
	void downloadTexture(const char* path, const char* name, int format, int width, int height, GLubyte* pixels, int channels);
	GLubyte* getTextureFromProgram(int texture, int width, int height, int channels);
	TextureData getTextureData(const char* path);
	ScreenPaintingReturnData createScreenPaintTexture(GLubyte*& screenTexture, GLFWwindow* screen);
	void refreshScreenDrawingTexture(bool reduceQuality);
	GLubyte* updateMaskTexture(unsigned int FBOScreen, int screenSize_x, int screenSize_y, float brushRotationRangeBarValue,bool renderTiny,float brushBorderRangeBarValue);
	InitializedTextures initTextures(const char* maskTexturePath);
	void sendProgramsToTextures(Programs appPrograms);
    void sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight);
};
#endif