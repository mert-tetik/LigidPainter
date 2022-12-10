#ifndef LGDTEXTUREGEN_H
#define LGDTEXTUREGEN_H
class TextureGenerator {
public:
	void drawToScreen(GLFWwindow* window, std::string path, unsigned int screenPaintingTxtrId, float brushSize, unsigned int FBOScreen, float rotationValue, float opacityRangeBarValue, double lastMouseXPos, double lastMouseYPos, double mouseXpos, double mouseYpos, bool mirrorUsed,bool useNegativeForDrawing,bool brushValChanged, int paintingFillNumericModifierVal,Programs programs,int maxScreenWidth,int maxScreenHeight,bool reduceScreenPaintingQuality,float brushBorderRangeBarValue,float brushBlurVal);
};

#endif