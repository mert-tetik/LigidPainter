#ifndef RGDTEXTURE_H
#define RGDTEXTURE_H
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
	GetTextureData getTexture(std::string path, double imgX, double imgY, bool resize, float brushSizeRangeBarValue);
	void downloadTexture(const char* path, const char* name, int format, int width, int height, GLubyte* pixels, int channels);
	GLubyte* getTextureFromProgram(int texture, int width, int height, int channels);
	TextureData getTextureData(const char* path);
	unsigned int createScreenPaintTexture(GLubyte*& screenTexture, GLFWwindow* screen);
	void refreshScreenDrawingTexture();
};
#endif