#ifndef LSHPAPP_H
#define LSHPAPP_H
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>


#include "glad/glad.h"
#include "GLFW/glfw3.h"



struct Programs
{
	unsigned int uiProgram;
	unsigned int skyboxProgram;
	unsigned int blurProgram;
	unsigned int iconsProgram;
	unsigned int skyboxblurProgram;
	unsigned int PBRProgram;
	unsigned int saturationValBoxProgram;
	unsigned int screenDepthProgram;
	unsigned int hueProgram;
	unsigned int axisPointerProgram;
	unsigned int outProgram;
	unsigned int twoDPaintingProgram; 
	unsigned int renderTheTextureProgram; 
	unsigned int prefilterMapProgram; 
	unsigned int brdfProgram; 
	unsigned int blurySkyboxProgram; 
	unsigned int normalGenProgram; 
};
struct Cubemaps{
	unsigned int cubemap;
	unsigned int prefiltered;
	unsigned int cubemap2;
	unsigned int prefiltered2;
	unsigned int cubemap3;
	unsigned int prefiltered3;
	unsigned int cubemap4;
	unsigned int prefiltered4;
	unsigned int cubemap5;
	unsigned int prefiltered5;

	unsigned int blurycubemap;
};
struct LigidCursors{
	GLFWcursor* pointerCursor;
	GLFWcursor* defaultCursor;
	GLFWcursor* dropperCursor;
};
struct PBRShaderData{
	glm::mat4 view;
	glm::mat4 mirroredView;
	glm::mat4 projection;
	int screenMaskTexture;
	int mirroredScreenMaskTexture;
	int useMirror;
	glm::vec3 drawColor;
	int depthTexture;
	int mirroredDepthTexture;
	glm::vec3 viewPos;
	glm::vec3 mirroredViewPos;
	int bluryskybox; 
	int materialDiffuse;
};

struct SkyBoxShaderData{
	glm::mat4 view;
	glm::mat4 projection;
	int skybox;
};
struct BlurShaderData{
	int inputTexture;
	int isRenderVerticalBlur;
	float brushBlurVal;
	glm::mat4 renderTextureProjection;
};
struct SaturationValShaderData{
	glm::mat4 renderTextureProjection;
	glm::vec3 boxColor = glm::vec3(0.0,1.0,0.0);
};
struct ScreenDepthShaderData{
	glm::mat4 view;
	glm::mat4 mirroredView;
	glm::mat4 projection;
	int renderMirrored;
};
struct HueShaderData{
	glm::mat4 renderTextureProjection;
	int useTexCoords;
};

struct AxisPointerShaderData{
	glm::mat4 projection;
	glm::mat4 view;
};
struct OutShaderData{
	glm::mat4 view;
	glm::mat4 mirroredView;
	glm::mat4 projection;
	int isTwoDimensional;
	glm::mat4 renderTextureProjection;
	glm::mat4 renderTrans;	



	int screenMaskTexture;
	int mirroredScreenMaskTexture;
	int useMirror;
	int isRenderScreenMaskMode;
	int verticalMirror;
	glm::vec3 drawColor;
	int depthTexture; //Screen rendered with depth color
	int mirroredDepthTexture; //Screen rendered with depth color
	int renderDepth;
	int renderMaskBrush;
	int modifiedMaskTexture;
	glm::vec3 viewPos;
	glm::vec3 mirroredViewPos;
	int whiteRendering;
	int uvMask;
	int interpretWithUvMask;
	int paintedTxtrMask;
};


