#ifndef LGDTEXTUREGEN_H
#define LGDTEXTUREGEN_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Core/Model/model.h"

struct GLFWwindow;
struct Programs;
struct OutShaderData;
class Model;
class vector;

class TextureGenerator {
public:
	void drawToScreen(GLFWwindow*& window, unsigned int screenPaintingTxtrId, float brushSize, unsigned int FBOScreen, float rotationValue, float opacityRangeBarValue, double lastMouseXPos, double lastMouseYPos, double mouseXpos, double mouseYpos, bool mirrorUsed,bool useNegativeForDrawing,bool brushValChanged,Programs& programs,int maxScreenWidth,int maxScreenHeight,float brushBorderRangeBarValue,float brushBlurVal,unsigned int FBO,OutShaderData &outShaderData,Model &model,std::vector<MaterialOut> &modelMaterials,bool fillBetween,glm::mat4 view);
	std::vector<float> gaussianFunc(int radius,float mean);
};

#endif