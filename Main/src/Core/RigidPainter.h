#ifndef MSHPAPP_H
#define MSHPAPP_H

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
	bool isAutoTriangulateHover; 
	bool isAutoTriangulateChecked; 
	bool isbackfaceCullingHover; 
	bool isbackfaceCullingChecked; 
	bool backfaceCulling;
	float brushSizeRangeBarValue;
};
class Sphere {
public:
	std::vector<float> getSphere();
};
class Texture_Generator {
public:
	//
};
class RigidPainter {
public:
	void run();
};
struct PanelData {
	bool movePanel;
	bool modelPanelActive;
	bool texturePanelActive;
	bool paintingPanelActive;
};
#endif // !MSHPAPP