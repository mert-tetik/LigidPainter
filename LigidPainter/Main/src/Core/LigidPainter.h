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
	unsigned int Painting;
	unsigned int Logo;
	unsigned int AddTexture;
	unsigned int Material;
};

struct RenderData { //GlSet::render
	GLFWwindow* window;
	float panelLoc; 
	std::string modelLoadFilePath; 
	bool backfaceCulling;
	float brushSizeValue;
	float brushSizeIndicator;
	float colorBoxPickerValue_x; 
	float colorBoxPickerValue_y;
	float colorBoxColorRangeBarValue;
	bool cameraPosChanged;
	const char* exportFolder;
	bool doPainting; 
	unsigned int depthTexture;
	bool paintingMode;
	float brushBlurValue;
	float brushRotationValue;
	float brushOpacityValue;
	float brushSpacingValue;
	float brushBorderValue;

	float textureDemonstratorButtonPosX;
	float textureDemonstratorButtonPosY;
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
	void textureDemonstratorButton(double xOffset,double yOffset,int width,int height);
	void textureDemonstratorBoundaries(double xOffset,double yOffset,int width,int height);
	void paintingDropper();
	void exportFileNameTextBox();
	void colorBox();
	void maskPanelSlider(double yOffset,int screenSizeY);
	void hexValTextbox();
	void loadCustomModel();
	void hueBar();
	void mainPanelBoundaries(float xOffset,int screenSizeX);
};
struct PanelData {
	bool movePanel;
	bool modelPanelActive;
	bool texturePanelActive;
	bool paintingPanelActive;
	bool exportPanelActive;
};
#endif // !MSHPAPP