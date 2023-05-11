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

std::vector<std::string> tdModelPaths;
unsigned int selectedSkyBox = 0;

void Render::startScreenPanel(std::vector<UIElement> &UIElements,Programs renderPrograms,Cubemaps cubemaps,SkyBoxShaderData skyBoxShaderData,
                                        float &createProjectPanelBlurVal,std::string &projectPath,double screenGapX,GLFWwindow* window,Icons icons,double mouseXpos,double mouseYpos,
                                        bool firstClick,bool &displayProjectFolderManager,std::vector<Font> &fonts,ProjectManager &projectManager,std::vector<aTexture> &albedoTextures
                                        ,int txtrRes,std::vector<NodeScene> &materials, std::vector<Node> &appNodes, ContextMenu &addNodeContexMenu, Model &model,bool firstClickR,Renderer &renderer
										,float &scrVal){
		#if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
		#else
			char folderDistinguisher = '/'; 
		#endif


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
		glUseProgram(renderPrograms.gradient);
		gls.uniformMatrix4fv(renderPrograms.gradient, "TextProjection", projection);
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

		glUseProgram(renderPrograms.gradient);
		ui.renderTheProgram(-1.0 + 0.4f + 0.9f,0.0f,0.9f,1.f);
		
		glUseProgram(renderPrograms.directionalShadow);
		ui.renderTheProgram(-1.0 + 0.4f + 0.9f,0.0f,0.9f,1.f);

					glUseProgram(renderPrograms.renderTheTextureProgram);
			glActiveTexture(GL_TEXTURE14);
			gls.uniform1f(renderPrograms.renderTheTextureProgram,"opacity",0.1f);
			
			gls.bindTexture(icons.rendered1);
			ui.renderTheProgram(0.2f,-0.2f,0.8f / 1.428 /1.5,0.8f);
			gls.uniform1f(renderPrograms.renderTheTextureProgram,"opacity",1.f);
		
        
		//glEnable(GL_DEPTH_TEST);
		
		
		glUseProgram(renderPrograms.uiProgram);
        renderer.startScreenNewProjectButton.draw(glm::vec3(-1.f + 0.2f,0.7f,0.92f),glm::vec2(mouseXpos,mouseYpos));
        renderer.startScreenLoadProjectButton.draw(glm::vec3(-1.f + 0.2f,0.58f,0.92f),glm::vec2(mouseXpos,mouseYpos));
			


		if(createProjectMode){
			ui.renderText(renderPrograms.uiProgram,"Create new 3D project",-0.55f,0.8f-scrVal,0.0006f,glm::vec4(0.06,0.12,0.15,1.0),0.91f,false);
			
			//1 Project Title
			glUseProgram(renderPrograms.iconsProgram);
			ui.iconBox(0.05f/1.5f,0.05f,-0.54f,0.715f-scrVal,0.9f,icons.Circle,0.f,glm::vec4(0.8,0.8,0.8,1.0),glm::vec4(0.06,0.12,0.15,1.0));
			glUseProgram(renderPrograms.uiProgram);
			
			ui.renderText(renderPrograms.uiProgram,"1",-0.55f,0.7f-scrVal,0.0004f,glm::vec4(0.95,0.95,0.95,1.0),0.91f,false);
			ui.renderText(renderPrograms.uiProgram,"Project Title",-0.47f,0.7f-scrVal,0.0004f,glm::vec4(0.06,0.12,0.15,1.0),0.91f,false);
			
			renderer.startScreenProjectTitleTextBox.draw(glm::vec3(-0.47f + renderer.startScreenProjectTitleTextBox.width,0.615f-scrVal,0.9f),glm::vec2(mouseXpos,mouseYpos),firstClick);
			
			
			//2 Project Path
			glUseProgram(renderPrograms.iconsProgram);
			ui.iconBox(0.05f/1.5f,0.05f,-0.54f,0.415f-scrVal,0.9f,icons.Circle,0.f,glm::vec4(0.8,0.8,0.8,1.0),glm::vec4(0.06,0.12,0.15,1.0));
			glUseProgram(renderPrograms.uiProgram);
			
			ui.renderText(renderPrograms.uiProgram,"2",-0.55f,0.4f-scrVal,0.0004f,glm::vec4(0.95,0.95,0.95,1.0),0.91f,false);
			ui.renderText(renderPrograms.uiProgram,"Project Path",-0.47f,0.4f-scrVal,0.0004f,glm::vec4(0.06,0.12,0.15,1.0),0.91f,false);

			renderer.startScreenProjectPathTextBox.draw(glm::vec3(-0.47f + renderer.startScreenProjectTitleTextBox.width,0.315f-scrVal,0.9f),glm::vec2(mouseXpos,mouseYpos),firstClick);
			
			
			//3 Texture Resolution
			glUseProgram(renderPrograms.iconsProgram);
			ui.iconBox(0.05f/1.5f,0.05f,-0.54f,0.115f-scrVal,0.9f,icons.Circle,0.f,glm::vec4(0.8,0.8,0.8,1.0),glm::vec4(0.06,0.12,0.15,1.0));
			glUseProgram(renderPrograms.uiProgram);
			
			ui.renderText(renderPrograms.uiProgram,"3",-0.55f,0.1f-scrVal,0.0004f,glm::vec4(0.95,0.95,0.95,1.0),0.91f,false);
			ui.renderText(renderPrograms.uiProgram,"Texture Resolution",-0.47f,0.1f-scrVal,0.0004f,glm::vec4(0.06,0.12,0.15,1.0),0.91f,false);

			renderer.startScreenProjectResolutionTextBox.draw(glm::vec3(-0.47f + renderer.startScreenProjectTitleTextBox.width,0.015f-scrVal,0.9f),glm::vec2(mouseXpos,mouseYpos),firstClick);
			
			
			//4 Select a skybox
			glUseProgram(renderPrograms.iconsProgram);
			ui.iconBox(0.05f/1.5f,0.05f,-0.54f,-0.185f-scrVal,0.9f,icons.Circle,0.f,glm::vec4(0.8,0.8,0.8,1.0),glm::vec4(0.06,0.12,0.15,1.0));
			glUseProgram(renderPrograms.uiProgram);
			
			ui.renderText(renderPrograms.uiProgram,"4",-0.55f,-0.2f-scrVal,0.0004f,glm::vec4(0.95,0.95,0.95,1.0),0.91f,false);
			ui.renderText(renderPrograms.uiProgram,"Select a skybox",-0.47f,-0.2f-scrVal,0.0004f,glm::vec4(0.06,0.12,0.15,1.0),0.91f,false);

			glUseProgram(renderPrograms.renderTheTextureProgram);
			glActiveTexture(GL_TEXTURE14);
			gls.bindTexture(icons.sky1);
			ui.renderTheProgram(-0.35f,-0.4f-scrVal,0.1f*1.4705/1.5,0.1f);
			
			gls.bindTexture(icons.sky2);
			ui.renderTheProgram(-0.1f,-0.4f-scrVal,0.1f*1.4705/1.5,0.1f);
			
			gls.bindTexture(icons.sky3);
			ui.renderTheProgram(0.15f,-0.4f-scrVal,0.1f*1.4705/1.5,0.1f);
			
			gls.bindTexture(icons.sky4);
			ui.renderTheProgram(0.4f,-0.4f-scrVal,0.1f*1.4705/1.5,0.1f);
			
			gls.bindTexture(icons.sky5);
			ui.renderTheProgram(-0.35f,-0.6f-scrVal,0.1f*1.4705/1.5,0.1f);
			
			gls.bindTexture(icons.sky6);
			ui.renderTheProgram(-0.1f,-0.6f-scrVal,0.1f*1.4705/1.5,0.1f);

			glUseProgram(renderPrograms.uiProgram);
        	
			bool sky0Enter = ui.isMouseOnButton(window,0.11f*1.4705/1.5,0.08f,-0.35f,-0.4f-scrVal,mouseXpos,mouseYpos,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);
			if(sky0Enter && firstClick)
				selectedSkyBox = 0;
			glm::vec4 sky0BtnColor = glm::vec4(0.06,0.12,0.15,1.0);
			glm::vec4 sky0BtnColorActive = glm::vec4(colorData.LigidPainterThemeColor,1.0);
			if(sky0Enter){
				sky0BtnColor = glm::vec4(0.06,0.12,0.15,0.5);
				sky0BtnColorActive = glm::vec4(colorData.LigidPainterThemeColor,0.5);
			}
			ui.box(0.11f*1.4705/1.5,0.08f,-0.35f,-0.4f-scrVal,"",sky0BtnColor,0,false,false,0.92f,40,sky0BtnColorActive,selectedSkyBox == 0);
        	
			bool sky1Enter = ui.isMouseOnButton(window,0.11f*1.4705/1.5,0.08f,-0.1f, -0.4f-scrVal,mouseXpos,mouseYpos,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);
			if(sky1Enter && firstClick)
				selectedSkyBox = 1;
			glm::vec4 sky1BtnColor = glm::vec4(0.06,0.12,0.15,1.0);
			glm::vec4 sky1BtnColorActive = glm::vec4(colorData.LigidPainterThemeColor,1.0);
			if(sky1Enter){
				sky1BtnColor = glm::vec4(0.06,0.12,0.15,0.5);
				sky1BtnColorActive = glm::vec4(colorData.LigidPainterThemeColor,0.5);
			}
			ui.box(0.11f*1.4705/1.5,0.08f,-0.1f, -0.4f-scrVal,"",sky1BtnColor,0,false,false,0.92f,40,sky1BtnColorActive,selectedSkyBox == 1);
        	
			bool sky2Enter = ui.isMouseOnButton(window,0.11f*1.4705/1.5,0.08f, 0.15f,-0.4f-scrVal,mouseXpos,mouseYpos,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);
			if(sky2Enter && firstClick)
				selectedSkyBox = 2;
			glm::vec4 sky2BtnColor = glm::vec4(0.06,0.12,0.15,1.0);
			glm::vec4 sky2BtnColorActive = glm::vec4(colorData.LigidPainterThemeColor,1.0);
			if(sky2Enter){
				sky2BtnColor = glm::vec4(0.06,0.12,0.15,0.5);
				sky2BtnColorActive = glm::vec4(colorData.LigidPainterThemeColor,0.5);
			}
			ui.box(0.11f*1.4705/1.5,0.08f, 0.15f,-0.4f-scrVal,"",sky2BtnColor,0,false,false,0.92f,40,sky2BtnColorActive,selectedSkyBox == 2);
        	
			bool sky3Enter = ui.isMouseOnButton(window,0.11f*1.4705/1.5,0.08f, 0.4f, -0.4f-scrVal,mouseXpos,mouseYpos,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);
			if(sky3Enter && firstClick)
				selectedSkyBox = 3;
			glm::vec4 sky3BtnColor = glm::vec4(0.06,0.12,0.15,1.0);
			glm::vec4 sky3BtnColorActive = glm::vec4(colorData.LigidPainterThemeColor,1.0);
			if(sky3Enter){
				sky3BtnColor = glm::vec4(0.06,0.12,0.15,0.5);
				sky3BtnColorActive = glm::vec4(colorData.LigidPainterThemeColor,0.5);
			}
			ui.box(0.11f*1.4705/1.5,0.08f, 0.4f, -0.4f-scrVal,"",sky3BtnColor,0,false,false,0.92f,40,sky3BtnColorActive,selectedSkyBox == 3);
        	
			bool sky4Enter = ui.isMouseOnButton(window,0.11f*1.4705/1.5,0.08f,-0.35f,-0.6f-scrVal,mouseXpos,mouseYpos,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);
			if(sky4Enter && firstClick)
				selectedSkyBox = 4;
			glm::vec4 sky4BtnColor = glm::vec4(0.06,0.12,0.15,1.0);
			glm::vec4 sky4BtnColorActive = glm::vec4(colorData.LigidPainterThemeColor,1.0);
			if(sky4Enter){
				sky4BtnColor = glm::vec4(0.06,0.12,0.15,0.5);
				sky4BtnColorActive = glm::vec4(colorData.LigidPainterThemeColor,0.5);
			}
			ui.box(0.11f*1.4705/1.5,0.08f,-0.35f,-0.6f-scrVal,"",sky4BtnColor,0,false,false,0.92f,40,sky4BtnColorActive,selectedSkyBox == 4);
        	
			bool sky5Enter = ui.isMouseOnButton(window,0.11f*1.4705/1.5,0.08f,-0.1f, -0.6f-scrVal,mouseXpos,mouseYpos,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);
			if(sky5Enter && firstClick)
				selectedSkyBox = 5;
			glm::vec4 sky5BtnColor = glm::vec4(0.06,0.12,0.15,1.0);
			glm::vec4 sky5BtnColorActive = glm::vec4(colorData.LigidPainterThemeColor,1.0);
			if(sky5Enter){
				sky5BtnColor = glm::vec4(0.06,0.12,0.15,0.5);
				sky5BtnColorActive = glm::vec4(colorData.LigidPainterThemeColor,0.5);
			}
			ui.box(0.11f*1.4705/1.5,0.08f,-0.1f, -0.6f-scrVal,"",sky5BtnColor,0,false,false,0.92f,40,sky5BtnColorActive,selectedSkyBox == 5);

			
			//5 Project Settings			
			glUseProgram(renderPrograms.iconsProgram);
			ui.iconBox(0.05f/1.5f,0.05f,-0.54f,-0.885f-scrVal,0.9f,icons.Circle,0.f,glm::vec4(0.8,0.8,0.8,1.0),glm::vec4(0.06,0.12,0.15,1.0));
			glUseProgram(renderPrograms.uiProgram);

			ui.renderText(renderPrograms.uiProgram,"5",-0.55f,-0.9f-scrVal,0.0004f,glm::vec4(0.95,0.95,0.95,1.0),0.91f,false);
			ui.renderText(renderPrograms.uiProgram,"Project settings",-0.47f,-0.9f-scrVal,0.0004f,glm::vec4(0.06,0.12,0.15,1.0),0.91f,false);
			
			renderer.startScreenIncludeTexturesCheckBox.draw(glm::vec3(-0.45,-1.0f-scrVal,0.95f),glm::vec2(mouseXpos,mouseYpos),firstClick);
			renderer.startScreenIncludeNodesCheckBox.draw(glm::vec3(-0.15,-1.0f-scrVal,0.95f),glm::vec2(mouseXpos,mouseYpos),firstClick);
			renderer.startScreenIncludeFontsCheckBox.draw(glm::vec3(0.15,-1.0f-scrVal,0.95f),glm::vec2(mouseXpos,mouseYpos),firstClick);

			//6 Upload 3D Models
			glUseProgram(renderPrograms.iconsProgram);
			ui.iconBox(0.05f/1.5f,0.05f,-0.54f,-1.285f-scrVal,0.9f,icons.Circle,0.f,glm::vec4(0.8,0.8,0.8,1.0),glm::vec4(0.06,0.12,0.15,1.0));
			glUseProgram(renderPrograms.uiProgram);

			ui.renderText(renderPrograms.uiProgram,"6",-0.55f,-1.3f-scrVal,0.0004f,glm::vec4(0.95,0.95,0.95,1.0),0.91f,false);
			ui.renderText(renderPrograms.uiProgram,"Upload 3D Models",-0.47f,-1.3f-scrVal,0.0004f,glm::vec4(0.06,0.12,0.15,1.0),0.91f,false);

			float posX = 0;
			float posY = -0.31f;
			for (size_t i = 0; i < tdModelPaths.size()+1; i++)
			{
				posX += (0.15*2);
				if(i%4 == 0){
					posX = 0.f;
					posY += 0.31f;
				}				
				bool buttonEnter = ui.isMouseOnButton(window,0.15f,0.15f,-0.37f + posX,-1.5f-scrVal - posY,mouseXpos,mouseYpos,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);
        		ui.box(0.15f,0.15f,-0.37f + posX,-1.5f-scrVal - posY,"",glm::vec4(0.06,0.12,0.15,1.0),0,false,false,0.92f,10,glm::vec4(colorData.LigidPainterThemeColor,1.0),buttonEnter);
				
				if(i == 0){
					if(buttonEnter && firstClick){
						char const* lFilterPatterns[11] = { "*.obj","*.gltf", "*.fbx", "*.stp", "*.max","*.x3d","*.obj","*.vrml","*.3ds","*.stl","*.dae" };	
						char * modelFilePathCheck = tinyfd_openFileDialog("Select 3D Model","",11, lFilterPatterns,"",false);
						if(modelFilePathCheck)
							tdModelPaths.push_back(modelFilePathCheck);
					}
					
					glUseProgram(renderPrograms.iconsProgram);
					ui.iconBox(0.05f/1.5f,0.05f,-0.37f + posX,-1.45f-scrVal - posY,0.95f,icons.Plus,buttonEnter,glm::vec4(0.06,0.12,0.15,1.0),glm::vec4(colorData.LigidPainterThemeColor,1.0));
					
					glUseProgram(renderPrograms.uiProgram);
					ui.renderText(renderPrograms.uiProgram,"Add Model", -0.3f + posX - 0.15f,-1.56f-scrVal - posY,0.00032f,glm::vec4(0.06,0.12,0.15,1.0),0.95f,false);
				}
				else{
					ui.renderText(renderPrograms.uiProgram,util.getLastWordBySeparatingWithChar(tdModelPaths[i-1],folderDistinguisher), -0.37f + posX,-1.5f-scrVal - posY,0.00032f,glm::vec4(0.06,0.12,0.15,1.0),0.95f,false,-0.37f + posX + 0.14f, false);
					
					//Delete Button
					bool delButtonEnter = ui.isMouseOnButton(window,0.03f/1.5f,0.03f,-0.26f + posX,-1.605f-scrVal - posY,mouseXpos,mouseYpos,false,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);
					ui.box(0.03f/1.5f,0.03f,-0.26f + posX,-1.605f-scrVal - posY,"",glm::vec4(0.86,0.12,0.15,1.0),0,false,false,0.92f,10000,glm::vec4(colorData.LigidPainterThemeColor,1.0),delButtonEnter);
					glUseProgram(renderPrograms.iconsProgram);
					ui.iconBox(0.03f/1.5f,0.03f,-0.26f + posX,-1.605f-scrVal - posY,0.95f,icons.Trash,delButtonEnter,glm::vec4(1.0,1.0,1.0,1.0),glm::vec4(colorData.LigidPainterThemeColor,1.0));
				
					if(delButtonEnter && firstClick){
						tdModelPaths.erase(tdModelPaths.begin()+i-1);
						break;
					}
					glUseProgram(renderPrograms.uiProgram);
				}
			}

			glUseProgram(renderPrograms.uiProgram);
			ui.box(0.9f,0.08f,-1.0 + 0.4f + 0.9f,-1.0f+0.08,"",glm::vec4(0.86,0.86,0.86,1.0),0,false,false,0.92f,10000,glm::vec4(colorData.LigidPainterThemeColor,1.0),0);

			renderer.startScreenCreateTheProjectButton.draw(glm::vec3(-1.0 + 0.4f + 0.9f,-1.0f+0.08,0.95f),glm::vec2(mouseXpos,mouseYpos));
		}
}