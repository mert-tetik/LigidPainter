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

class Render {
public:

	RenderOutData render(RenderData &renderData, unsigned int FBOScreen, PanelData &panelData, ExportData &exportData,Icons &icons,float maskPanelSliderValue,
	std::vector<unsigned int> &maskTextures,bool renderPlane,bool renderSphere,PBRShaderData &pbrShaderData,SkyBoxShaderData &skyBoxShaderData,
	float brushBlurVal,ScreenDepthShaderData &screenDepthShaderData,AxisPointerShaderData &axisPointerShaderData,OutShaderData &outShaderData,Model &model,
	std::vector<aTexture> &albedoTextures,bool paintRender,float materialsPanelSlideValue,std::vector<UIElement> &UIElements, ColorPicker &colorPicker,
	TextureDisplayer &textureDisplayer,Cubemaps cubemaps,ContextMenu &addNodeContextMenu,NodePanel &nodePanel,SndPanel &sndPanel,
	int& selectedAlbedoTextureIndex,TextureSelectionPanel &textureSelectionPanel,std::vector<NodeScene>& nodeScenes,int &selectedNodeScene
	,std::vector<Node> appNodes,glm::mat4 perspectiveProjection,glm::mat4 view,std::vector<MaterialOut> &modelMaterials,bool& newModelAdded,bool firstClick,
	glm::vec3 viewPos,ColoringPanel &coloringPanel,TextureCreatingPanel &txtrCreatingPanel,int& chosenTextureResIndex,int &chosenSkyboxTexture,bool& bakeTheMaterial
	,bool& anyTextureNameActive,std::string &textureText,int viewportBGImage,std::vector<NodeScene> &nodeScenesHistory,BrushMaskTextures &brushMaskTextures,bool maskPanelEnter);


    void sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight);
	void sendProgramsToRender(Programs appPrograms);
	ViewUpdateData updateViewMatrix(glm::vec3 cameraPos, glm::vec3 originPos,bool mirrorX,bool mirrorY,bool mirrorZ);//TODO : This function is not related
	glm::mat4 setMatrices();
    void renderTexture(std::vector<float>& vertices,unsigned int width, unsigned int height,unsigned int texture,unsigned int channels,Model &model,bool useModel,std::vector<MaterialOut> &modelMaterialProgram,glm::mat4 view,std::vector<aTexture> albedoTextures,int chosenTextureIndex);
	glm::vec3 getColorPickerValue(unsigned int FBOScreen,ColorPicker &colorPicker,  int screenSizeX,  int screenSizeY, Programs programs, int maxScreenWidth,int maxScreenHeight,SaturationValShaderData &saturationValShaderData);

private:
	//Render The Scene
	void renderModel(bool backfaceCulling, PBRShaderData &data,Model &model,bool renderDefault,std::vector<MaterialOut> &modelMaterials,Programs programs,int currentMaterialIndex,glm::mat4 view,bool paintingMode,std::vector<aTexture> albedoTextures,int chosenTextureIndex,glm::vec3 viewPos,float skyboxExposureVal,float skyboxRotationVal);
	
	MaterialOut renderTheNodes(NodeScene &material,Model &model,glm::mat4 perspectiveProjection,glm::mat4 view,int maxScreenWidth,int screenSizeX,int maxScreenHeight,int screenSizeY,std::vector<Node>appNodes,int chosenTextureResIndex,bool& bakeTheMaterial,std::vector<aTexture> &albedoTextures,int currentMaterialIndex,std::vector<NodeScene> &nodeScenesHistory);

	
	RenderOutData renderUi(PanelData& panelData, RenderData &renderData, unsigned int FBOScreen, Icons &icons,const char* exportFileName,
	float maskPanelSliderValue,std::vector<unsigned int> &maskTextures,double mouseXpos,double mouseYpos,int screenSizeX,int screenSizeY,float brushBlurVal,
	OutShaderData &outShaderData,Model &model,std::vector<aTexture> &albedoTextures,Programs programs,int &currentMaterialIndex,int maxScreenWidth,
	int maxScreenHeight, SaturationValShaderData &saturationValShaderData,unsigned int &currentBrushMaskTexture,float materialsPanelSlideValue,
	std::vector<UIElement> &UIElements,ColorPicker &colorPicker,TextureDisplayer &textureDisplayer,ContextMenu &addNodeContextMenu,NodePanel &nodePanel,
	SndPanel &sndPanel,int& selectedAlbedoTextureIndex,TextureSelectionPanel &textureSelectionPanel,std::vector<NodeScene>& nodeScenes,
	int &selectedNodeScene,std::vector<Node> appNodes,bool &newModelAdded,std::vector<MaterialOut> &modelMaterials,bool firstClick,ColoringPanel& coloringPanel,
	TextureCreatingPanel &txtrCreatingPanel,int& chosenTextureResIndex,int &chosenSkyboxTexture,bool& bakeTheMaterial,bool& anyTextureNameActive,std::string &textureText
	,std::vector<NodeScene> &nodeScenesHistory,BrushMaskTextures &brushMaskTextures,bool maskPanelEnter);
	
	
	void renderAxisPointer(AxisPointerShaderData axisPointerShaderData,Programs programs);
	void renderModifiedBrushCursor(float distanceX,int screenWidth,int screenHeight,double mouseXpos,double mouseYpos,glm::vec3 color,int maxScreenWidth,int maxScreenHeight,Programs programs);
	void renderSkyBox(SkyBoxShaderData data,Programs programs,float exposureVal,float skyboxRotationVal);

	//Texture Process
	void renderTextures(unsigned int FBOScreen, int screenSizeX,  int screenSizeY, OutShaderData outShaderData,Model &model,bool renderDefault,std::vector<aTexture> &albedoTextures,bool paintOut,bool isRenderTexture,bool paintRender,bool firstPaint,int currentMaterialIndex,Programs programs, int maxScreenWidth , int maxScreenHeight,std::vector<MaterialOut> &modelMaterials,glm::mat4 view,int chosenTextureIndex,int chosenTextureResIndex);
	void getDepthTexture( unsigned int FBOScreen,  int screenSizeX,  int screenSizeY,ScreenDepthShaderData screenDepthShaderData,Model &model,bool renderDefault,std::vector<MaterialOut> &modelMaterials,Programs programs,int currentMaterialIndex, int maxScreenWidth ,int maxScreenHeight,glm::mat4 view,std::vector<aTexture> albedoTextures,int chosenTextureIndex);
    void exportTexture(bool JPG,bool PNG,const char* exportPath,const char* exportFileName,std::vector<aTexture> &albedoTextures,int chosenTextureResIndex);

};
#endif // !RGDRENDER_H