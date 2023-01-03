#ifndef LGDRENDER_H
#define LGDRENDER_H
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "Core/LigidPainter.h"
#include "Core/Model/model.h"
#include "Core/UI/UserInterface.h"


struct UndoActions
{
	GLubyte* undoTextures; 
	unsigned int activeMaterial; 
};

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

struct RenderOutData{
	glm::vec3 mouseHoverPixel;
	bool maskPanelMaskHover;
	bool maskPanelMaskClicked;
	unsigned int currentBrushMaskTxtr;

	bool texturePanelButtonHover;
	bool texturePanelButtonClicked;
	unsigned int chosenMaterialIndex;
};
class Render {
public:
	RenderOutData render(RenderData &renderData, unsigned int FBOScreen, PanelData &panelData, ExportData &exportData,Icons &icons,float maskPanelSliderValue,std::vector<unsigned int> &maskTextures,bool renderPlane,bool renderSphere,PBRShaderData &pbrShaderData,SkyBoxShaderData &skyBoxShaderData,float brushBlurVal,ScreenDepthShaderData &screenDepthShaderData,AxisPointerShaderData &axisPointerShaderData,OutShaderData &outShaderData,Model &model,std::vector<unsigned int> &albedoTextures,bool paintRender,float materialsPanelSlideValue,std::vector<UIElement> &UIElements, ColorPicker &colorPicker,TextureDisplayer &textureDisplayer);
    void sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight);
	void sendProgramsToRender(Programs appPrograms);
	ViewUpdateData updateViewMatrix(glm::vec3 cameraPos, glm::vec3 originPos,bool mirrorX,bool mirrorY,bool mirrorZ);//TODO : This function is not related
	glm::mat4 setMatrices();
    void renderTexture(std::vector<float>& vertices,unsigned int width, unsigned int height,unsigned int texture,unsigned int channels,Model &model,bool useModel,std::vector<unsigned int> &albedoTextures);
private:
	//Render The Scene
	void renderModel(bool backfaceCulling, PBRShaderData &data,Model &model,bool renderDefault,std::vector<unsigned int> &albedoTextures,Programs programs,int currentMaterialIndex);
	RenderOutData renderUi(PanelData& panelData, RenderData &renderData, unsigned int FBOScreen, Icons &icons,const char* exportFileName,float maskPanelSliderValue,std::vector<unsigned int> &maskTextures,double mouseXpos,double mouseYpos,int screenSizeX,int screenSizeY,float brushBlurVal,OutShaderData &outShaderData,Model &model,std::vector<unsigned int> &albedoTextures,Programs programs,int &currentMaterialIndex,int maxScreenWidth,int maxScreenHeight, SaturationValShaderData &saturationValShaderData,unsigned int &currentBrushMaskTexture,float materialsPanelSlideValue,std::vector<UIElement> &UIElements,ColorPicker &colorPicker,TextureDisplayer &textureDisplayer);
	void renderAxisPointer(AxisPointerShaderData axisPointerShaderData,Programs programs);
	void renderModifiedBrushCursor(float distanceX,int screenWidth,int screenHeight,double mouseXpos,double mouseYpos,glm::vec3 color,int maxScreenWidth,int maxScreenHeight,Programs programs);
	void renderSkyBox(SkyBoxShaderData data,Programs programs);

	//Texture Process
	void renderTextures(unsigned int FBOScreen, bool exportImage, bool JPG, bool PNG, const char* exportPath, int screenSizeX,  int screenSizeY,const char* exportFileName, OutShaderData outShaderData,Model &model,bool renderDefault,std::vector<unsigned int> &albedoTextures,bool paintOut,bool isRenderTexture,bool paintRender,bool firstPaint,int currentMaterialIndex,std::vector<UndoActions> &undoList,Programs programs, int maxScreenWidth , int maxScreenHeight);
	glm::vec3 getColorPickerValue(unsigned int FBOScreen,ColorPicker &colorPicker,  int screenSizeX,  int screenSizeY, Programs programs, int maxScreenWidth,int maxScreenHeight,SaturationValShaderData &saturationValShaderData);
	void getDepthTexture( unsigned int FBOScreen,  int screenSizeX,  int screenSizeY,ScreenDepthShaderData screenDepthShaderData,Model &model,bool renderDefault,std::vector<unsigned int> &albedoTextures,Programs programs,int currentMaterialIndex, int maxScreenWidth ,int maxScreenHeight);
    void exportTexture(bool JPG,bool PNG,const char* exportPath,const char* exportFileName,std::vector<unsigned int> &albedoTextures);


	//TODO : Delete these
	void getUnprojection(glm::vec3 vPos, glm::vec3 cameraPos, glm::vec3 originPos); //Not used
	void drawLightObject(glm::vec3 lightPos);//Not used
};
#endif // !RGDRENDER_H