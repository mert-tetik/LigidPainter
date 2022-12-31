#include<iostream>

#include "Core/LigidPainter.h"
#include "Core/Callback.h"
#include "Core/gl.h"
#include "Core/UI/UiActions.h"

bool noButtonClick;

bool clickTaken = false;

int uiactionsMaxScreenWidth;
int uiactionsMaxScreenHeight;

bool colorBoxFirstPress = false;
void UiActions::isFirstClickDoneInside(GLFWwindow* window ,CallbckData callbackData,bool textureDemonstratorBoundariesHover) {
	if (glfwGetMouseButton(window, 0) == GLFW_PRESS && !clickTaken) {
		if (!callbackData.uiElementEnter && !callbackData.modelPanelButtonEnter && !callbackData.texturePanelButtonEnter && !callbackData.paintingPanelButtonEnter 
		&& !callbackData.exportPanelButtonEnter && !callbackData.colorBoxPickerEnter && !callbackData.colorBoxColorRangeBarEnter && !callbackData.hexValueTextboxEnter
		&& !callbackData.paintingDropperEnter && !callbackData.hueBarEnter && !callbackData.colorBoxEnter && !callbackData.maskPanelEnter && !callbackData.textureDemonstratorButtonEnter) {
			noButtonClick = true;
			colorBoxFirstPress = false;

		}
		else {
			if(callbackData.colorBoxEnter){
				colorBoxFirstPress = true;
			}
			else{
				colorBoxFirstPress = false;
			}
			noButtonClick = false;
		}
		clickTaken = true;
	}
	if (glfwGetMouseButton(window, 0) == GLFW_RELEASE && clickTaken) {
		clickTaken = false;
	}
}
//Range Values
bool brushSizeRangeBarPressed;
bool brushBlurRangeBarPressed;
bool brushRotationRangeBarPressed;
bool brushOpacityRangeBarPressed;
bool brushSpacingRangeBarPressed;
bool brushBordersRangeBarPressed;
bool colorBoxColorRangeBarPressed;
bool colorBoxPickerPressed;
bool textureDemonstratorButtonPressed;
bool textureDemonstratorBoundariesPressed;
bool maskPanelSliderPressed;
//

