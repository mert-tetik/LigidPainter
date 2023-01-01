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
		&& !callbackData.paintingDropperEnter && !callbackData.hueBarEnter && !callbackData.colorBoxEnter && !callbackData.maskPanelEnter 
		&& !callbackData.textureDemonstratorButtonEnter && !callbackData.mainPanelBoundariesEnter) {
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
bool mainPanelBoundariesPressed;
//

bool buttonGetInput = true;
bool buttonPressed = false;
UiActionsData UiActions::uiActions(GLFWwindow* window ,CallbckData callbackData,bool textureDemonstratorBoundariesHover, std::vector<UIElement> &UIElements) {
    LigidPainter ligid;
	isFirstClickDoneInside(window ,callbackData,textureDemonstratorBoundariesHover);

	if (!noButtonClick) {
		if (buttonGetInput) {
			if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {
				buttonGetInput = false;
				buttonPressed = true;
				if (UIElements[UIbrushSizeRangeBar].rangeBar.hover) {
					brushSizeRangeBarPressed = true;
				}
				else if (UIElements[UIbrushBlurRangeBar].rangeBar.hover) {
					brushBlurRangeBarPressed = true;
				}
				else if (UIElements[UIbrushRotationRangeBar].rangeBar.hover) {
					brushRotationRangeBarPressed = true;
				}
				else if (UIElements[UIbrushOpacityRangeBar].rangeBar.hover) {
					brushOpacityRangeBarPressed = true;
				}
				else if (UIElements[UIbrushSpacingRangeBar].rangeBar.hover) {
					brushSpacingRangeBarPressed = true;
				}
				else if (UIElements[UIbrushBordersRangeBar].rangeBar.hover) {
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
				else if(callbackData.mainPanelBoundariesEnter){
					mainPanelBoundariesPressed = true;
				}
			}
		}
		if (glfwGetMouseButton(window, 0) == GLFW_RELEASE) {
			buttonGetInput = true;
			if (buttonPressed) {
				//Check mouse hover
				if (UIElements[UIaddBrushMaskTextureButton].button.hover)
					ligid.addMaskTextureButton();
				if (UIElements[UIUploadingModelPathTextBox].textBox.hover)
					ligid.modelFilePathTextBox(); 
				if (callbackData.modelPanelButtonEnter)
					ligid.modelPanelButton();
				if (callbackData.texturePanelButtonEnter)
					ligid.texturePanelButton();
				if (callbackData.paintingPanelButtonEnter)
					ligid.paintingPanelButton();
				if (callbackData.exportPanelButtonEnter)
					ligid.exportPanelButton();
				if (UIElements[UIloadPlaneModelButton].button.hover)
					ligid.addPlaneButton();
				if (UIElements[UIloadSphereModelButton].button.hover)
					ligid.addSphereButton();
				if (UIElements[UIautoTriangulateCheckBox].checkBox.mouseHover)
					ligid.autoTriangulateCheckBox();
				if (UIElements[UIbackfaceCullingCheckBox].checkBox.mouseHover)
					ligid.backfaceCullingCheckBox();
				if (UIElements[UIuseNegativeCheckBox].checkBox.mouseHover)
					ligid.useNegativeForDrawingCheckbox();
				if (UIElements[UIloadModelButton].button.hover)
					ligid.loadModelButton();
				if (callbackData.paintingDropperEnter)
					ligid.paintingDropper();
				if (UIElements[UIexportingPathTextBox].textBox.hover)
					ligid.exportPathTextBox();
				if (UIElements[UIexportingFolderNameTextBox].textBox.hover){
					UIElements[UIexportingFolderNameTextBox].textBox.clicked = !UIElements[UIexportingFolderNameTextBox].textBox.clicked; 
					ligid.exportFileNameTextBox();
				}
				if (UIElements[UIdownloadButton].button.hover)
					ligid.exportDownloadButtonEnter();
				if (UIElements[UIjpgCheckBox].checkBox.mouseHover)
					ligid.exportExtJPGCheckBox();
				if (UIElements[UIpngCheckBox].checkBox.mouseHover)
					ligid.exportExtPNGCheckBox();
				if (UIElements[UImirrorXCheckBox].checkBox.mouseHover)
					ligid.mirrorXCheckBox();
				if (UIElements[UImirrorYCheckBox].checkBox.mouseHover)
					ligid.mirrorYCheckBox();
				if (UIElements[UImirrorZCheckBox].checkBox.mouseHover)
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
		mainPanelBoundariesPressed = false;
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
	if(mainPanelBoundariesPressed){
		ligid.mainPanelBoundaries(xOffset,screenWidth);
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
