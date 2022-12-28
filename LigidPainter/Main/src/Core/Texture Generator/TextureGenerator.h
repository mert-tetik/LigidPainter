#ifndef LGDTEXTUREGEN_H
#define LGDTEXTUREGEN_H

struct GLFWwindow;
struct Programs;
struct OutShaderData;
class Model;
class vector;

class TextureGenerator {
public:
	void drawToScreen(GLFWwindow*& window, std::string &path, unsigned int screenPaintingTxtrId, float brushSize, unsigned int FBOScreen, float rotationValue, float opacityRangeBarValue, double lastMouseXPos, double lastMouseYPos, double mouseXpos, double mouseYpos, bool mirrorUsed,bool useNegativeForDrawing,bool brushValChanged,Programs& programs,int maxScreenWidth,int maxScreenHeight,float brushBorderRangeBarValue,float brushBlurVal,unsigned int FBO,OutShaderData &outShaderData,Model &model,std::vector<unsigned int> &albedoTextures,bool fillBetween);
	std::vector<double> gaussianFunc(int radius,float mean);
};

#endif