bool buttonGetInput = true;
bool buttonPressed = false;
UiActionsData UiActions::uiActions(GLFWwindow* window ,CallbckData callbackData,bool textureDemonstratorBoundariesHover, UI &UIElements) {
    LigidPainter ligid;
	isFirstClickDoneInside(window ,callbackData,textureDemonstratorBoundariesHover);

	if (!noButtonClick) {
		if (buttonGetInput) {
			if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {
				buttonGetInput = false;
				buttonPressed = true;
				if (UIElements.uiElements["brushSizeRangeBar"].rangeBar.hover) {
					brushSizeRangeBarPressed = true;
				}
				else if (UIElements.uiElements["brushBlurRangeBar"].rangeBar.hover) {
					brushBlurRangeBarPressed = true;
				}
				else if (UIElements.uiElements["brushRotationRangeBar"].rangeBar.hover) {
					brushRotationRangeBarPressed = true;
				}
				else if (UIElements.uiElements["brushOpacityRangeBar"].rangeBar.hover) {
					brushOpacityRangeBarPressed = true;
				}
				else if (UIElements.uiElements["brushSpacingRangeBar"].rangeBar.hover) {
					brushSpacingRangeBarPressed = true;
				}
				else if (UIElements.uiElements["brushBordersRangeBar"].rangeBar.hover) {
					brushBordersRangeBarPressed = true;
				}
				else if (callbackData.colorBoxPickerEnter) {
					colorBoxPickerPressed = true;
				}
				else if (callbackData.colorBoxColorRangeBarEnter) {
					colorBoxColorRangeBarPressed= true;
				}
				else if (callbackData.textureDemonstratorButtonEnter) {
					textureDemonstratorButtonPressed = true;
				}
				else if(callbackData.maskPanelSliderEnter){
					maskPanelSliderPressed = true;
				}
				else if (textureDemonstratorBoundariesHover) { //Keep at the end
					textureDemonstratorBoundariesPressed = true;
				}
			}
		}
		if (glfwGetMouseButton(window, 0) == GLFW_RELEASE) {
			buttonGetInput = true;
			if (buttonPressed) {
				//Check mouse hover
				if (UIElements.uiElements["addBrushMaskTextureButton"].button.hover)
					ligid.addMaskTextureButton();
				if (UIElements.uiElements["uploadingModelPathTextBox"].textBox.hover){
					ligid.modelFilePathTextBox(); 
				}
				if (callbackData.modelPanelButtonEnter)
					ligid.modelPanelButton();
				if (callbackData.texturePanelButtonEnter)
					ligid.texturePanelButton();
				if (callbackData.paintingPanelButtonEnter)
					ligid.paintingPanelButton();
				if (callbackData.exportPanelButtonEnter)
					ligid.exportPanelButton();
				if (UIElements.uiElements["loadPlaneModelButton"].button.hover)
					ligid.addPlaneButton();
				if (UIElements.uiElements["loadSphereModelButton"].button.hover)
					ligid.addSphereButton();
				if (UIElements.uiElements["autoTriangulateCheckBox"].checkBox.mouseHover)
					ligid.autoTriangulateCheckBox();
				if (UIElements.uiElements["backfaceCullingCheckBox"].checkBox.mouseHover)
					ligid.backfaceCullingCheckBox();
				if (UIElements.uiElements["useNegativeCheckBox"].checkBox.mouseHover)
					ligid.useNegativeForDrawingCheckbox();
				if (UIElements.uiElements["loadModelButton"].button.hover)
					ligid.loadModelButton();
				if (callbackData.paintingDropperEnter)
					ligid.paintingDropper();
				if (UIElements.uiElements["exportingPathTextBox"].textBox.hover){
					ligid.exportPathTextBox();
				}
				if (UIElements.uiElements["exportingFolderNameTextBox"].textBox.hover){
					UIElements.uiElements["exportingFolderNameTextBox"].textBox.clicked = !UIElements.uiElements["exportingFolderNameTextBox"].textBox.clicked; 
					ligid.exportFileNameTextBox();
				}
				if (UIElements.uiElements["downloadButton"].button.hover)
					ligid.exportDownloadButtonEnter();
				if (UIElements.uiElements["jpgCheckBox"].checkBox.mouseHover)
					ligid.exportExtJPGCheckBox();
				if (UIElements.uiElements["pngCheckBox"].checkBox.mouseHover)
					ligid.exportExtPNGCheckBox();
				if (UIElements.uiElements["mirrorXCheckBox"].checkBox.mouseHover)
					ligid.mirrorXCheckBox();
				if (UIElements.uiElements["mirrorYCheckBox"].checkBox.mouseHover)
					ligid.mirrorYCheckBox();
				if (UIElements.uiElements["mirrorZCheckBox"].checkBox.mouseHover)
					ligid.mirrorZCheckBox();
				if(callbackData.colorBoxEnter && colorBoxFirstPress)
					ligid.colorBox();
				if(callbackData.hueBarEnter)
					ligid.hueBar();
				if(callbackData.hexValueTextboxEnter)
					ligid.hexValTextbox();
				if(callbackData.loadCustomModelEnter)
					ligid.loadCustomModel();
			}
			buttonPressed = false;
		}
	}
    if(glfwGetMouseButton(window, 0) == GLFW_RELEASE){
		brushSizeRangeBarPressed = false;
		brushBlurRangeBarPressed = false;
		brushRotationRangeBarPressed = false;
		brushOpacityRangeBarPressed = false;
		brushSpacingRangeBarPressed = false;
		brushBordersRangeBarPressed = false;
		colorBoxPickerPressed = false;
		colorBoxColorRangeBarPressed = false;
		textureDemonstratorButtonPressed = false;
		textureDemonstratorBoundariesPressed = false;
        maskPanelSliderPressed = false;
	}
    UiActionsData uiData;
    uiData.textureDemonstratorBoundariesPressed =textureDemonstratorBoundariesPressed;
    uiData.textureDemonstratorButtonPressed = textureDemonstratorButtonPressed;
    return uiData;
}
bool UiActions::updateRangeValues(GLFWwindow* window, double xOffset,double yOffset, int screenWidth, int screenHeight){
    bool hideCursor;
    LigidPainter ligid;
    
    if (brushSizeRangeBarPressed) {
		ligid.brushSizeRangeBar(xOffset,screenWidth);//Changes the global variable
	}
	if (brushBlurRangeBarPressed) {
		ligid.brushBlurRangeBar(xOffset,screenWidth,screenHeight,true);//Changes the global variable
	}
	if (brushRotationRangeBarPressed) {
		ligid.brushRotationRangeBar(xOffset, screenWidth, screenHeight);//Changes the global variable
	}
	if (brushOpacityRangeBarPressed) {
		ligid.brushOpacityRangeBar(xOffset, screenWidth, screenHeight);//Changes the global variable
	}
	if (brushSpacingRangeBarPressed) {
		ligid.brushSpacingRangeBar(xOffset, screenWidth, screenHeight);//Changes the global variable
	}
	if (brushBordersRangeBarPressed) {
		ligid.brushBordersRangeBar(xOffset, screenWidth, screenHeight);//Changes the global variable
	}
	if (colorBoxColorRangeBarPressed) {
		ligid.colorBoxColorRangeBar(yOffset,screenHeight);//Changes the global variable
	}
	if (colorBoxPickerPressed) {
		ligid.colorBoxPickerButton(xOffset,yOffset,screenWidth,screenHeight);//Changes the global variable
	}
	if (textureDemonstratorButtonPressed) {
		ligid.textureDemonstratorButton(xOffset,yOffset,screenWidth,screenHeight);//Changes the global variable
	}
	if (textureDemonstratorBoundariesPressed) {
		ligid.textureDemonstratorBoundaries(xOffset,yOffset,screenWidth,screenHeight);//Changes the global variable
	}
	if(maskPanelSliderPressed){
		ligid.maskPanelSlider(yOffset,uiactionsMaxScreenHeight);//Changes the global variable
	}
    if (colorBoxPickerPressed || colorBoxColorRangeBarPressed || brushBlurRangeBarPressed || brushSizeRangeBarPressed || brushRotationRangeBarPressed || brushOpacityRangeBarPressed || brushSpacingRangeBarPressed|| brushBordersRangeBarPressed || textureDemonstratorButtonPressed || textureDemonstratorBoundariesPressed || maskPanelSliderPressed) { //Set cursor as hidden and restrict panel movement if any of the rangebars value is changing
        hideCursor = true;
	}
	else {
        hideCursor = false;
	}

    return hideCursor;
}

void UiActions::sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight){
	uiactionsMaxScreenHeight = maxScreenHeight;
	uiactionsMaxScreenWidth = maxScreenWidth;
}
