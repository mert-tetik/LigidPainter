#ifndef LGDTEXTURE_H
#define LGDTEXTURE_H

#include <Core/UI/UserInterface.h>

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
	GLubyte* getTextureFromProgram(unsigned int texture, int width, int height, int channels);
	TextureData getTextureData(const char* path);
	ScreenPaintingReturnData createScreenPaintTexture(GLubyte*& screenTexture, GLFWwindow* screen);
	void refreshScreenDrawingTexture();
	std::vector<GLubyte> updateMaskTexture(unsigned int FBOScreen, int screenSize_x, int screenSize_y, float brushRotationRangeBarValue,bool renderTiny,float brushBorderRangeBarValue,float brushBlurVal,OutShaderData outShaderData,Programs programs,int maxScreenWidth, int maxScreenHeight);
	void sendProgramsToTextures(Programs appPrograms);
    void sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight);
	void refreshScreenTxtr();
	void deleteOpenglTexture(aTexture texture);
	GLubyte* resizeTexture(GLubyte* data,int w,int h,int ow,int oh);
	
};
#endif