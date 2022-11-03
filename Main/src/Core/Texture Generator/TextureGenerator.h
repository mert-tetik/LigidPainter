#ifndef RGDTEXTUREGEN_H
#define RGDTEXTUREGEN_H
class TextureGenerator {
public:
	void drawTexture(GLFWwindow* window, std::string path, bool brushTextureChanged, unsigned int screenPaintingTextureId, float brushSizeRangeBarValue);
};

#endif