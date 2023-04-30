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
bool paintOverScaleXRangeBarPressed;
bool paintOverScaleYRangeBarPressed;
bool generateTextSizeRangeBarPressed;
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
			UIElements[UImirrorXRangeBarElement].rangeBar.pressed = true;
		}
		else if (UIElements[UImirrorYRangeBarElement].rangeBar.hover) {
			UIElements[UImirrorYRangeBarElement].rangeBar.pressed = true;
		}
		else if (UIElements[UImirrorZRangeBarElement].rangeBar.hover) {
			UIElements[UImirrorZRangeBarElement].rangeBar.pressed = true;
		}
		else if (UIElements[UIpaintOverPosXRangeBarElement].rangeBar.hover) {
			UIElements[UIpaintOverPosXRangeBarElement].rangeBar.pressed = true;
		}
		else if (UIElements[UIpaintOverPosYRangeBarElement].rangeBar.hover) {
			UIElements[UIpaintOverPosYRangeBarElement].rangeBar.pressed = true;
		}
		else if (UIElements[UIpaintOverScaleXRangeBarElement].rangeBar.hover) {
			paintOverScaleXRangeBarPressed = true;
		}
		else if (UIElements[UIpaintOverScaleYRangeBarElement].rangeBar.hover) {
			paintOverScaleYRangeBarPressed = true;
		}
		else if (UIElements[UIgenerateTextSizeRangeBarElement].rangeBar.hover) {
			generateTextSizeRangeBarPressed = true;
		}
		else if (UIElements[UITDModelSizeRangeBarElement].rangeBar.hover) {
			UIElements[UITDModelSizeRangeBarElement].rangeBar.pressed = true;
		}
		else if (UIElements[UITDModelPosXRangeBarElement].rangeBar.hover) {
			UIElements[UITDModelPosXRangeBarElement].rangeBar.pressed = true;
		}
		else if (UIElements[UITDModelPosYRangeBarElement].rangeBar.hover) {
			UIElements[UITDModelPosYRangeBarElement].rangeBar.pressed = true;
		}
		else if (UIElements[UITDModelPosZRangeBarElement].rangeBar.hover) {
			UIElements[UITDModelPosZRangeBarElement].rangeBar.pressed = true;
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
		if (UIElements[UIaddBrushMaskTextureIcon].icon.hover)
			ligid.addMaskTextureButton();
		if (UIElements[UIselectBrushMaskTextureIcon].icon.hover)
			ligid.selectBrushMaskTexture();
		if (UIElements[UIbakeButtonElement].button.hover)
			ligid.bakeButton();
		if (UIElements[UIoutSubmeshesButtonElement].button.hover)
			ligid.outSubmeshesButton();
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
		
		if (UIElements[UIalbedoChannelCheckBoxElement].checkBox.mouseHover){
			UIElements[UIalbedoChannelCheckBoxElement].checkBox.checked = true; 
			UIElements[UIroughnessChannelCheckBoxElement].checkBox.checked = false; 
			UIElements[UImetallicChannelCheckBoxElement].checkBox.checked = false; 
			UIElements[UInormalChannelCheckBoxElement].checkBox.checked = false; 
		}
		if (UIElements[UIroughnessChannelCheckBoxElement].checkBox.mouseHover){
			UIElements[UIroughnessChannelCheckBoxElement].checkBox.checked = true; 
			UIElements[UIalbedoChannelCheckBoxElement].checkBox.checked = false; 
			UIElements[UImetallicChannelCheckBoxElement].checkBox.checked = false; 
			UIElements[UInormalChannelCheckBoxElement].checkBox.checked = false; 
		}
		if (UIElements[UImetallicChannelCheckBoxElement].checkBox.mouseHover){
			UIElements[UImetallicChannelCheckBoxElement].checkBox.checked = true; 
			UIElements[UIroughnessChannelCheckBoxElement].checkBox.checked = false; 
			UIElements[UIalbedoChannelCheckBoxElement].checkBox.checked = false; 
			UIElements[UInormalChannelCheckBoxElement].checkBox.checked = false; 
		}
		if (UIElements[UInormalChannelCheckBoxElement].checkBox.mouseHover){
			UIElements[UInormalChannelCheckBoxElement].checkBox.checked = true; 
			UIElements[UIroughnessChannelCheckBoxElement].checkBox.checked = false; 
			UIElements[UImetallicChannelCheckBoxElement].checkBox.checked = false; 
			UIElements[UIalbedoChannelCheckBoxElement].checkBox.checked = false; 
		}

		if (UIElements[UIuseUVCheckBox].checkBox.mouseHover)
			ligid.useUVCheckBox();
		if (UIElements[UIbackfaceCullingCheckBox].checkBox.mouseHover)
			ligid.backfaceCullingCheckBox();
		if(UIElements[UIskyboxCheckBox].checkBox.mouseHover)
			UIElements[UIskyboxCheckBox].checkBox.checked = !UIElements[UIskyboxCheckBox].checkBox.checked;
		if(UIElements[UIfocusModeCheckBox].checkBox.mouseHover)
			UIElements[UIfocusModeCheckBox].checkBox.checked = !UIElements[UIfocusModeCheckBox].checkBox.checked;

		if(UIElements[UInormalmapCheckBoxElement].checkBox.mouseHover && !UIElements[UInormalmapCheckBoxElement].checkBox.checked){
			UIElements[UInormalmapCheckBoxElement].checkBox.checked = !UIElements[UInormalmapCheckBoxElement].checkBox.checked;
			UIElements[UInoiseCheckBoxElement].checkBox.checked = !UIElements[UInormalmapCheckBoxElement].checkBox.checked;
			UIElements[UIgenerateTextCheckBoxElement].checkBox.checked = !UIElements[UInormalmapCheckBoxElement].checkBox.checked;
			UIElements[UIgenerateBlackToAlphaCheckBoxElement].checkBox.checked = !UIElements[UInormalmapCheckBoxElement].checkBox.checked;
		}
		if(UIElements[UInoiseCheckBoxElement].checkBox.mouseHover && !UIElements[UInoiseCheckBoxElement].checkBox.checked){
			UIElements[UInoiseCheckBoxElement].checkBox.checked = !UIElements[UInoiseCheckBoxElement].checkBox.checked;
			UIElements[UInormalmapCheckBoxElement].checkBox.checked = !UIElements[UInoiseCheckBoxElement].checkBox.checked;
			UIElements[UIgenerateTextCheckBoxElement].checkBox.checked = !UIElements[UInoiseCheckBoxElement].checkBox.checked;
			UIElements[UIgenerateBlackToAlphaCheckBoxElement].checkBox.checked = !UIElements[UInoiseCheckBoxElement].checkBox.checked;

		}
		if(UIElements[UIgenerateTextCheckBoxElement].checkBox.mouseHover && !UIElements[UIgenerateTextCheckBoxElement].checkBox.checked){
			UIElements[UIgenerateTextCheckBoxElement].checkBox.checked = !UIElements[UIgenerateTextCheckBoxElement].checkBox.checked;
			UIElements[UInormalmapCheckBoxElement].checkBox.checked = !UIElements[UIgenerateTextCheckBoxElement].checkBox.checked;
			UIElements[UInoiseCheckBoxElement].checkBox.checked = !UIElements[UIgenerateTextCheckBoxElement].checkBox.checked;
			UIElements[UIgenerateBlackToAlphaCheckBoxElement].checkBox.checked = !UIElements[UIgenerateTextCheckBoxElement].checkBox.checked;
		}
		if(UIElements[UIgenerateBlackToAlphaCheckBoxElement].checkBox.mouseHover && !UIElements[UIgenerateBlackToAlphaCheckBoxElement].checkBox.checked){
			UIElements[UIgenerateBlackToAlphaCheckBoxElement].checkBox.checked = !UIElements[UIgenerateBlackToAlphaCheckBoxElement].checkBox.checked;
			UIElements[UInormalmapCheckBoxElement].checkBox.checked = !UIElements[UIgenerateBlackToAlphaCheckBoxElement].checkBox.checked;
			UIElements[UInoiseCheckBoxElement].checkBox.checked = !UIElements[UIgenerateBlackToAlphaCheckBoxElement].checkBox.checked;
			UIElements[UIgenerateTextCheckBoxElement].checkBox.checked = !UIElements[UIgenerateBlackToAlphaCheckBoxElement].checkBox.checked;
		}
		
		if(UIElements[UIrealtimeMaterialRenderingCheckBox].checkBox.mouseHover)
			UIElements[UIrealtimeMaterialRenderingCheckBox].checkBox.checked = !UIElements[UIrealtimeMaterialRenderingCheckBox].checkBox.checked;
			
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
		if (UIElements[UIgenerateTextTextureTextTextBoxElement].textBox.hover){
			UIElements[UIgenerateTextTextureTextTextBoxElement].textBox.clicked = !UIElements[UIgenerateTextTextureTextTextBoxElement].textBox.clicked; 
		}
		if (UIElements[UIpaintThroughCheckBoxElement].checkBox.mouseHover)
			UIElements[UIpaintThroughCheckBoxElement].checkBox.checked = !UIElements[UIpaintThroughCheckBoxElement].checkBox.checked; 
		
		if (UIElements[UIpaintOverCheckBoxElement].checkBox.mouseHover)
			UIElements[UIpaintOverCheckBoxElement].checkBox.checked = !UIElements[UIpaintOverCheckBoxElement].checkBox.checked; 
		if (UIElements[UIdisplayPaintOverTextureCheckBoxElement].checkBox.mouseHover)
			UIElements[UIdisplayPaintOverTextureCheckBoxElement].checkBox.checked = !UIElements[UIdisplayPaintOverTextureCheckBoxElement].checkBox.checked; 
		if (UIElements[UIselectPaintOverTextureIconElement].icon.hover)
			ligid.selectingPaintOverTextureIcon();
		
		if (UIElements[UIdynamicPaintingCheckBoxElement].checkBox.mouseHover){
			UIElements[UIdynamicPaintingCheckBoxElement].checkBox.checked = true; 
			UIElements[UImaskPaintingCheckBoxElement].checkBox.checked = false; 
			UIElements[UIcolorPaintingCheckBoxElement].checkBox.checked = false; 
			UIElements[UInormalmapPaintingCheckBoxElement].checkBox.checked = false; 
			UIElements[UIbrushBlurText].text.text = "Hardness";
			UIElements[UIbrushSizeText].text.text = "Radius";
		}
		if (UIElements[UImaskPaintingCheckBoxElement].checkBox.mouseHover){
			UIElements[UImaskPaintingCheckBoxElement].checkBox.checked = true; 
			UIElements[UIcolorPaintingCheckBoxElement].checkBox.checked = false; 
			UIElements[UInormalmapPaintingCheckBoxElement].checkBox.checked = false; 
			UIElements[UIdynamicPaintingCheckBoxElement].checkBox.checked = false; 
			UIElements[UIbrushBlurText].text.text = "Blur";
			UIElements[UIbrushSizeText].text.text = "Size";
		}
		if (UIElements[UIcolorPaintingCheckBoxElement].checkBox.mouseHover){
			UIElements[UIcolorPaintingCheckBoxElement].checkBox.checked = true; 
			UIElements[UImaskPaintingCheckBoxElement].checkBox.checked = false; 
			UIElements[UInormalmapPaintingCheckBoxElement].checkBox.checked = false; 
			UIElements[UIdynamicPaintingCheckBoxElement].checkBox.checked = false; 
			UIElements[UIbrushBlurText].text.text = "Blur";
			UIElements[UIbrushSizeText].text.text = "Size";
		}
		if (UIElements[UInormalmapPaintingCheckBoxElement].checkBox.mouseHover){
			UIElements[UInormalmapPaintingCheckBoxElement].checkBox.checked = true; 
			UIElements[UIcolorPaintingCheckBoxElement].checkBox.checked = false; 
			UIElements[UImaskPaintingCheckBoxElement].checkBox.checked = false; 
			UIElements[UIdynamicPaintingCheckBoxElement].checkBox.checked = false; 
			UIElements[UIbrushBlurText].text.text = "Blur";
			UIElements[UIbrushSizeText].text.text = "Size";
		}

		if (UIElements[UIgenerateTextLeftAlignCheckBoxElement].checkBox.mouseHover){
			UIElements[UIgenerateTextLeftAlignCheckBoxElement].checkBox.checked = true; 
			UIElements[UIgenerateTextMidAlignCheckBoxElement].checkBox.checked = false; 
			UIElements[UIgenerateTextRightAlignCheckBoxElement].checkBox.checked = false; 
		}
		if (UIElements[UIgenerateTextMidAlignCheckBoxElement].checkBox.mouseHover){
			UIElements[UIgenerateTextMidAlignCheckBoxElement].checkBox.checked = true; 
			UIElements[UIgenerateTextLeftAlignCheckBoxElement].checkBox.checked = false; 
			UIElements[UIgenerateTextRightAlignCheckBoxElement].checkBox.checked = false; 
		}
		if (UIElements[UIgenerateTextRightAlignCheckBoxElement].checkBox.mouseHover){
			UIElements[UIgenerateTextRightAlignCheckBoxElement].checkBox.checked = true; 
			UIElements[UIgenerateTextMidAlignCheckBoxElement].checkBox.checked = false; 
			UIElements[UIgenerateTextLeftAlignCheckBoxElement].checkBox.checked = false; 
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
		UIElements[UImirrorXRangeBarElement].rangeBar.pressed = false;
		UIElements[UImirrorYRangeBarElement].rangeBar.pressed = false;
		UIElements[UImirrorZRangeBarElement].rangeBar.pressed = false;
		UIElements[UIpaintOverPosXRangeBarElement].rangeBar.pressed = false;
		UIElements[UIpaintOverPosYRangeBarElement].rangeBar.pressed = false;
		paintOverScaleXRangeBarPressed = false;
		paintOverScaleYRangeBarPressed = false;
		generateTextSizeRangeBarPressed = false;
		UIElements[UITDModelSizeRangeBarElement].rangeBar.pressed = false;
		UIElements[UITDModelPosXRangeBarElement].rangeBar.pressed = false;
		UIElements[UITDModelPosYRangeBarElement].rangeBar.pressed = false;
		UIElements[UITDModelPosZRangeBarElement].rangeBar.pressed = false;
	}
}
bool UiActions::updateRangeValues(GLFWwindow* window, double xOffset,double yOffset, int screenWidth, int screenHeight,TextureDisplayer &textureDisplayer,
SndPanel &sndPanel,std::vector<UIElement> &UIElements){
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
	if(UIElements[UImirrorXRangeBarElement].rangeBar.pressed || UIElements[UImirrorYRangeBarElement].rangeBar.pressed || UIElements[UImirrorZRangeBarElement].rangeBar.pressed){
		ligid.mirrorRangeBars(xOffset, screenWidth,screenHeight,UIElements[UImirrorXRangeBarElement].rangeBar.pressed, UIElements[UImirrorYRangeBarElement].rangeBar.pressed, UIElements[UImirrorZRangeBarElement].rangeBar.pressed);
	}
	if(UIElements[UIpaintOverPosXRangeBarElement].rangeBar.pressed || UIElements[UIpaintOverPosYRangeBarElement].rangeBar.pressed || paintOverScaleXRangeBarPressed || paintOverScaleYRangeBarPressed){
		ligid.paintoverTransformRanegBars(xOffset, screenWidth,screenHeight,UIElements[UIpaintOverPosXRangeBarElement].rangeBar.pressed , UIElements[UIpaintOverPosYRangeBarElement].rangeBar.pressed , paintOverScaleXRangeBarPressed , paintOverScaleYRangeBarPressed);
	}
	if(generateTextSizeRangeBarPressed){
		ligid.generateTextSizeRangeBar(xOffset,screenWidth);
	}
	if(UIElements[UITDModelSizeRangeBarElement].rangeBar.pressed){
		ligid.TDModelSizeRangeBar(xOffset,screenWidth,1,0,0,0);
	}
	if(UIElements[UITDModelPosXRangeBarElement].rangeBar.pressed){
		ligid.TDModelSizeRangeBar(xOffset,screenWidth,0,1,0,0);
	}
	if(UIElements[UITDModelPosYRangeBarElement].rangeBar.pressed){
		ligid.TDModelSizeRangeBar(xOffset,screenWidth,0,0,1,0);
	}
	if(UIElements[UITDModelPosZRangeBarElement].rangeBar.pressed){
		ligid.TDModelSizeRangeBar(xOffset,screenWidth,0,0,0,1);
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
