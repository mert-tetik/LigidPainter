#include<iostream>
#include "../../thirdparty/include/glad/glad.h"
#include "../../thirdparty/include/GLFW/glfw3.h"
#include <string>
#include <fstream>
#include <sstream>
#include "../../thirdparty/include/glm/glm.hpp"
#include "../../thirdparty/include/glm/gtc/matrix_transform.hpp"
#include "../../thirdparty/include/glm/gtc/type_ptr.hpp"
#include "../../thirdparty/include/glm/gtx/string_cast.hpp"
#include "Core/LigidPainter.h"
#include "Core/ModelRead.h"
#include "UserInterface.h"
#include "Utilities.h"
#include "Callback.h"
#include "gl.h"
#include "Render.h"
#include "UiActions.h"
#include <vector>
#include <map>
#include "stb_image.h"
#include "stb_image_write.h"
#include "tinyfiledialogs.h"

bool noButtonClick;

bool clickTaken = false;
void UiActions::isFirstClickDoneInside(GLFWwindow* window ,CallbckData callbackData,bool textureDemonstratorBoundariesHover) {
	if (glfwGetMouseButton(window, 0) == GLFW_PRESS && !clickTaken) {
		if (!callbackData.addImageButtonEnter && !callbackData.addMaskTextureButtonEnter && !callbackData.addPlaneButtonEnter
			//Check all the buttons
			&& !callbackData.addSphereButtonEnter && !callbackData.autoTriangulateCheckBoxEnter && !callbackData.backfaceCullingCheckBoxEnter
			&& !callbackData.brushSizeRangeBarEnter && !callbackData.loadModelButtonEnter && !callbackData.modelFilePathTextBoxEnter
			&& !callbackData.modelPanelButtonEnter && !callbackData.paintingPanelButtonEnter && !callbackData.exportPanelButtonEnter && !callbackData.texturePanelButtonEnter && !callbackData.colorBoxPickerEnter
			&& !callbackData.colorBoxColorRangeBarEnter && !callbackData.exportPathTextBoxEnter && !callbackData.exportDownloadButtonEnter && !callbackData.exportExtJPGCheckBoxEnter && !callbackData.exportExtPNGCheckBoxEnter
			&& !callbackData.brushBlurRangeBarEnter && !callbackData.brushRotationRangeBarEnter && !callbackData.brushOpacityRangeBarEnter && !callbackData.brushSpacingRangeBarEnter
			&& !callbackData.mirrorXCheckBoxEnter && !callbackData.mirrorYCheckBoxEnter && !callbackData.mirrorZCheckBoxEnter && !callbackData.textureDemonstratorButtonEnter && !textureDemonstratorBoundariesHover && !callbackData.useNegativeForDrawingCheckboxEnter
			&& !callbackData.paintingDropperEnter && !callbackData.paintingFillNumericModifierPEnter && !callbackData.paintingFillNumericModifierNEnter) {
			noButtonClick = true;
		}
		else {
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
bool colorBoxColorRangeBarPressed;
bool colorBoxPickerPressed;
bool textureDemonstratorButtonPressed;
bool textureDemonstratorBoundariesPressed;

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
				if (callbackData.brushBlurRangeBarEnter) {
					brushBlurRangeBarPressed = true;
				}
				if (callbackData.brushRotationRangeBarEnter) {
					brushRotationRangeBarPressed = true;
				}
				if (callbackData.brushOpacityRangeBarEnter) {
					brushOpacityRangeBarPressed = true;
				}
				if (callbackData.brushSpacingRangeBarEnter) {
					brushSpacingRangeBarPressed = true;
				}
				if (callbackData.colorBoxPickerEnter) {
					colorBoxPickerPressed = true;
				}
				if (callbackData.colorBoxColorRangeBarEnter) {
					colorBoxColorRangeBarPressed= true;
				}
				if (callbackData.textureDemonstratorButtonEnter) {
					textureDemonstratorButtonPressed = true;
				}
				if (textureDemonstratorBoundariesHover) {
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
				if (callbackData.addImageButtonEnter)
					ligid.addImageButton();
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
		colorBoxPickerPressed = false;
		colorBoxColorRangeBarPressed = false;
		textureDemonstratorButtonPressed = false;
		textureDemonstratorBoundariesPressed = false;
        
	}
    UiActionsData uiData;
    uiData.textureDemonstratorBoundariesPressed =textureDemonstratorBoundariesPressed;
    uiData.textureDemonstratorButtonPressed = textureDemonstratorButtonPressed;
    return uiData;
}
bool UiActions::updateRangeValues(GLFWwindow* window, float xOffset,float yOffset,unsigned int screenWidth,unsigned int screenHeight){
    bool hideCursor;
    LigidPainter ligid;

    

    
    if (brushSizeRangeBarPressed) {
		ligid.brushSizeRangeBar(xOffset,screenWidth);//Changes the global variable
	}
	if (brushBlurRangeBarPressed) {
		ligid.brushBlurRangeBar(xOffset,screenWidth,screenHeight);//Changes the global variable
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
    if (colorBoxPickerPressed || colorBoxColorRangeBarPressed || brushBlurRangeBarPressed || brushSizeRangeBarPressed || brushRotationRangeBarPressed || brushOpacityRangeBarPressed || brushSpacingRangeBarPressed || textureDemonstratorButtonPressed || textureDemonstratorBoundariesPressed) { //Set cursor as hidden and restrict panel movement if any of the rangebars value is changing
        hideCursor = true;
	}
	else {
        hideCursor = false;
	}

    return hideCursor;
}