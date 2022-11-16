#ifndef LGDTEXTURE_H
#define LGDTEXTURE_H
struct GetTextureData {
	unsigned int Id;
	GLubyte* imgData;
};
struct TextureData {
	unsigned int width;
	unsigned int height;
	unsigned char channels;
};
class Texture {
public:
	GetTextureData getTexture(std::string path, unsigned int desiredWidth, unsigned int desiredHeight);
	void downloadTexture(const char* path, const char* name, int format, int width, int height, GLubyte* pixels, int channels);
	GLubyte* getTextureFromProgram(int texture, int width, int height, int channels);
	TextureData getTextureData(const char* path);
	unsigned int createScreenPaintTexture(GLubyte*& screenTexture, GLFWwindow* screen);
	void refreshScreenDrawingTexture();
	GLubyte* updateMaskTexture(unsigned int FBOScreen, unsigned int screenSize_x, unsigned int screenSize_y, float brushRotationRangeBarValue);
};
#endif