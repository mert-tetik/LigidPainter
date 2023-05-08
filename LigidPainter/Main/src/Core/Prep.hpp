#ifndef LGDPREP_H
#define LGDPREP_H

#include "../../thirdparty/include/glad/glad.h"
#include "../../thirdparty/include/GLFW/glfw3.h"

#include "gl.h"
#include "Load.hpp"

#include "LibAL.h"


void prepGLFW(GLFWwindow* window,GLFWmonitorfun monitor_callback,GLFWwindowsizefun framebuffer_size_callback,GLFWkeyfun key_callback,GLFWcursorposfun mouse_callback,
                GLFWscrollfun scroll_callback,GLFWjoystickfun joystick_callback,GLFWmousebuttonfun mouse_button_callback,GLFWdropfun drop_callback){
    	//Set Callbacks
	glfwSetMonitorCallback(monitor_callback); 
	glfwSetWindowSizeCallback(window, framebuffer_size_callback); 
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetJoystickCallback(joystick_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetDropCallback(window,drop_callback);
}

void prepVertexBuffers(unsigned int &sqrVBO,unsigned int &sqrVAO,unsigned int &VBO,unsigned int &VAO){
    //Generate a vertex object for rendering
    glGenBuffers(1, &sqrVBO);
	glBindBuffer(GL_ARRAY_BUFFER, sqrVBO);
	glGenVertexArrays(1,&sqrVAO);
	glBindVertexArray(sqrVAO);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	
	std::vector<float> sqrCoor{
		// first triangle
		 1,  1., 0,	1,1,	0,0,0,  // top right
		 1, -1., 0,	1,0,	0,0,0,  // bottom right
		-1,  1., 0,	0,1,	0,0,0,  // top left 
		 		
		 1, -1., 0,	1,0,	0,0,0,  // bottom right
		-1, -1., 0,	0,0,	0,0,0,  // bottom left
		-1,  1., 0,	0,1,	0,0,0  // top left
	};

	glBufferData(GL_ARRAY_BUFFER,sqrCoor.size() * sizeof(float), &sqrCoor[0],GL_DYNAMIC_DRAW);



	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glGenVertexArrays(1,&VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBufferData(GL_ARRAY_BUFFER, 10000, NULL, GL_DYNAMIC_DRAW); 
}
void prepLibAL(unsigned int &MessageBox,unsigned int &Login,unsigned int &ButtonEnter,unsigned int &Alert){
    if(!LibAL_start())
		std::cout << "ERROR : Initializing libal\n";
	
	LibAL_genAudio(MessageBox);
	LibAL_genAudio(Login);
	LibAL_genAudio(ButtonEnter);
	LibAL_genAudio(Alert);

	if(!LibAL_modifyAudioViaPath("LigidPainter/Resources/Sounds/MessageBox.wav","wav",MessageBox)){
		std::cout << "ERROR : Modifying audio object MessageBox.wav  " << '\n';
	}
	else{
		std::cout << "Loaded MessageBox.wav  " << '\n';
	}
	if(!LibAL_modifyAudioViaPath("LigidPainter/Resources/Sounds/Login.wav","wav",Login)){
		std::cout << "ERROR : Modifying audio object Login.wav  " << '\n';
	}
	else{
		std::cout << "Loaded Login.wav  " << '\n';
	}
	if(!LibAL_modifyAudioViaPath("LigidPainter/Resources/Sounds/ButtonEnter.wav","wav",ButtonEnter)){
		std::cout << "ERROR : Modifying audio object ButtonEnter.wav  " << '\n';
	}
	else{
		std::cout << "Loaded ButtonEnter.wav  " << '\n';
	}
	if(!LibAL_modifyAudioViaPath("LigidPainter/Resources/Sounds/Alert.wav","wav",Alert)){
		std::cout << "ERROR : Modifying audio object Alert.wav  " << '\n';
	}
	else{
		std::cout << "Loaded Alert.wav  " << '\n';
	}
}

void prepStartLoadingScreen(GLFWwindow* window,unsigned int renderTheTextureProgram,unsigned int &viewportBGImage){
    Texture txtr;
    GlSet glset;

    glActiveTexture(GL_TEXTURE0);
	glset.genTextures(viewportBGImage);
	glset.bindTexture(viewportBGImage);
	txtr.getTexture("LigidPainter/Resources/Images/BGImage.jpg",1920,1080,true);
	glActiveTexture(GL_TEXTURE28);

	//Loading screen
	std::vector<float> renderVertices = { 
		// first triangle
		 1.0f,  1.0f, 0.0f,1,1,0,0,0,  // top right
		 1.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
		 0.0f,  1.0f, 0.0f,0,1,0,0,0,  // top left 
		// second triangle	  ,0,0,0,
		 1.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
		 0.0f,  0.0f, 0.0f,0,0,0,0,0,  // bottom left
		 0.0f,  1.0f, 0.0f,0,1,0,0,0   // top left
	};
	glUseProgram(renderTheTextureProgram);
	glm::mat4 txtprojection = glm::ortho(0,1,0,1);
	glset.uniformMatrix4fv(renderTheTextureProgram,"TextProjection",txtprojection);
	glset.uniform1i(renderTheTextureProgram,"txtr",0);
	glset.drawArrays(renderVertices,0);
	glfwSwapBuffers(window);
}
void prepLoadStuff(Cubemaps &cubemaps,Icons &icons,LigidCursors &cursors,InitializedTextures &textures,std::vector<UIElement> &UIElements,unsigned int &FBOScreen,Programs &programs,std::string &maskTexturePath){
    Load load;
    UserInterface ui;

	programs = load.getProgram();
	//Load chars
	load.uploadChars();
	//Load brush mask textures
	//Load cubemaps both blury and not blury
	cubemaps = load.loadCubemaps();
	//Load icons
	icons = load.loadIcons();
	//Load cursors
	cursors = load.loadCursors();
	//Create textures
	textures = load.initTextures(maskTexturePath.c_str());
	//Load UI
	UIElements = ui.getUiElements(icons);
	//Load context menus
	//addNodeContextMenu = ui.createContextMenus(appNodes);
	//Load the prefilter map
	cubemaps.prefiltered = load.createPrefilterMap(programs,cubemaps,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height);
	//Load general rendering FBO
	FBOScreen = load.createScreenFrameBufferObject(glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height);
}
void prepPrograms(Programs programs,std::vector<UIElement> &UIElements,glm::mat4 &modelMatrix){
    ColorData colorData;
    GlSet glset;

    glUseProgram(programs.brushCursor);
	glset.uniform1i(programs.brushCursor, "modifiedMaskTexture", 12);
	glUseProgram(programs.iconsProgram);
	glset.uniform1i(programs.iconsProgram, "icon", 6);
	glUseProgram(programs.uiProgram);
	glm::vec3 textClr = glm::vec3(colorData.textColor);
	glset.uniform3fv(programs.uiProgram,"textColor",textClr);
	glset.uniform1i(programs.uiProgram, "text", 2);
	glset.uniform1i(programs.uiProgram, "modifiedMaskTexture", 12);

    glUseProgram(programs.dynamicPaintingProgram);
	glset.uniform1f(programs.dynamicPaintingProgram,"radius",(UIElements[UIbrushSizeRangeBar].rangeBar.value+0.11f)*300);
	glset.uniform1f(programs.dynamicPaintingProgram,"hardness",(UIElements[UIbrushBlurRangeBar].rangeBar.value-0.09f)*4.5454545*50);
	glset.uniform1f(programs.dynamicPaintingProgram,"opacity",(UIElements[UIbrushOpacityRangeBar].rangeBar.value + 0.11)*4.5454545);

    glUseProgram(programs.outProgram);
	glset.uniform1i(programs.outProgram,"maskMode",1);

	modelMatrix = glm::mat4(1);
	modelMatrix = glm::scale(modelMatrix,glm::vec3((UIElements[UITDModelSizeRangeBarElement].rangeBar.value+0.11f)*4.54545454545));
	glUseProgram(programs.screenDepthProgram);
	glset.uniformMatrix4fv(programs.screenDepthProgram,"modelMatrix",modelMatrix);
	glUseProgram(programs.outProgram);
	glset.uniformMatrix4fv(programs.outProgram,"modelMatrix",modelMatrix);
	glUseProgram(programs.PBRProgram);
	glset.uniformMatrix4fv(programs.PBRProgram,"modelMatrix",modelMatrix);
}
#endif