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

	RenderOutData render(RenderData &renderData, unsigned int FBOScreen, PanelData &panelData, ExportData &exportData,Icons &icons,
						bool renderPlane,bool renderSphere,PBRShaderData &pbrShaderData,SkyBoxShaderData &skyBoxShaderData,
						float brushBlurVal,ScreenDepthShaderData &screenDepthShaderData,AxisPointerShaderData &axisPointerShaderData,OutShaderData &outShaderData,Model &model,
						std::vector<aTexture> &albedoTextures,bool paintRender,float materialsPanelSlideValue,std::vector<UIElement> &UIElements, ColorPicker &colorPicker,
						TextureDisplayer &textureDisplayer,Cubemaps &cubemaps,ContextMenu &addNodeContextMenu,NodePanel &nodePanel,SndPanel &sndPanel,
						int& selectedAlbedoTextureIndex,TextureSelectionPanel &textureSelectionPanel,std::vector<NodeScene>& nodeScenes,int &selectedNodeScene
						,std::vector<Node> &appNodes,glm::mat4 perspectiveProjection,glm::mat4 view,std::vector<MaterialOut> &modelMaterials,bool& newModelAdded,bool &firstClick,
						glm::vec3 viewPos,ColoringPanel &coloringPanel,TextureCreatingPanel &txtrCreatingPanel,int& chosenTextureResIndex,int &chosenSkyboxTexture,bool& bakeTheMaterial
						,bool& anyTextureNameActive,std::string &textureText,int viewportBGImage,std::vector<NodeScene> &nodeScenesHistory,BrushTexture &brushMaskTextures,bool maskPanelEnter
						,bool &duplicateNodeCall,Objects &objects,int &chosenNodeResIndex,glm::vec3 &drawColor,std::vector<MirrorParam>&mirrorParams,unsigned int &depthTextureID
						,glm::vec3 cameraPos, glm::vec3 originPos,bool &startScreen,std::string &projectFilePath,aTexture paintOverTexture,Model &spherModel,Audios audios,
						unsigned int materialFBO,int &currentMaterialIndex,bool &textureDraggingState,bool &debugMode,bool &createProject,char* &modelFilePath,std::string &modelName,
						std::string &customModelName,glm::mat4 &modelMatrix,bool &displayProjectFolderManager);

    void sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight);
	void sendProgramsToRender(Programs appPrograms);
	ViewUpdateData updateViewMatrix(glm::vec3 cameraPos, glm::vec3 originPos,bool mirrorX,bool mirrorY,bool mirrorZ);
	glm::mat4 setMatrices();
    void renderTexture(std::vector<float>& vertices,unsigned int width, unsigned int height,unsigned int texture,unsigned int channels,Model &model,
		bool useModel,std::vector<MaterialOut> &modelMaterialProgram,glm::mat4 view,std::vector<aTexture> albedoTextures,int chosenTextureIndex,bool useUV);
	glm::vec3 getColorPickerValue(unsigned int FBOScreen,ColorPicker &colorPicker,  int screenSizeX,  int screenSizeY, Programs programs, int maxScreenWidth,int maxScreenHeight,SaturationValShaderData &saturationValShaderData);
	void renderSkyBox(SkyBoxShaderData data,Programs programs,float exposureVal,float skyboxRotationVal);
	void renderBlurySkybox(Cubemaps &cubemaps,SkyBoxShaderData &skyBoxShaderData,Programs & renderPrograms,std::vector<UIElement> UIElements, float &createProjectPanelBlurVal,float w, float h,bool roundCorners);


