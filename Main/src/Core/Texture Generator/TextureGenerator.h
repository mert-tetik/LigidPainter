#ifndef LGDTEXTUREGEN_H
#define LGDTEXTUREGEN_H
class TextureGenerator {
public:
	void drawToScreen(GLFWwindow* window, std::string path, bool brushTextureChanged, unsigned int screenPaintingTextureId, float brushSize, unsigned int FBOScreen, bool brushBlurChanged,bool brushSizeChanged, float rotationValue, bool brushRotationChanged, float opacityRangeBarValue, double lastMouseXPos, double lastMouseYPos, double mouseXpos, double mouseYpos);
};

#endif