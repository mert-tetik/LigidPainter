#ifndef LGDRENDER_H
#define LGDRENDER_H
#include <vector>
#include "../../thirdparty/include/glm/glm.hpp"
#include "../../thirdparty/include/glm/gtc/matrix_transform.hpp"
#include "../../thirdparty/include/glm/gtc/type_ptr.hpp"
#include "../../thirdparty/include/glm/gtx/string_cast.hpp"
#include "LigidPainter.h"

struct ExportData
{
	bool exportImage;
	const char* path;
};
struct FramebufferData
{
	unsigned int FBO;
	unsigned int Texture;
	unsigned int RBO;
};
struct UiData {
	bool loadModelButtonEnter;
	bool loadModelButtonPressed;

	bool modelFilePathTextBoxEnter;

	bool autoTriangulateCheckBoxEnter;
	bool autoTriangulateCheckBoxPressed;

	bool backfaceCullingCheckBoxEnter;
	bool backfaceCullingCheckBoxPressed;

	bool addPlaneButtonEnter;
	bool addPlaneButtonPressed;

	bool addSphereButtonEnter;
	bool addSphereButtonPressed;

	bool addImageButtonEnter;
	bool addImageButtonPressed;

	bool addMaskTextureButtonEnter;
	bool addMaskTextureButtonPressed;

	bool brushSizeRangeBarEnter;

	bool brushBlurRangeBarEnter;

	bool brushRotationRangeBarEnter;

	bool brushOpacityRangeBarEnter;

	bool brushSpacingRangeBarEnter;

	bool colorBoxPickerEnter;

	bool colorBoxColorRangeBarEnter;

	bool exportPathTextBoxEnter;

	bool exportExtJPGCheckBoxEnter;
	bool exportExtJPGCheckBoxPressed;

	bool exportExtPNGCheckBoxEnter;
	bool exportExtPNGCheckBoxPressed;

	bool mirrorXCheckBoxEnter;
	bool mirrorXCheckBoxPressed;
	bool mirrorYCheckBoxEnter;
	bool mirrorYCheckBoxPressed;
	bool mirrorZCheckBoxEnter;
	bool mirrorZCheckBoxPressed;

	bool exportDownloadButtonEnter;
	bool exportDownloadButtonPressed;

	bool useNegativeForDrawingCheckboxEnter;
	bool useNegativeForDrawingCheckboxPressed;
};
class Render {
public:
	glm::vec3 render(RenderData renderData, std::vector<float>& vertices, unsigned int FBOScreen, PanelData panelData, ExportData exportData,UiData uidata,float textureDemonstratorButtonPosX,float textureDemonstratorButtonPosY, bool textureDemonstratorButtonPressClicked,float textureDemonstratorWidth, float textureDemonstratorHeight,bool textureDemonstratorBoundariesPressed,Icons icons,const char* maskTextureFile);
	void renderModel(bool backfaceCulling, std::vector<float>& vertices);
	void renderUi(PanelData panelData, UiData uidata, RenderData renderData, unsigned int FBOScreen, float brushBlurRangeBarValue, float brushRotationRangeBarValue, float brushOpacityRangeBarValue, float brushSpacingRangeBarValue,float textureDemonstratorButtonPosX,float textureDemonstratorButtonPosY, bool textureDemonstratorButtonPressClicked,Icons icons,glm::vec3 colorBoxValue,const char* maskTextureFile);

    void exportTexture(bool JPG,bool PNG,const char* exportPath);
    void renderTexture(std::vector<float>& vertices,unsigned int width, unsigned int height,unsigned int texture);
	void renderTextures(unsigned int FBOScreen, std::vector<float>& vertices, bool exportImage, bool JPG, bool PNG, const char* exportPath, unsigned int screenSizeX, unsigned int screenSizeY);
	glm::vec3 getColorBoxValue(unsigned int FBOScreen, float colorBoxPickerValue_x, float colorBoxPickerValue_y, unsigned int screenSizeX, unsigned int screenSizeY);

	void getDepthTexture(std::vector<float>& vertices, unsigned int FBOScreen, unsigned int screenSizeX, unsigned int screenSizeY);
	void setMatrices();
	void updateViewMatrix(glm::vec3 cameraPos, glm::vec3 originPos,bool mirrorX,bool mirrorY,bool mirrorZ);
	void getUnprojection(glm::vec3 vPos, glm::vec3 cameraPos, glm::vec3 originPos); //Not used
	void drawLightObject(glm::vec3 lightPos);
};
#endif // !RGDRENDER_H