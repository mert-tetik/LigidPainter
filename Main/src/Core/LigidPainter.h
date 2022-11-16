#ifndef LSHPAPP_H
#define LSHPAPP_H

#include <vector>

struct CommonData {
	unsigned int program = 3;
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
	unsigned int paintingSqFBO; 
	float colorBoxColorRangeBarValue;
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
};
struct PanelData {
	bool movePanel;
	bool modelPanelActive;
	bool texturePanelActive;
	bool paintingPanelActive;
	bool exportPanelActive;
};
#endif // !MSHPAPP