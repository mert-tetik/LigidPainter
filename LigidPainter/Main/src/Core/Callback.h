#ifndef LGDCALLBACK_H
#define LGDCALLBACK_H
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
	bool brushBlurRangeBarEnter;
	bool brushRotationRangeBarEnter;
	bool brushSpacingRangeBarEnter;
	bool brushOpacityRangeBarEnter;
	bool modelPanelButtonEnter;
	bool texturePanelButtonEnter;
	bool paintingPanelButtonEnter;
	bool exportPanelButtonEnter;
	bool colorBoxPickerEnter;
	bool colorBoxColorRangeBarEnter;

	bool exportPathTextBoxEnter;
	bool exportFileNameTextBoxEnter;
	bool exportExtJPGCheckBoxEnter;
	bool exportExtPNGCheckBoxEnter;
	bool exportDownloadButtonEnter;

	bool mirrorXCheckBoxEnter;
	bool mirrorYCheckBoxEnter;
	bool mirrorZCheckBoxEnter;

	bool textureDemonstratorButtonEnter;

	bool useNegativeForDrawingCheckboxEnter;

	bool paintingDropperEnter;

	bool paintingFillNumericModifierPEnter;
	bool paintingFillNumericModifierNEnter;

	bool colorBoxEnter;

	bool maskPanelSliderEnter;
	bool maskPanelEnter;

	bool hexValueTextboxEnter;

	glm::vec3 originPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraPos = glm::vec3(0.034906f, 0.000000f, -9.999939f);
	float panelLoc;

	bool movePanel;
	bool panelChangeLoc; 
};
class Callback {
public:
	CallbckData scroll_callback(GLFWwindow* window, double scroll, double scrollx);
	CallbckData mouse_callback(GLFWwindow* window, double xpos, double ypos, PanelData panelData, float brushSizeRangeBarValue, float colorBoxPickerValue_x, float colorBoxPickerValue_y, float colorBoxColorRangeBarValue, float brushBlurRangeBarValue, bool enablePanelMovement, float brushRotationRangeBarValue, float brushOpacityRangeBarValue, float brushSpacingRangeBarValue,float textureDemonstratorButtonPosX,float textureDemonstratorButtonPosY,float maskPanelSliderValue,bool brushMaskPanelMaskHover,GLFWcursor* pointerCursor,GLFWcursor* defaultCursor);
	void panelCheck(GLFWwindow* window, int mouseXpos, int screenSizeX, bool enablePanelMovement);
	void buttonCheck(GLFWwindow* window, int mouseXPos, int mouseYPos, PanelData panelData, float brushSizeRangeBarValue, float colorBoxPickerValue_x, float colorBoxPickerValue_y, float colorBoxColorRangeBarValue, float brushBlurRangeBarValue, float brushRotationRangeBarValue, float brushOpacityRangeBarValue, float brushSpacingRangeBarValue,float textureDemonstratorButtonPosX,float textureDemonstratorButtonPosY,float maskPanelSliderValue,bool brushMaskPanelMaskHover,GLFWcursor* pointerCursor,GLFWcursor* defaultCursor);
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight);

};
#endif