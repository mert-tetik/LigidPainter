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
class Texture {
public:
	unsigned int getTexture(std::string path, unsigned int desiredWidth, unsigned int desiredHeight,bool update);
	void downloadTexture(const char* path, const char* name, int format, int width, int height, GLubyte* pixels, int channels);
	GLubyte* getTextureFromProgram(int texture, int width, int height, int channels);
	TextureData getTextureData(const char* path);
	ScreenPaintingReturnData createScreenPaintTexture(GLubyte*& screenTexture, GLFWwindow* screen);
	void refreshScreenDrawingTexture();
	GLubyte* updateMaskTexture(unsigned int FBOScreen, unsigned int screenSize_x, unsigned int screenSize_y, float brushRotationRangeBarValue);
};
#endif