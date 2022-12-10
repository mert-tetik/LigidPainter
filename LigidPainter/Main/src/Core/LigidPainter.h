#ifndef LSHPAPP_H
#define LSHPAPP_H
#include <vector>

struct Programs
{
	unsigned int program;
	unsigned int skyboxProgram;
	unsigned int blurProgram;
	unsigned int iconsProgram;
	unsigned int skyboxblurProgram;
	unsigned int PBRProgram;
	unsigned int saturationValBoxProgram;
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
};

struct RenderData { //GlSet::render
	GLFWwindow* window;
	float panelLoc; 
	std::string modelLoadFilePath; 
	bool backfaceCulling;
	float brushSizeRangeBarValue;
	float brushSizeIndicator;
	float colorBoxPickerValue_x; 
	float colorBoxPickerValue_y;
	float colorBoxColorRangeBarValue;
	bool cameraPosChanged;
	const char* exportFolder;
	bool doPainting; 
	unsigned int depthTexture;
	bool paintingMode;
	float brushBlurRangeBarValue;
	float brushRotationRangeBarValue;
	float brushOpacityRangeBarValue;
	float brushSpacingRangeBarValue;
	float brushBorderRangeBarValue;

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
	void colorBoxColorRangeBar(float yOffset, int height);
	void colorBoxPickerButton(float xOffset, float yOffset, int width, int height);
	void brushSizeRangeBar(float xOffset, int width);
	void brushBlurRangeBar(float xOffset, int width, int height,bool renderTiny);
	void brushRotationRangeBar(float xOffset, int width, int height);
	void brushOpacityRangeBar(float xOffset, int width, int height);
	void brushSpacingRangeBar(float xOffset, int width, int height);
	void brushBordersRangeBar(float xOffset, int width, int height);
	void textureDemonstratorButton(float xOffset,float yOffset,int width,int height);
	void textureDemonstratorBoundaries(float xOffset,float yOffset,int width,int height);
	void paintingDropper();
	void paintingFillNumericModifier(bool p, bool n);
	void exportFileNameTextBox();
	void colorBox();
	void maskPanelSlider(float yOffset,int screenSizeY);
	void hexValTextbox();
	void loadCustomModel();
	void hueBar();
};
struct PanelData {
	bool movePanel;
	bool modelPanelActive;
	bool texturePanelActive;
	bool paintingPanelActive;
	bool exportPanelActive;
};
#endif // !MSHPAPP