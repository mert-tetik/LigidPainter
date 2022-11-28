#ifndef LSHPAPP_H
#define LSHPAPP_H
#include <vector>

struct CommonData {
	unsigned int program = 3;
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
};
struct ProjectionData
{
	glm::mat4 modelMat;
	glm::mat4 viewMat;
	glm::mat4 projMat;
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
	void brushBlurRangeBar(float xOffset, int width, int height);
	void brushRotationRangeBar(float xOffset, int width, int height);
	void brushOpacityRangeBar(float xOffset, int width, int height);
	void brushSpacingRangeBar(float xOffset, int width, int height);
	void textureDemonstratorButton(float xOffset,float yOffset,int width,int height);
	void textureDemonstratorBoundaries(float xOffset,float yOffset,int width,int height);
	void paintingDropper();
	void paintingFillNumericModifier(bool p, bool n);
	void exportFileNameTextBox();
	void colorBox();
	void maskPanelSlider(float yOffset,int screenSizeY);
};
struct PanelData {
	bool movePanel;
	bool modelPanelActive;
	bool texturePanelActive;
	bool paintingPanelActive;
	bool exportPanelActive;
};
#endif // !MSHPAPP