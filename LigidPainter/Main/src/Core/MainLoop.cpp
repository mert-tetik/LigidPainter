#include <iostream>
#include "Core/LigidPainter.h"

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