struct Icons{
	unsigned int dropperIcon;
	unsigned int TDModel;
	unsigned int BackfaceCulling;
	unsigned int ColorPicker;
	unsigned int Export;
	unsigned int Folder;
	unsigned int ImportMask;
	unsigned int ImportModel;
	unsigned int ImportTexture;
	unsigned int JpgFile;
	unsigned int MaskGausBlur;
	unsigned int MaskOpacity;
	unsigned int MaskRotation;
	unsigned int MaskScale;
	unsigned int MaskSpacing;
	unsigned int Mirror;
	unsigned int Panel;
	unsigned int PngFile;
	unsigned int Sphere;
	unsigned int Triangulate;
	unsigned int ArrowRight;
	unsigned int ArrowLeft;
	unsigned int ArrowDown;
	unsigned int Painting;
	unsigned int Logo;
	unsigned int AddTexture;
	unsigned int Material;
	unsigned int Circle;
	unsigned int TL;
	unsigned int TR;
	unsigned int BL;
	unsigned int BR;
	unsigned int PanelButtonL;
	unsigned int PanelButtonR;
	unsigned int Plus;
	unsigned int Minus;
	unsigned int Texture;
	unsigned int X;
	unsigned int O;
	unsigned int CircularX;
};

struct RenderData { //GlSet::render
	GLFWwindow* window;
	float panelLoc; 
	std::string modelLoadFilePath; 
	bool backfaceCulling;
	float brushSizeIndicator;
	bool cameraPosChanged;
	const char* exportFolder;
	bool doPainting; 
	unsigned int depthTexture;
	bool paintingMode;
};
class Sphere {
public:
	std::vector<float> getSphere();
};
class LigidPainter {
public:
	bool run();


	void loadModelButton();
	void modelFilePathTextBox();
	void autoTriangulateCheckBox();
	void backfaceCullingCheckBox();
	void useNegativeForDrawingCheckbox();
	void addPlaneButton();
	void addSphereButton();
	void addImageButton();
	void modelPanelButton();
	void texturePanelButton();
	void paintingPanelButton();
	void exportPanelButton();
	void settingsPanelButton();
	void addMaskTextureButton();
	void exportDownloadButtonEnter();
	void exportPathTextBox();
	void exportExtJPGCheckBox();
	void exportExtPNGCheckBox();
	void mirrorXCheckBox();
	void mirrorYCheckBox();
	void mirrorZCheckBox();
	void colorBoxColorRangeBar(double yOffset, int height);
	void colorBoxPickerButton(double xOffset, double yOffset, int width, int height);
	void brushSizeRangeBar(double xOffset, int width);
	void brushBlurRangeBar(double xOffset, int width, int height,bool renderTiny);
	void brushRotationRangeBar(double xOffset, int width, int height);
	void brushOpacityRangeBar(double xOffset, int width, int height);
	void brushSpacingRangeBar(double xOffset, int width, int height);
	void brushBordersRangeBar(double xOffset, int width, int height);
	void textureDisplayerButton(double xOffset,double yOffset,int width,int height);
	void textureDisplayerBoundaries(double xOffset,double yOffset,int width,int height);
	void skyboxExposureRangeBar(double xOffset, int width, int height);
	void skyBoxRotationRangeBar(double xOffset, int width, int height);

	void paintingDropper();
	void exportFileNameTextBox();
	void colorBox();
	void maskPanelSlider(double yOffset,int screenSizeY);
	void hexValTextbox();
	void loadCustomModel();
	void hueBar();
	void mainPanelBoundaries(float xOffset,int screenSizeX);
	void nodePanelBoundaries(float yOffset,float screenHeight);
	void sndPanelBoundaries(float xOffset, int screenSizeX);
	void sndPanelPlusIcon();
	void sndPanelDownIcon();
	void sndPanelMinusIcon();
	void updateColorPicker(glm::vec3 RGBval,bool changeHue,bool changeSatV,float &hueValue,float &saturationValuePosX, float &saturationValuePosY,bool isMainColorPicker);



};
struct PanelData {
	bool movePanel;
	bool modelPanelActive;
	bool texturePanelActive;
	bool paintingPanelActive;
	bool exportPanelActive;
	bool settingsPanelActive;
};
#endif // !MSHPAPP