#ifndef LGDCALLBACK_H
#define LGDCALLBACK_H

#include "Core/UI/UserInterface.h"

struct CallbckData { //Return from callback
	bool movePanel;
	bool modelPanelButtonEnter;
	bool texturePanelButtonEnter;
	bool paintingPanelButtonEnter;
	bool exportPanelButtonEnter;
	bool settingsPanelButtonEnter;
	bool generatorPanelButtonEnter;
	bool maskPanelSliderEnter;
	bool maskPanelEnter;
	bool uiElementEnter;
	bool mainPanelBoundariesEnter;
	bool mainPanelEnter;

	glm::vec3 originPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraPos = glm::vec3(0.034906f, 0.000000f, -9.999939f);
};
class Callback {
public:
	CallbckData scroll_callback(GLFWwindow* window, double scroll, double scrollx);
	CallbckData mouse_callback(GLFWwindow* window, double xpos, double ypos, PanelData &panelData, float maskPanelSliderValue,bool brushMaskPanelMaskHover,LigidCursors cursors,bool texturePanelButtonHover,std::vector<UIElement> &uiElements,float mainPanelLoc,ColorPicker &colorPicker,TextureDisplayer &textureDisplayer,NodePanel &nodePanel,ContextMenu &addNodeContextMenu,SndPanel &sndPanel,ColoringPanel &coloringPanel);
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight);
	void key_callback(int key,int action,GLFWwindow* window);

private:
	void buttonCheck(GLFWwindow* window, double mouseXPos,double mouseYPos,PanelData &panelData,float maskPanelSliderValue,bool brushMaskPanelMaskHover,LigidCursors cursors,bool texturePanelButtonHover,std::vector<UIElement> &uiElements,float mainPanelLoc, ColorPicker &colorPicker,TextureDisplayer &textureDisplayer,NodePanel &nodePanel,ContextMenu &addNodeContextMenu,SndPanel &sndPanel,ColoringPanel &coloringPanel);
};
#endif