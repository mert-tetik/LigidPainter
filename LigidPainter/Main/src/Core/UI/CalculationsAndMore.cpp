#include<iostream>

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/LigidPainter.h"
#include "Core/UI/UserInterface.h"

int CAMuiMaxScreenWidth;
int CAMuiMaxScreenHeight;

bool UserInterface::isMouseOnPanelChangeButton(GLFWwindow* window, float position_x, float position_y, double mouseXpos, double mouseYpos) { //Button with different shape used in order to switch between panels
	std::vector<float> buttonCoor{
		// first triangle
		 0.00f + position_x,  0.02f + position_y, 0.9f,0,0,0,0,0,  // top right
		 0.00f + position_x, -0.05f + position_y, 0.9f,0,0,0,0,0,  // bottom right
		-0.025f + position_x,  0.02f + position_y, 0.9f,0,0,0,0,0,  // top left 
		// second triangle						     
		 0.00f + position_x, -0.05f + position_y, 0.9f,0,0,0,0,0,  // bottom right
		-0.025f + position_x, -0.02f + position_y, 0.9f,0,0,0,0,0,  // bottom left
		-0.025f + position_x,  0.02f + position_y, 0.9f,0,0,0,0,0  // top left
	};
	//glfwGetFramebufferSize();
	int screenSizeX;
	int screenSizeY;
	glfwGetFramebufferSize(window, &screenSizeX, &screenSizeY);

	float mouseFX = ((float)mouseXpos / (CAMuiMaxScreenWidth/2)); //Screen Coord
	float mouseFY = (((float)mouseYpos / (CAMuiMaxScreenHeight / 2)) - 1.0f) * -1.0f; //Screen Coord

	//Barycentric calculations
	for (size_t i = 0; i < 2; i++)
	{
		float ax = buttonCoor[0 + (24 * i)];
		float ay = buttonCoor[1 + (24 * i)];
		float bx = buttonCoor[8 + (24 * i)];
		float by = buttonCoor[9 + (24 * i)];
		float cx = buttonCoor[16 + (24 * i)];
		float cy = buttonCoor[17 + (24 * i)];

		if (cy - ay == 0) {
			cy += 0.0001f;
		}

		float w1 = (ax * (cy - ay) + (mouseFY - ay) * (cx - ax) - mouseFX * (cy - ay)) / ((by - ay) * (cx - ax) - (bx - ax) * (cy - ay));
		float w2 = (mouseFY - ay - w1 * (by - ay)) / (cy - ay);


		if (w1 >= 0 && w2 >= 0 && (w1 + w2) <= 1) {
			return true;
		}
		else if (i == 1) {
			return false;
		}
	}
	//Barycentric calculations
}

bool UserInterface::isMouseOnButton(GLFWwindow* window, float width, float height, float position_x, float position_y,double mouseXpos, double mouseYpos,bool isPanelMoving){ //Return true if mouse hover on the given coordinates
	
	std::vector<float> buttonCoor{
		// first triangle
		 width + position_x,  height + position_y, 1,1,0,0,0,0,  // top right
		 width + position_x, -height + position_y, 1,0,0,0,0,0,  // bottom right
		-width + position_x,  height + position_y, 0,1,0,0,0,0,  // top left 
		// second triangle
		 width + position_x, -height + position_y, 1,0,0,0,0,0,  // bottom right
		-width + position_x, -height + position_y, 0,0,0,0,0,0,  // bottom left
		-width + position_x,  height + position_y, 0,1,0,0,0,0  // top left
	};
	int screenSizeX;
	int screenSizeY;
	glfwGetFramebufferSize(window,&screenSizeX,&screenSizeY);
	float mouseFX;
	if (!isPanelMoving) {
		mouseFX = ((float)mouseXpos / (CAMuiMaxScreenWidth / 2)) - 1.0f;//Screen Coord
	}
	else {
		mouseFX = ((float)mouseXpos / (CAMuiMaxScreenWidth / 2));//Screen Coord

	}
	float mouseFY = (((float)mouseYpos / (CAMuiMaxScreenHeight / 2))-1.0f)*-1.0f;//Screen Coord

	//Barycentric calculations
	for (size_t i = 0; i < 2; i++)
	{
		float ax = buttonCoor[0 + (24*i)];
		float ay = buttonCoor[1 + (24 * i)];
		float bx = buttonCoor[8 + (24 * i)];
		float by = buttonCoor[9 + (24 * i)];
		float cx = buttonCoor[16 + (24 * i)];
		float cy = buttonCoor[17 + (24 * i)];

		if (cy - ay == 0) {
			cy += 0.0001f;
		}

		float w1 = (ax * (cy - ay) + (mouseFY - ay) * (cx - ax) - mouseFX * (cy - ay)) / ((by - ay) * (cx - ax) - (bx - ax) * (cy - ay));
		float w2 = (mouseFY - ay - w1 * (by - ay)) / (cy - ay);
		if (w1 >= 0 && w2 >= 0 && (w1 + w2) <= 1) {
			return true;
		}
		else if (i == 1) {
			return false;
		}
	}
	//Barycentric calculations
}

