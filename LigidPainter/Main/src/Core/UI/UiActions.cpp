#include<iostream>

#include "Core/LigidPainter.h"
#include "Core/Callback.h"
#include "Core/gl.h"
#include "Core/UI/UiActions.h"


//Range Values
bool brushSizeRangeBarPressed;
bool brushBlurRangeBarPressed;
bool brushRotationRangeBarPressed;
bool brushOpacityRangeBarPressed;
bool brushSpacingRangeBarPressed;
bool brushBordersRangeBarPressed;
bool skyboxExposureRangeBarPressed;
bool skyboxRotationRangeBarPressed;
bool normalStrengthRangeBarPressed;
bool noiseStrengthRangeBarPressed;
bool colorBoxColorRangeBarPressed;
bool colorBoxPickerPressed;
bool maskPanelSliderPressed;
bool mainPanelBoundariesPressed;
bool nodePanelBoundariesPressed;
bool subSelectedImagePowerRangeBarPressed;
bool mirrorXRangeBarPressed;
bool mirrorYRangeBarPressed;
bool mirrorZRangeBarPressed;
//

void UiActions::uiActions(GLFWwindow* window ,CallbckData callbackData,std::vector<UIElement> &UIElements, ColorPicker &colorPicker,TextureDisplayer &textureDisplayer,NodePanel &nodePanel,SndPanel &sndPanel,bool &firstClick) {
    LigidPainter ligid;

	if (firstClick) {
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
		else if (UIElements[UIskyBoxExposureRangeBar].rangeBar.hover) {
			skyboxExposureRangeBarPressed = true;
		}
		else if (UIElements[UIskyBoxRotationRangeBar].rangeBar.hover) {
			skyboxRotationRangeBarPressed = true;
		}
		else if (UIElements[UInormalStrengthRangeBarElement].rangeBar.hover) {
			normalStrengthRangeBarPressed = true;
		}
		else if (UIElements[UInoiseStrengthRangeBarElement].rangeBar.hover) {
			noiseStrengthRangeBarPressed = true;
		}
		else if (UIElements[UIsubSelectedImagePowerRangeBarElement].rangeBar.hover) {
			subSelectedImagePowerRangeBarPressed = true;
		}
		else if (UIElements[UImirrorXRangeBarElement].rangeBar.hover) {
			mirrorXRangeBarPressed = true;
		}
		else if (UIElements[UImirrorYRangeBarElement].rangeBar.hover) {
			mirrorYRangeBarPressed = true;
		}
		else if (UIElements[UImirrorZRangeBarElement].rangeBar.hover) {
			mirrorZRangeBarPressed = true;
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
			nodePanel.boundariesPressed = true;
		}
		else if(sndPanel.boundariesHover){
			sndPanel.boundariesPressed = true;
		}
				//Check mouse hover
		if (UIElements[UIaddBrushMaskTextureButton].button.hover)
			ligid.addMaskTextureButton();
		if (UIElements[UIbakeButtonElement].button.hover)
			ligid.bakeButton();
		if (UIElements[UIUploadingModelPathTextBox].textBox.hover)
			ligid.modelFilePathTextBox(); 
		if (UIElements[UIgenerateTextureButtonElement].button.hover)
			ligid.generateTextureButton(); 
		if (callbackData.modelPanelButtonEnter)
			ligid.modelPanelButton();
		if (callbackData.texturePanelButtonEnter)
			ligid.texturePanelButton();
		if (callbackData.paintingPanelButtonEnter)
			ligid.paintingPanelButton();
		if (callbackData.exportPanelButtonEnter)
			ligid.exportPanelButton();
		if (callbackData.settingsPanelButtonEnter)
			ligid.settingsPanelButton();
		if (callbackData.generatorPanelButtonEnter)
			ligid.generatorPanelButton();
		if (UIElements[UIloadPlaneModelButton].icon.hover)
			ligid.addPlaneButton();
		if (UIElements[UIloadSphereModelButton].icon.hover)
			ligid.addSphereButton();
		if (UIElements[UIautoTriangulateCheckBox].checkBox.mouseHover)
			ligid.autoTriangulateCheckBox();
		if (UIElements[UIbackfaceCullingCheckBox].checkBox.mouseHover)
			ligid.backfaceCullingCheckBox();
		if(UIElements[UIskyboxCheckBox].checkBox.mouseHover)
			UIElements[UIskyboxCheckBox].checkBox.checked = !UIElements[UIskyboxCheckBox].checkBox.checked;
		if(UIElements[UIfocusModeCheckBox].checkBox.mouseHover)
			UIElements[UIfocusModeCheckBox].checkBox.checked = !UIElements[UIfocusModeCheckBox].checkBox.checked;

		if(UIElements[UInormalmapCheckBoxElement].checkBox.mouseHover && !UIElements[UInormalmapCheckBoxElement].checkBox.checked){
			UIElements[UInormalmapCheckBoxElement].checkBox.checked = !UIElements[UInormalmapCheckBoxElement].checkBox.checked;
			UIElements[UInoiseCheckBoxElement].checkBox.checked = !UIElements[UInormalmapCheckBoxElement].checkBox.checked;

		}
		if(UIElements[UInoiseCheckBoxElement].checkBox.mouseHover && !UIElements[UInoiseCheckBoxElement].checkBox.checked){
			UIElements[UInoiseCheckBoxElement].checkBox.checked = !UIElements[UInoiseCheckBoxElement].checkBox.checked;
			UIElements[UInormalmapCheckBoxElement].checkBox.checked = !UIElements[UInoiseCheckBoxElement].checkBox.checked;

		}
		
		if(UIElements[UIstabilizeFpsCheckBox].checkBox.mouseHover)
			UIElements[UIstabilizeFpsCheckBox].checkBox.checked = !UIElements[UIstabilizeFpsCheckBox].checkBox.checked;
		if(UIElements[UIimageCheckBoxElement].checkBox.mouseHover)
			UIElements[UIimageCheckBoxElement].checkBox.checked = !UIElements[UIimageCheckBoxElement].checkBox.checked;
		if (UIElements[UIuseNegativeCheckBox].checkBox.mouseHover)
			ligid.useNegativeForDrawingCheckbox();
		if (UIElements[UIloadModelButton].button.hover)
			ligid.loadModelButton();
		if (UIElements[UIviewportImageTextBoxElement].textBox.hover)
			ligid.viewportImageTextbox();
		if (colorPicker.dropperEnter)
			ligid.paintingDropper();
		if (UIElements[UIexportingPathTextBox].textBox.hover)
			ligid.exportPathTextBox();
		if (UIElements[UIexportingFolderNameTextBox].textBox.hover){
			UIElements[UIexportingFolderNameTextBox].textBox.clicked = !UIElements[UIexportingFolderNameTextBox].textBox.clicked; 
			ligid.exportFileNameTextBox();
		}
		if (UIElements[UIpaintThroughCheckBoxElement].checkBox.mouseHover)
			UIElements[UIpaintThroughCheckBoxElement].checkBox.checked = !UIElements[UIpaintThroughCheckBoxElement].checkBox.checked; 
		
		if (UIElements[UImaskPaintingCheckBoxElement].checkBox.mouseHover){
			UIElements[UImaskPaintingCheckBoxElement].checkBox.checked = true; 
			UIElements[UIcolorPaintingCheckBoxElement].checkBox.checked = false; 
			UIElements[UInormalmapPaintingCheckBoxElement].checkBox.checked = false; 
		}
		if (UIElements[UIcolorPaintingCheckBoxElement].checkBox.mouseHover){
			UIElements[UIcolorPaintingCheckBoxElement].checkBox.checked = true; 
			UIElements[UImaskPaintingCheckBoxElement].checkBox.checked = false; 
			UIElements[UInormalmapPaintingCheckBoxElement].checkBox.checked = false; 
		}
		if (UIElements[UInormalmapPaintingCheckBoxElement].checkBox.mouseHover){
			UIElements[UInormalmapPaintingCheckBoxElement].checkBox.checked = true; 
			UIElements[UIcolorPaintingCheckBoxElement].checkBox.checked = false; 
			UIElements[UImaskPaintingCheckBoxElement].checkBox.checked = false; 
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
		if(colorPicker.saturationValueBoxHover){
			ligid.colorBox();
			colorBoxPickerPressed = true; 
		}
		if(colorPicker.hueValueBarHover){
			ligid.hueBar();
			colorBoxColorRangeBarPressed = true;
		}
		
		if(colorPicker.hexValTextBoxEnter){
			ligid.hexValTextbox();

		}
		if(UIElements[UIloadCustomModelButton].button.hover)
			ligid.loadCustomModel();
		if(sndPanel.plusSignHover)
			ligid.sndPanelPlusIcon();
		if(sndPanel.downSignHover)
			ligid.sndPanelDownIcon();
		if(sndPanel.folderSignHover)
			ligid.sndPanelFolderIcon();
		if(sndPanel.backSignHover)
			ligid.sndPanelBackIcon();
		if(sndPanel.minusSignHover)
			ligid.sndPanelMinusIcon();
		if(sndPanel.duplicateSignHover)
			ligid.sndPanelDuplicateIcon();
		if(sndPanel.texturePanelButtonHover)
			sndPanel.state = 0;
		if(sndPanel.materialPanelButtonHover)
			sndPanel.state = 1;
	}
    if(glfwGetMouseButton(window, 0) == GLFW_RELEASE){
		brushSizeRangeBarPressed = false;
		brushBlurRangeBarPressed = false;
		subSelectedImagePowerRangeBarPressed = false;
		brushRotationRangeBarPressed = false;
		brushOpacityRangeBarPressed = false;
		brushSpacingRangeBarPressed = false;
		brushBordersRangeBarPressed = false;
		skyboxExposureRangeBarPressed = false;
		skyboxRotationRangeBarPressed = false;
		normalStrengthRangeBarPressed = false;
		noiseStrengthRangeBarPressed = false;
		colorBoxPickerPressed = false;
		colorBoxColorRangeBarPressed = false;
		textureDisplayer.buttonPressed = false;
		textureDisplayer.cornerPressed = false;
        maskPanelSliderPressed = false;
		mainPanelBoundariesPressed = false;
		nodePanelBoundariesPressed = false;
		nodePanel.boundariesPressed = false;
		sndPanel.boundariesPressed = false;
		mirrorXRangeBarPressed = false;
		mirrorYRangeBarPressed = false;
		mirrorZRangeBarPressed = false;
	}
}
bool UiActions::updateRangeValues(GLFWwindow* window, double xOffset,double yOffset, int screenWidth, int screenHeight,TextureDisplayer &textureDisplayer,
SndPanel &sndPanel){
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
	if(mirrorXRangeBarPressed || mirrorYRangeBarPressed || mirrorZRangeBarPressed){
		ligid.mirrorRangeBars(xOffset, screenWidth,screenHeight,mirrorXRangeBarPressed, mirrorYRangeBarPressed, mirrorZRangeBarPressed);
	}
	if (subSelectedImagePowerRangeBarPressed) {
		ligid.subSelectedImagePowerRangeBar(xOffset, screenWidth, screenHeight);//Changes the global variable
	}
	if (skyboxExposureRangeBarPressed) {
		ligid.skyboxExposureRangeBar(xOffset, screenWidth, screenHeight);//Changes the global variable
	}
	if (skyboxRotationRangeBarPressed) {
		ligid.skyBoxRotationRangeBar(xOffset, screenWidth, screenHeight);//Changes the global variable
	}
	if (normalStrengthRangeBarPressed) {
		ligid.normalStrengthRangeBar(xOffset, screenWidth, screenHeight);//Changes the global variable
	}
	if (noiseStrengthRangeBarPressed) {
		ligid.noiseStrengthRangeBar(xOffset, screenWidth, screenHeight);//Changes the global variable
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
		ligid.maskPanelSlider(yOffset,glfwGetVideoMode(glfwGetPrimaryMonitor())->height);//Changes the global variable
	}
	if(mainPanelBoundariesPressed){
		ligid.mainPanelBoundaries(xOffset,screenWidth);
	}
	if(nodePanelBoundariesPressed){
		ligid.nodePanelBoundaries(yOffset,screenHeight);
	}
	if(sndPanel.boundariesPressed){
		ligid.sndPanelBoundaries(xOffset,screenWidth);
	}
    if (colorBoxPickerPressed || colorBoxColorRangeBarPressed || subSelectedImagePowerRangeBarPressed || brushBlurRangeBarPressed || brushSizeRangeBarPressed || brushRotationRangeBarPressed || brushOpacityRangeBarPressed || brushSpacingRangeBarPressed|| brushBordersRangeBarPressed || textureDisplayer.buttonPressed || textureDisplayer.cornerPressed || maskPanelSliderPressed) { //Set cursor as hidden and restrict panel movement if any of the rangebars value is changing
        hideCursor = true;
	}
	else {
        hideCursor = false;
	}

    return hideCursor;
}

void UiActions::sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight){
}
