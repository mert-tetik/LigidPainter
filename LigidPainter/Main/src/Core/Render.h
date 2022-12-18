#ifndef LGDRENDER_H
#define LGDRENDER_H
#include <vector>
#include "../../thirdparty/include/glm/glm.hpp"
#include "../../thirdparty/include/glm/gtc/matrix_transform.hpp"
#include "../../thirdparty/include/glm/gtc/type_ptr.hpp"
#include "../../thirdparty/include/glm/gtx/string_cast.hpp"
#include "LigidPainter.h"
#include "model.h"



struct ExportData
{
	bool exportImage;
	const char* path;
	const char* fileName;
};
struct FramebufferData
{
	unsigned int FBO;
	unsigned int Texture;
	unsigned int RBO;
};
struct ViewUpdateData
{
	glm::vec3 mirroredCameraPos;
	glm::mat4 mirroredView;
	glm::vec3 cameraPos;
	glm::mat4 view;
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

	bool paintingFillNumericModifierPEnter;
	bool paintingFillNumericModifierNEnter;
	
	bool dropperEnter;

	bool exportFileNameTextBoxPressed;

	bool hexValTextboxPressed;


	bool customModelButtonHover;
};
struct RenderOutData{
	glm::vec3 mouseHoverPixel;
	bool maskPanelMaskHover;
	bool maskPanelMaskClicked;
	unsigned int currentBrushMaskTxtr;

	bool texturePanelButtonHover;
	bool texturePanelButtonClicked;
	unsigned int chosenMaterialIndex;

	std::string colorpickerHexVal;
	glm::vec3 colorBoxVal;
};
class Render {
public:
	RenderOutData render(RenderData &renderData, std::vector<float>& vertices, unsigned int FBOScreen, PanelData &panelData, ExportData &exportData,UiData &uidata,float textureDemonstratorButtonPosX,float textureDemonstratorButtonPosY, bool textureDemonstratorButtonPressClicked,float textureDemonstratorWidth, float textureDemonstratorHeight,bool textureDemonstratorBoundariesPressed,Icons &icons,const char* maskTextureFile,int paintingFillNumericModifierVal,float maskPanelSliderValue,std::vector<unsigned int> &maskTextures,std::string &colorpickerHexVal,bool colorpickerHexValTextboxValChanged,bool colorBoxValChanged,bool renderPlane,bool renderSphere,bool reduceScreenPaintingQuality,PBRShaderData &pbrShaderData,SkyBoxShaderData &skyBoxShaderData,float brushBlurVal,ScreenDepthShaderData &screenDepthShaderData,AxisPointerShaderData &axisPointerShaderData,OutShaderData &outShaderData,Model &model,vector<unsigned int> albedoTextures);
	void renderModel(bool backfaceCulling, PBRShaderData &data,Model &model,bool renderDefault,vector<unsigned int> &albedoTextures);
	RenderOutData renderUi(PanelData& panelData, UiData& uidata, RenderData &renderData, unsigned int FBOScreen, float brushBlurRangeBarValue, float brushRotationRangeBarValue, float brushOpacityRangeBarValue, float brushSpacingRangeBarValue,float textureDemonstratorButtonPosX,float textureDemonstratorButtonPosY, bool textureDemonstratorButtonPressClicked,Icons &icons,glm::vec3 colorBoxValue,const char* maskTextureFile,int paintingFillNumericModifierVal,const char* exportFileName,float maskPanelSliderValue,std::vector<unsigned int> &maskTextures,double mouseXpos,double mouseYpos,int screenSizeX,int screenSizeY,std::string &colorpickerHexVal,float brushBorderRangeBarValue,float brushBlurVal,OutShaderData &outShaderData,Model &model,vector<unsigned int> albedoTextures);

    void exportTexture(bool JPG,bool PNG,const char* exportPath,const char* exportFileName,vector<unsigned int> &albedoTextures);
    void renderTexture(std::vector<float>& vertices,unsigned int width, unsigned int height,unsigned int texture,unsigned int channels,Model &model,bool useModel,vector<unsigned int> &albedoTextures);
	void renderTextures(unsigned int FBOScreen, bool exportImage, bool JPG, bool PNG, const char* exportPath,  int screenSizeX,  int screenSizeY,const char* exportFileName,bool reduceScreenPaintingQuality,OutShaderData outShaderData,Model &model,bool renderDefault,vector<unsigned int> &albedoTextures,bool paintOut);
	glm::vec3 getColorBoxValue(unsigned int FBOScreen, float colorBoxPickerValue_x, float colorBoxPickerValue_y,  int screenSizeX,  int screenSizeY);

	void getDepthTexture( unsigned int FBOScreen,  int screenSizeX,  int screenSizeY,ScreenDepthShaderData screenDepthShaderData,Model &model,bool renderDefault,vector<unsigned int> &albedoTextures);
	glm::mat4 setMatrices();
	ViewUpdateData updateViewMatrix(glm::vec3 cameraPos, glm::vec3 originPos,bool mirrorX,bool mirrorY,bool mirrorZ);
	void getUnprojection(glm::vec3 vPos, glm::vec3 cameraPos, glm::vec3 originPos); //Not used
	void drawLightObject(glm::vec3 lightPos);
	void renderSkyBox(SkyBoxShaderData data);
	void sendProgramsToRender(Programs appPrograms);
    void sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight);
};
#endif // !RGDRENDER_H