bool UserInterface::isMouseOnCoords(GLFWwindow*window,double mouseXpos, double mouseYpos,std::vector<float> buttonCoor,bool isPanelMoving){ //Return true if mouse hover on the given coordinates
	int screenSizeX;
	int screenSizeY;
	glfwGetFramebufferSize(window,&screenSizeX,&screenSizeY);

	float mouseFX;
	if (!isPanelMoving) {
		mouseFX = ((float)mouseXpos / (CAMuiMaxScreenWidth / 2)) - 1.0f;//Screen Coord
	}
	else {
		mouseFX = ((float)mouseXpos / (CAMuiMaxScreenWidth / 2));//Screen Coord

	}
	float mouseFY = (((float)mouseYpos / (CAMuiMaxScreenHeight / 2))-1.0f)*-1.0f;//Screen Coord

	//Barycentric calculations
	for (size_t i = 0; i < 2; i++)
	{
		float ax = buttonCoor[0 + (24*i)];
		float ay = buttonCoor[1 + (24 * i)];
		float bx = buttonCoor[8 + (24 * i)];
		float by = buttonCoor[9 + (24 * i)];
		float cx = buttonCoor[16 + (24 * i)];
		float cy = buttonCoor[17 + (24 * i)];

		if (cy - ay == 0) {
			cy += 0.0001f;
		}

		float w1 = (ax * (cy - ay) + (mouseFY - ay) * (cx - ax) - mouseFX * (cy - ay)) / ((by - ay) * (cx - ax) - (bx - ax) * (cy - ay));
		float w2 = (mouseFY - ay - w1 * (by - ay)) / (cy - ay);
		if (w1 >= 0 && w2 >= 0 && (w1 + w2) <= 1) {
			return true;
		}
		else if (i == 1) {
			return false;
		}
	}
	//Barycentric calculations
}
bool UserInterface::isMouseOnNodePanel(GLFWwindow* window,float mainPanelLoc,float height,double mouseXpos,double mouseYpos,bool calculateBoundaries,float sndPanel){
	std::vector<float> buttonCoor;
	if(!calculateBoundaries){
		buttonCoor = std::vector<float>
		{
			//first triangle								    //Color - Normal Vectors Will Be Usen For Color Data Of Vertices
			 sndPanel + 0.037f  			,  -1.00f 			, 0.9f,1.0f,1.0f	,1,1,1,  // top right
			 sndPanel + 0.037f 			,  -1.00f + height 	, 0.9f,1.0f,0.0f	,0,0,0,  // bottom right
			 mainPanelLoc 	-0.037f	,  -1.00f 			, 0.9f,0.0f,1.0f	,0,0,0,  // top left 
			//second triangle
			 sndPanel + 0.037f 		,  -1.00f + height	, 0.9f,1.0f,0.0f	,0,0,0,  // bottom right
			 mainPanelLoc 	-0.037f	,  -1.00f + height	, 0.9f,0.0f,0.0f	,0,0,0,  // bottom left
			 mainPanelLoc 	-0.037f	,  -1.00f 			, 0.9f,0.0f,1.0f	,0,0,0 // top left
		};
	}
	else{
		const float trackingRange = 0.06f;
		
		buttonCoor = std::vector<float>{
			//first triangle								    //Color - Normal Vectors Will Be Usen For Color Data Of Vertices
			 sndPanel + 0.037f 			,  -1.00f + (height+0.056f - trackingRange) 			, 0.9f,1.0f,1.0f	,1,1,1,  // top right
			 sndPanel + 0.037f			,  -1.00f + (height+0.056f + trackingRange) 								, 0.9f,1.0f,0.0f	,0,0,0,  // bottom right
			 mainPanelLoc 	-0.037f	,  -1.00f + (height+0.056f - trackingRange) 			, 0.9f,0.0f,1.0f	,0,0,0,  // top left 
			//second triangle
			 sndPanel + 0.037f			,  -1.00f + (height+0.056f + trackingRange)								, 0.9f,1.0f,0.0f	,0,0,0,  // bottom right
			 mainPanelLoc 	-0.037f	,  -1.00f + (height+0.056f + trackingRange)								, 0.9f,0.0f,0.0f	,0,0,0,  // bottom left
			 mainPanelLoc 	-0.037f	,  -1.00f + (height+0.056f - trackingRange) 			, 0.9f,0.0f,1.0f	,0,0,0 // top left
		};
		
	}

	int screenSizeX;
	int screenSizeY;
	glfwGetFramebufferSize(window,&screenSizeX,&screenSizeY);

	float mouseFX = ((float)mouseXpos / (CAMuiMaxScreenWidth / 2))-1.0f;//Screen Coord

	float mouseFY = (((float)mouseYpos / (CAMuiMaxScreenHeight / 2))-1.0f)*-1.0f;//Screen Coord

	//Barycentric calculations
	for (size_t i = 0; i < 2; i++)
	{
		float ax = buttonCoor[0 + (24*i)];
		float ay = buttonCoor[1 + (24 * i)];
		float bx = buttonCoor[8 + (24 * i)];
		float by = buttonCoor[9 + (24 * i)];
		float cx = buttonCoor[16 + (24 * i)];
		float cy = buttonCoor[17 + (24 * i)];

		if (cy - ay == 0) {
			cy += 0.0001f;
		}

		float w1 = (ax * (cy - ay) + (mouseFY - ay) * (cx - ax) - mouseFX * (cy - ay)) / ((by - ay) * (cx - ax) - (bx - ax) * (cy - ay));
		float w2 = (mouseFY - ay - w1 * (by - ay)) / (cy - ay);
		if (w1 >= 0 && w2 >= 0 && (w1 + w2) <= 1) {
			return true;
		}
		else if (i == 1) {
			return false;
		}
	}
}


void UserInterface::sendMaxWindowSizeToCalculationsAndMore(int maxScreenWidth,int maxScreenHeight){
	CAMuiMaxScreenHeight = maxScreenHeight;
	CAMuiMaxScreenWidth = maxScreenWidth;
}