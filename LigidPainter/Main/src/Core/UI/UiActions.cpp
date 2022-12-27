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
		if (!callbackData.addMaskTextureButtonEnter && !callbackData.addPlaneButtonEnter
			//Check all the buttons
			&& !callbackData.addSphereButtonEnter && !callbackData.autoTriangulateCheckBoxEnter && !callbackData.backfaceCullingCheckBoxEnter
			&& !callbackData.brushSizeRangeBarEnter && !callbackData.loadModelButtonEnter && !callbackData.modelFilePathTextBoxEnter
			&& !callbackData.modelPanelButtonEnter && !callbackData.paintingPanelButtonEnter && !callbackData.exportPanelButtonEnter && !callbackData.texturePanelButtonEnter && !callbackData.colorBoxPickerEnter
			&& !callbackData.colorBoxColorRangeBarEnter && !callbackData.exportPathTextBoxEnter && !callbackData.exportDownloadButtonEnter && !callbackData.exportExtJPGCheckBoxEnter && !callbackData.exportExtPNGCheckBoxEnter
			&& !callbackData.brushBlurRangeBarEnter && !callbackData.brushRotationRangeBarEnter && !callbackData.brushOpacityRangeBarEnter && !callbackData.brushSpacingRangeBarEnter && !callbackData.brushBordersRangeBarEnter
			&& !callbackData.mirrorXCheckBoxEnter && !callbackData.mirrorYCheckBoxEnter && !callbackData.mirrorZCheckBoxEnter && !callbackData.textureDemonstratorButtonEnter && !textureDemonstratorBoundariesHover && !callbackData.useNegativeForDrawingCheckboxEnter
			&& !callbackData.paintingDropperEnter && !callbackData.paintingFillNumericModifierPEnter && !callbackData.paintingFillNumericModifierNEnter && !callbackData.exportFileNameTextBoxEnter && !callbackData.colorBoxEnter && !callbackData.maskPanelSliderEnter 
			&& !callbackData.hexValueTextboxEnter && !callbackData.loadCustomModelEnter && !callbackData.hueBarEnter) {
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
UiActionsData UiActions::uiActions(GLFWwindow* window ,CallbckData callbackData,bool textureDemonstratorBoundariesHover) {
    LigidPainter ligid;
	isFirstClickDoneInside(window ,callbackData,textureDemonstratorBoundariesHover);
	if (!noButtonClick) {
		if (buttonGetInput) {
			if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {
				buttonGetInput = false;
				buttonPressed = true;
				if (callbackData.brushSizeRangeBarEnter) {
					brushSizeRangeBarPressed = true;
				}
				else if (callbackData.brushBlurRangeBarEnter) {
					brushBlurRangeBarPressed = true;
				}
				else if (callbackData.brushRotationRangeBarEnter) {
					brushRotationRangeBarPressed = true;
				}
				else if (callbackData.brushOpacityRangeBarEnter) {
					brushOpacityRangeBarPressed = true;
				}
				else if (callbackData.brushSpacingRangeBarEnter) {
					brushSpacingRangeBarPressed = true;
				}
				else if (callbackData.brushBordersRangeBarEnter) {
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
				if (callbackData.addMaskTextureButtonEnter)
					ligid.addMaskTextureButton();
				if (callbackData.modelFilePathTextBoxEnter)
					ligid.modelFilePathTextBox(); 
				if (callbackData.modelPanelButtonEnter)
					ligid.modelPanelButton();
				if (callbackData.texturePanelButtonEnter)
					ligid.texturePanelButton();
				if (callbackData.paintingPanelButtonEnter)
					ligid.paintingPanelButton();
				if (callbackData.exportPanelButtonEnter)
					ligid.exportPanelButton();
				if (callbackData.addPlaneButtonEnter)
					ligid.addPlaneButton();
				if (callbackData.addSphereButtonEnter)
					ligid.addSphereButton();
				if (callbackData.autoTriangulateCheckBoxEnter)
					ligid.autoTriangulateCheckBox();
				if (callbackData.backfaceCullingCheckBoxEnter)
					ligid.backfaceCullingCheckBox();
				if (callbackData.useNegativeForDrawingCheckboxEnter)
					ligid.useNegativeForDrawingCheckbox();
				if (callbackData.loadModelButtonEnter)
					ligid.loadModelButton();
				if (callbackData.paintingDropperEnter)
					ligid.paintingDropper();
				if (callbackData.exportPathTextBoxEnter) 
					ligid.exportPathTextBox();
				if (callbackData.exportFileNameTextBoxEnter) 
					ligid.exportFileNameTextBox();
				if (callbackData.exportDownloadButtonEnter)
					ligid.exportDownloadButtonEnter();
				if (callbackData.exportExtJPGCheckBoxEnter)
					ligid.exportExtJPGCheckBox();
				if (callbackData.exportExtPNGCheckBoxEnter)
					ligid.exportExtPNGCheckBox();
				if (callbackData.mirrorXCheckBoxEnter)
					ligid.mirrorXCheckBox();
				if (callbackData.mirrorYCheckBoxEnter)
					ligid.mirrorYCheckBox();
				if (callbackData.mirrorZCheckBoxEnter)
					ligid.mirrorZCheckBox();
				if(callbackData.paintingFillNumericModifierPEnter)
					ligid.paintingFillNumericModifier(1,0);
				if(callbackData.paintingFillNumericModifierNEnter)
					ligid.paintingFillNumericModifier(0,1);
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