private:
	//Render The Scene
	void renderModel(bool backfaceCulling, PBRShaderData &data,Model &model,bool renderDefault,std::vector<MaterialOut> &modelMaterials,Programs programs,int currentMaterialIndex,glm::mat4 view,
					bool paintingMode,std::vector<aTexture> albedoTextures,int chosenTextureIndex,glm::vec3 viewPos,float skyboxExposureVal,float skyboxRotationVal,Objects &objects,std::vector<NodeScene> nodeScenes,
					glm::mat4 &modelMatrix,bool useUV,float lightRotation);
	
	MaterialOut renderTheNodes(NodeScene &material,Model &model,glm::mat4 perspectiveProjection,glm::mat4 view,int maxScreenWidth,int screenSizeX,int maxScreenHeight,int screenSizeY,std::vector<Node>appNodes,int chosenTextureResIndex,bool& bakeTheMaterial,std::vector<aTexture> &albedoTextures,int currentMaterialIndex,std::vector<NodeScene> &nodeScenesHistory,int chosenNodeResIndex,unsigned int materialFBO);

	
	void renderFocusModeUI(Programs programs,RenderData &renderData,std::vector<UIElement> &UIElements,Icons icons,ColoringPanel &coloringPanel,
	SaturationValShaderData saturationValShaderData,double mouseXpos,double mouseYpos,bool firstClick,unsigned int FBOScreen,ColorPicker &colorPicker,
	glm::vec3 screenHoverPixel,glm::vec3 &drawColor,OutShaderData &outShaderData);

	RenderOutData renderUi(PanelData& panelData, RenderData &renderData, unsigned int FBOScreen, Icons &icons,const char* exportFileName,
							double mouseXpos,double mouseYpos,int screenSizeX,int screenSizeY,float brushBlurVal,
							OutShaderData &outShaderData,Model &model,std::vector<aTexture> &albedoTextures,Programs programs,int &currentMaterialIndex,int maxScreenWidth,
							int maxScreenHeight, SaturationValShaderData &saturationValShaderData,unsigned int &currentBrushMaskTexture,float materialsPanelSlideValue,
							std::vector<UIElement> &UIElements,ColorPicker &colorPicker,TextureDisplayer &textureDisplayer,ContextMenu &addNodeContextMenu,NodePanel &nodePanel,
							SndPanel &sndPanel,int& selectedAlbedoTextureIndex,TextureSelectionPanel &textureSelectionPanel,std::vector<NodeScene>& nodeScenes,
							int &selectedNodeScene,std::vector<Node> appNodes,bool &newModelAdded,std::vector<MaterialOut> &modelMaterials,bool &firstClick,ColoringPanel& coloringPanel,
							TextureCreatingPanel &txtrCreatingPanel,int& chosenTextureResIndex,int &chosenSkyboxTexture,bool& bakeTheMaterial,bool& anyTextureNameActive,std::string &textureText
							,std::vector<NodeScene> &nodeScenesHistory,BrushTexture &brushMaskTextures,bool maskPanelEnter,bool &duplicateNodeCall,Cubemaps &cubemaps,Objects &objects
							,glm::vec3 screenHoverPixel,int &chosenNodeResIndex,Audios audios,bool &textureDraggingState,float &lightRotVal,std::string projectPath);
	
	
	void renderAxisPointer(AxisPointerShaderData axisPointerShaderData,Programs programs);
	void renderModifiedBrushCursor(float distanceX,int screenWidth,int screenHeight,double mouseXpos,double mouseYpos,glm::vec3 color,int maxScreenWidth,int maxScreenHeight,Programs programs);
	void renderBackgroundImage(unsigned int renderTheTextureProgram, unsigned int viewportBGImage);


	//Texture Process
	void renderTextures(unsigned int FBOScreen, int screenSizeX,  int screenSizeY, OutShaderData outShaderData,Model &model,bool renderDefault,std::vector<aTexture> &albedoTextures,bool paintOut,bool isRenderTexture,bool paintRender,
						int currentMaterialIndex,Programs programs, int maxScreenWidth , int maxScreenHeight,std::vector<MaterialOut> &modelMaterials,glm::mat4 view,int chosenTextureIndex,int chosenTextureResIndex,bool useUV);
	void getDepthTexture( unsigned int FBOScreen,  int screenSizeX,  int screenSizeY,ScreenDepthShaderData screenDepthShaderData,Model &model,bool renderDefault,std::vector<MaterialOut> &modelMaterials,Programs programs,int currentMaterialIndex, 
							int maxScreenWidth ,int maxScreenHeight,glm::mat4 view,std::vector<aTexture> albedoTextures,int chosenTextureIndex,std::vector<MirrorParam> &mirrorParams,unsigned int &depthTextureID,glm::vec3 cameraPos, glm::vec3 originPos,float xMirrorPos,
							float yMirrorPos,float zMirrorPos,bool useUV);
    void exportTexture(bool JPG,bool PNG,const char* exportPath,const char* exportFileName,std::vector<aTexture> &albedoTextures,int chosenTextureResIndex);

	void projectFolderManagerPanel(std::vector<UIElement> &UIElements,Programs renderPrograms,Cubemaps cubemaps,SkyBoxShaderData skyBoxShaderData,
                                        float &createProjectPanelBlurVal,std::string &projectPath,double screenGapX,GLFWwindow* window,Icons icons,double mouseXpos,double mouseYpos
										,bool firstClick,bool &displayProjectFolderManager);
};
#endif // !RGDRENDER_H