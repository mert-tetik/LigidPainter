#include<iostream>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <filesystem>

#include "Core/LigidPainter.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/Model/model.h"

#include "Core/Render/Render.h"
#include "Core/UI/UserInterface.h"
#include "Core/Utilities.h"
#include "Core/Renderer.hpp"
#include "Core/gl.h"
#include "Core/Texture/Texture.h"
#include "Core/ProjectFile/WRLigidFile.hpp"
#include "Core/ProjectFile/WRLigidMaterialFile.hpp"
#include "Core/ProjectFile/WRLigidFolder.hpp"


#include "stb_image.h"
#include "stb_image_write.h"

#include "LibAL.h"

#include "tinyfiledialogs.h"

bool createProjectMode = true;

void Render::startScreenPanel(std::vector<UIElement> &UIElements,Programs renderPrograms,Cubemaps cubemaps,SkyBoxShaderData skyBoxShaderData,
                                        float &createProjectPanelBlurVal,std::string &projectPath,double screenGapX,GLFWwindow* window,Icons icons,double mouseXpos,double mouseYpos,
                                        bool firstClick,bool &displayProjectFolderManager,std::vector<Font> &fonts,ProjectManager &projectManager,std::vector<aTexture> &albedoTextures
                                        ,int txtrRes,std::vector<NodeScene> &materials, std::vector<Node> &appNodes, ContextMenu &addNodeContexMenu, Model &model,bool firstClickR,Renderer &renderer){
        glDisable(GL_DEPTH_TEST);
        Utilities util;
        UserInterface ui;
        ColorData colorData;
        ColorData2 colorData2;
        GlSet gls;

	    glm::mat4 view = glm::lookAt(glm::vec3(-5,0,0), glm::vec3(0), glm::vec3(0.0, 1.0, 0.0)); 
        skyBoxShaderData.view = view;
		gls.blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		
        glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
		glUseProgram(renderPrograms.iconsProgram);
		gls.uniformMatrix4fv(renderPrograms.iconsProgram, "Projection", projection);
		gls.uniform1i(renderPrograms.iconsProgram, "icon", 6);
		glUseProgram(renderPrograms.uiProgram);
		gls.uniformMatrix4fv(renderPrograms.uiProgram, "TextProjection", projection);
		glUseProgram(renderPrograms.directionalShadow);
		gls.uniformMatrix4fv(renderPrograms.directionalShadow, "TextProjection", projection);
		glUseProgram(renderPrograms.textureDisplayer);
		gls.uniform1i(renderPrograms.textureDisplayer, "currentTexture", 14);
		gls.uniform1i(renderPrograms.textureDisplayer, "roundCor",1);
		gls.uniformMatrix4fv(renderPrograms.textureDisplayer, "TextProjection", projection);
		glUseProgram(renderPrograms.renderTheTextureProgram);
		gls.uniformMatrix4fv(renderPrograms.renderTheTextureProgram, "TextProjection", projection);
		gls.uniform1i(renderPrograms.renderTheTextureProgram, "txtr", 14);
		gls.uniform1i(renderPrograms.renderTheTextureProgram, "isHover", 0);
		gls.uniform1i(renderPrograms.renderTheTextureProgram, "isPressed", 0);
		gls.uniform1i(renderPrograms.renderTheTextureProgram, "subSelected", 0);
		gls.uniform1i(renderPrograms.renderTheTextureProgram, "isMask", 0);
		gls.uniform1i(renderPrograms.renderTheTextureProgram, "maskUseColor", 0);
		gls.uniform1i(renderPrograms.renderTheTextureProgram, "roundCorners", 0);
		gls.uniform1i(renderPrograms.renderTheTextureProgram, "renderMaterials", 0);
		glUseProgram(renderPrograms.renderTheTextureBlur);
		gls.uniformMatrix4fv(renderPrograms.renderTheTextureBlur, "TextProjection", projection);
		glUseProgram(renderPrograms.uiProgram);

        glfwSetWindowAttrib(window,GLFW_DECORATED,GLFW_TRUE);
	    glfwSetWindowSize(window,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);
        glViewport(0,
                   0,
                    glfwGetVideoMode(glfwGetPrimaryMonitor())->height,
                    glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);
        
        glDisable(GL_DEPTH_TEST);
        
        //Back color
        ui.box(10,10,0,0,"",glm::vec4(0.98f,0.98f,0.98f,1),0,0,0,0.9f,10000,glm::vec4(1),0.f);
        //Left panel
        ui.box(0.2f,1.f,-1.f + 0.2f ,0,"",glm::vec4(1),0,0,0,0.9f,10000,glm::vec4(1),0.f);

		glUseProgram(renderPrograms.directionalShadow);
		ui.renderTheProgram(-1.0 + 0.4f + 0.9f,0.0f,0.9f,1.f);
        
		//glEnable(GL_DEPTH_TEST);
		
		
		glUseProgram(renderPrograms.uiProgram);
        renderer.startScreenNewProjectButton.draw(glm::vec3(-1.f + 0.2f,0.7f,0.92f),glm::vec2(mouseXpos,mouseYpos));
        renderer.startScreenLoadProjectButton.draw(glm::vec3(-1.f + 0.2f,0.58f,0.92f),glm::vec2(mouseXpos,mouseYpos));
			


		if(createProjectMode){
			ui.renderText(renderPrograms.uiProgram,"Create new 3D project",-0.55f,0.8f,0.0006f,glm::vec4(0.06,0.12,0.15,1.0),0.91f,false);
			
			//1 Project Title
			glUseProgram(renderPrograms.iconsProgram);
			ui.iconBox(0.05f/1.5f,0.05f,-0.54f,0.715f,0.9f,icons.Circle,0.f,glm::vec4(0.8,0.8,0.8,1.0),glm::vec4(0.06,0.12,0.15,1.0));
			glUseProgram(renderPrograms.uiProgram);
			
			ui.renderText(renderPrograms.uiProgram,"1",-0.55f,0.7f,0.0004f,glm::vec4(0.95,0.95,0.95,1.0),0.91f,false);
			ui.renderText(renderPrograms.uiProgram,"Project Title",-0.47f,0.7f,0.0004f,glm::vec4(0.06,0.12,0.15,1.0),0.91f,false);
			
			renderer.startScreenProjectTitleTextBox.draw(glm::vec3(-0.47f + renderer.startScreenProjectTitleTextBox.width,0.615f,0.9f),glm::vec2(mouseXpos,mouseYpos),firstClick);
			
			
			//2 Project Path
			glUseProgram(renderPrograms.iconsProgram);
			ui.iconBox(0.05f/1.5f,0.05f,-0.54f,0.415f,0.9f,icons.Circle,0.f,glm::vec4(0.8,0.8,0.8,1.0),glm::vec4(0.06,0.12,0.15,1.0));
			glUseProgram(renderPrograms.uiProgram);
			
			ui.renderText(renderPrograms.uiProgram,"2",-0.55f,0.4f,0.0004f,glm::vec4(0.95,0.95,0.95,1.0),0.91f,false);
			ui.renderText(renderPrograms.uiProgram,"Project Path",-0.47f,0.4f,0.0004f,glm::vec4(0.06,0.12,0.15,1.0),0.91f,false);

			renderer.startScreenProjectPathTextBox.draw(glm::vec3(-0.47f + renderer.startScreenProjectTitleTextBox.width,0.315f,0.9f),glm::vec2(mouseXpos,mouseYpos),firstClick);
			
			
			//3 Texture Resolution
			glUseProgram(renderPrograms.iconsProgram);
			ui.iconBox(0.05f/1.5f,0.05f,-0.54f,0.115f,0.9f,icons.Circle,0.f,glm::vec4(0.8,0.8,0.8,1.0),glm::vec4(0.06,0.12,0.15,1.0));
			glUseProgram(renderPrograms.uiProgram);
			
			ui.renderText(renderPrograms.uiProgram,"3",-0.55f,0.1f,0.0004f,glm::vec4(0.95,0.95,0.95,1.0),0.91f,false);
			ui.renderText(renderPrograms.uiProgram,"Texture Resolution",-0.47f,0.1f,0.0004f,glm::vec4(0.06,0.12,0.15,1.0),0.91f,false);

			renderer.startScreenProjectResolutionTextBox.draw(glm::vec3(-0.47f + renderer.startScreenProjectTitleTextBox.width,0.015f,0.9f),glm::vec2(mouseXpos,mouseYpos),firstClick);
			
			
			//4 Select a skybox
			glUseProgram(renderPrograms.iconsProgram);
			ui.iconBox(0.05f/1.5f,0.05f,-0.54f,-0.185f,0.9f,icons.Circle,0.f,glm::vec4(0.8,0.8,0.8,1.0),glm::vec4(0.06,0.12,0.15,1.0));
			glUseProgram(renderPrograms.uiProgram);
			
			ui.renderText(renderPrograms.uiProgram,"4",-0.55f,-0.2f,0.0004f,glm::vec4(0.95,0.95,0.95,1.0),0.91f,false);
			ui.renderText(renderPrograms.uiProgram,"Select a skybox",-0.47f,-0.2f,0.0004f,glm::vec4(0.06,0.12,0.15,1.0),0.91f,false);

			glUseProgram(renderPrograms.renderTheTextureProgram);
			glActiveTexture(GL_TEXTURE14);
			gls.bindTexture(icons.sky1);
			ui.renderTheProgram(-0.35f,-0.4f,0.2f*1.4705/1.5,0.2f);
			
			gls.bindTexture(icons.sky2);
			ui.renderTheProgram(0.15f,-0.4f,0.2f*1.4705/1.5,0.2f);
			
			gls.bindTexture(icons.sky3);
			ui.renderTheProgram(-0.35f,-0.7f,0.2f*1.4705/1.5,0.2f);
			
			gls.bindTexture(icons.sky4);
			ui.renderTheProgram(0.15f,-0.7f,0.2f*1.4705/1.5,0.2f);
			
			
			gls.bindTexture(icons.rendered1);
			ui.renderTheProgram(0.8f,-0.2f,0.8f / 1.428 /1.5,0.8f);
		}
}