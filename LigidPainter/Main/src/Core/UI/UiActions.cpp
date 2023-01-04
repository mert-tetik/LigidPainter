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
void UiActions::isFirstClickDoneInside(GLFWwindow* window ,CallbckData callbackData,ColorPicker &colorPicker,TextureDisplayer &textureDisplayer,NodePanel &nodePanel) {
	if (glfwGetMouseButton(window, 0) == GLFW_PRESS && !clickTaken) {
		if (!callbackData.uiElementEnter && !callbackData.modelPanelButtonEnter && !callbackData.texturePanelButtonEnter && !callbackData.paintingPanelButtonEnter 
		&& !callbackData.exportPanelButtonEnter && !colorPicker.saturationValuePointerHover && !colorPicker.hueValuePointerHover && !colorPicker.hexValTextBoxEnter
		&& !colorPicker.dropperEnter && !colorPicker.hueValueBarHover && !colorPicker.saturationValueBoxHover && !callbackData.maskPanelEnter 
		&& !textureDisplayer.buttonHover && !callbackData.mainPanelBoundariesEnter && !textureDisplayer.cornerHover && !nodePanel.boundariesHover) {
			noButtonClick = true;
			colorBoxFirstPress = false;
		}
		else {
			if(colorPicker.saturationValueBoxHover){
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
bool maskPanelSliderPressed;
bool mainPanelBoundariesPressed;
bool nodePanelBoundariesPressed;
//

bool buttonGetInput = true;
bool buttonPressed = false;
void UiActions::uiActions(GLFWwindow* window ,CallbckData callbackData,std::vector<UIElement> &UIElements, ColorPicker &colorPicker,TextureDisplayer &textureDisplayer,NodePanel &nodePanel) {
    LigidPainter ligid;
	isFirstClickDoneInside(window ,callbackData,colorPicker,textureDisplayer,nodePanel);

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
				else if (colorPicker.saturationValuePointerHover) {
					colorBoxPickerPressed = true;
				}
				else if (colorPicker.hueValuePointerHover) {
					colorBoxColorRangeBarPressed= true;
				}
				else if (textureDisplayer.buttonHover) {
					textureDisplayer.buttonPressed = true;
				}
				else if(callbackData.maskPanelSliderEnter){
					maskPanelSliderPressed = true;
				}
				else if (textureDisplayer.cornerHover) { //Keep at the end
					textureDisplayer.cornerPressed = true;
				}
				else if(callbackData.mainPanelBoundariesEnter){
					mainPanelBoundariesPressed = true;
				}
				else if(nodePanel.boundariesHover){
					nodePanelBoundariesPressed = true;
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
				if (colorPicker.dropperEnter)
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
				if(colorPicker.saturationValueBoxHover&& colorBoxFirstPress)
					ligid.colorBox();
				if(colorPicker.hueValueBarHover)
					ligid.hueBar();
				if(colorPicker.hexValTextBoxEnter)
					ligid.hexValTextbox();
				if(UIElements[UIloadCustomModelButton].button.hover)
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
		textureDisplayer.buttonPressed = false;
		textureDisplayer.cornerPressed = false;
        maskPanelSliderPressed = false;
		mainPanelBoundariesPressed = false;
		nodePanelBoundariesPressed = false;
	}
}
bool UiActions::updateRangeValues(GLFWwindow* window, double xOffset,double yOffset, int screenWidth, int screenHeight,TextureDisplayer &textureDisplayer){
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
	if (textureDisplayer.buttonPressed) {
		ligid.textureDisplayerButton(xOffset,yOffset,screenWidth,screenHeight);//Changes the global variable
	}
	if (textureDisplayer.cornerPressed) {
		ligid.textureDisplayerBoundaries(xOffset,yOffset,screenWidth,screenHeight);//Changes the global variable
	}
	if(maskPanelSliderPressed){
		ligid.maskPanelSlider(yOffset,uiactionsMaxScreenHeight);//Changes the global variable
	}
	if(mainPanelBoundariesPressed){
		ligid.mainPanelBoundaries(xOffset,screenWidth);
	}
	if(nodePanelBoundariesPressed){
		ligid.nodePanelBoundaries(yOffset,screenHeight);
	}
    if (colorBoxPickerPressed || colorBoxColorRangeBarPressed || brushBlurRangeBarPressed || brushSizeRangeBarPressed || brushRotationRangeBarPressed || brushOpacityRangeBarPressed || brushSpacingRangeBarPressed|| brushBordersRangeBarPressed || textureDisplayer.buttonPressed || textureDisplayer.cornerPressed || maskPanelSliderPressed) { //Set cursor as hidden and restrict panel movement if any of the rangebars value is changing
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
