#ifndef RGDCALLBACK_H
#define RGDCALLBACK_H
struct CallbckData { //Return from callback
	bool loadModelButtonEnter;
	bool modelFilePathTextBoxEnter;
	bool autoTriangulateCheckBoxEnter;
	bool backfaceCullingCheckBoxEnter;
	bool addPlaneButtonEnter;
	bool addSphereButtonEnter;
	bool addImageButtonEnter;
	bool addMaskTextureButtonEnter;
	bool brushSizeRangeBarEnter;
	bool modelPanelButtonEnter;
	bool texturePanelButtonEnter;
	bool paintingPanelButtonEnter;
	bool exportPanelButtonEnter;
	bool colorBoxPickerEnter;
	bool colorBoxColorRangeBarEnter;

	bool exportPathTextBoxEnter;
	bool exportExtJPGButtonEnter;
	bool exportExtPNGButtonEnter;
	bool exportDownloadButtonEnter;

	glm::vec3 originPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraPos = glm::vec3(0.034906f, 0.000000f, -9.999939f);
	float panelLoc;

	bool movePanel;
};
class Callback {
public:
	CallbckData scroll_callback(GLFWwindow* window, double scroll, double scrollx);
	CallbckData mouse_callback(GLFWwindow* window, double xpos, double ypos, PanelData panelData, float brushSizeRangeBarValue, float colorBoxPickerValue_x, float colorBoxPickerValue_y, float colorBoxColorRangeBarValue);
	void panelCheck(GLFWwindow* window, int mouseXpos, int screenSizeX);
	void buttonCheck(GLFWwindow* window, int mouseXPos, int mouseYPos, PanelData panelData, float brushSizeRangeBarValue, float colorBoxPickerValue_x, float colorBoxPickerValue_y, float colorBoxColorRangeBarValue);
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};
#endif