#include <iostream>

#include "Core/LigidPainter.h"
#include "Core/UI/UserInterface.h"
#include "Core/Utilities.h"
#include "Core/Render/Render.h"

//If firstClick && buttonHover = buttonClicked
void MainLoop::detectClick(GLFWwindow* window,bool &mousePress,bool &firstClick){
    if(glfwGetMouseButton(window, 0) == GLFW_PRESS){
		if(!mousePress){
			firstClick = true;
		}
		else{
			firstClick = false;
		}
			mousePress = true;
	}
	if(glfwGetMouseButton(window, 0) == GLFW_RELEASE){
		firstClick = false;
		mousePress = false;
	}
}

glm::vec3 holdCameraPos; //Used to detect the camera position change
void MainLoop::updateCameraPosChanging(glm::vec3 cameraPos,bool &cameraPosChanging){
	if (cameraPos != holdCameraPos) {
		cameraPosChanging = true;
	}
	holdCameraPos = cameraPos;
}

void MainLoop::updateRenderTheScene(GLFWwindow* window,int &renderTheSceneCounter,bool &renderTheScene){
	const bool mouseInputTaken = glfwGetMouseButton(window, 0) == GLFW_PRESS || glfwGetMouseButton(window, 1) == GLFW_PRESS || glfwGetMouseButton(window, 2) == GLFW_PRESS;   
	if(mouseInputTaken){
		renderTheScene = true;
		renderTheSceneCounter = 0;
	}
}

void MainLoop::setContextPanelsStates(GLFWwindow* window,ColoringPanel &coloringPanel, ContextMenu &addNodeContextMenu, TextureSelectionPanel &textureSelectionPanel){
	if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
		coloringPanel.active = false;
		textureSelectionPanel.active = false;
	}
}

void MainLoop::releaseTextBoxes(GLFWwindow* window,std::vector<UIElement> &UIElements,std::string &exportFileName,int& textBoxActiveChar,ColoringPanel &coloringPanel,TextureCreatingPanel& txtrCreatingPanel,ColorPicker &colorPicker){
	const bool deactivatingTextBoxesCondition = glfwGetMouseButton(window, 0) == GLFW_PRESS || glfwGetMouseButton(window, 1) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS; 
	if (deactivatingTextBoxesCondition){
		if(!UIElements[UIexportingFolderNameTextBox].textBox.hover && UIElements[UIexportingFolderNameTextBox].textBox.clicked){
			UIElements[UIexportingFolderNameTextBox].textBox.clicked = false; 
			if(exportFileName == ""){
				exportFileName = "LP_Export";
				UIElements[UIexportingFolderNameTextBox].textBox.text = exportFileName;
			}	
			textBoxActiveChar = 0;
		}
		if(!colorPicker.hexValTextBoxEnter){
			colorPicker.hexValTextBoxActive = false;
			textBoxActiveChar = 0;
		}
		if(!coloringPanel.hexValTextboxHover){
			coloringPanel.hexValTextboxActive = false;
			textBoxActiveChar = 0;
		}
		if(!txtrCreatingPanel.textBoxHover){
			txtrCreatingPanel.textBoxActive = false;
			textBoxActiveChar = 0;
		}
	}
}

void MainLoop::changeTextureDisplayersState(GLFWwindow* window,TextureDisplayer &textureDisplayer){
	if(textureDisplayer.buttonPressed){
		textureDisplayer.buttonPressedCounter++;
		textureDisplayer.buttonPressed = true;
	}
	const bool textureDisplayerClickedCondition = textureDisplayer.buttonPressedCounter < 20 && textureDisplayer.buttonPressed && glfwGetMouseButton(window, 0) == GLFW_RELEASE && !textureDisplayer.positionChanged; 
	if(textureDisplayerClickedCondition){
		textureDisplayer.buttonClicked = true;
		textureDisplayer.buttonPressed = false;
	}
	if(glfwGetMouseButton(window, 0) == GLFW_RELEASE){
		textureDisplayer.buttonPressedCounter = 0;
		textureDisplayer.buttonPressed = false;
		textureDisplayer.positionChanged = false;
	}
}

void MainLoop::changeColorPickersValue(GLFWwindow* window,ColorPicker& colorPicker,ColoringPanel& coloringPanel,glm::vec3 mouseHoverPixel,bool &firstClick){
	LigidPainter lp;
	Utilities util;
	const bool dropperColorPickingCondition = colorPicker.dropperActive && firstClick;
	const bool movingSatValPointerCondition = colorPicker.saturationValueBoxClicked && !colorPicker.saturationValuePointerHover;
	const bool movingHuePointerCondition = colorPicker.hueBarClicked && !colorPicker.saturationValuePointerHover && !colorPicker.huePointerChanging; 
	if(dropperColorPickingCondition || movingSatValPointerCondition || movingHuePointerCondition || colorPicker.hexValTextBoxGotInput){
		if(colorPicker.hexValTextBoxGotInput){
			lp.updateColorPicker(util.hexToRGBConverter(colorPicker.hexValTextBoxVal),true,true,colorPicker.hueValue,colorPicker.saturationValuePosX,colorPicker.saturationValuePosY,true);//Update colorbox val once color picker hex value textbox value changed
		}
		else{
			if(coloringPanel.dropperActive)
				lp.updateColorPicker(mouseHoverPixel,true,true,coloringPanel.hueBarPosX,coloringPanel.saturationValueBoxPosX,coloringPanel.saturationValueBoxPosY,false);//Update colorbox val once dropper is used or colorbox is clicked
			else
				lp.updateColorPicker(mouseHoverPixel,true,!colorPicker.hueBarClicked,colorPicker.hueValue,colorPicker.saturationValuePosX,colorPicker.saturationValuePosY,true);//Update colorbox val once dropper is used or colorbox is clicked
			
			colorPicker.dropperActive = false;
			coloringPanel.dropperActive = false;
			colorPicker.dropperEnter = false;
		}
	}
	if(glfwGetMouseButton(window, 0) == GLFW_RELEASE)
		colorPicker.huePointerChanging = false;

	colorPicker.saturationValueBoxClicked = false;
	colorPicker.hueBarClicked = false;
	colorPicker.saturationValuePointerChanging = false;
}