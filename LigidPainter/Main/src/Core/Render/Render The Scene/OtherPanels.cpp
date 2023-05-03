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
#include "Core/gl.h"
#include "Core/Texture/Texture.h"
#include "Core/ProjectFile/WRLigidFile.hpp"
#include "Core/ProjectFile/WRLigidMaterialFile.hpp"
#include "Core/ProjectFile/WRLigidFolder.hpp"


#include "stb_image.h"
#include "stb_image_write.h"

#include "LibAL.h"

#include "tinyfiledialogs.h"

int selectedFileIndex = 0;
std::string selectedFile = "";
unsigned int fileTxtr = 0;
Model fileModel;

void drawTheFolder(std::string path,float screenGapX,double mouseXpos,double mouseYpos,GLFWwindow* window,float midPanelW,Icons icons,Programs programs,bool firstClick){
    Utilities util;
    UserInterface ui;
    ColorData colorData;
    ColorData2 colorData2;

    #if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
		#else
			char folderDistinguisher = '/'; 
		#endif

    int bI = 0;
    for (const auto & entry : std::filesystem::directory_iterator(path)){
        int cI = 0;
		std::string fileName = entry.path().string();
		std::string file = util.getLastWordBySeparatingWithChar(fileName,folderDistinguisher);

        bool buttonHover = ui.isMouseOnButton(window,midPanelW,0.02f,(-1.f+midPanelW)+0.05f - screenGapX,(0.9f-0.02f) - ((float)bI * 0.04),mouseXpos,mouseYpos,false);
        if(buttonHover && firstClick){
            selectedFileIndex = bI;
            selectedFile = fileName;
        }
        glEnable(GL_DEPTH_TEST);
        glUseProgram(programs.uiProgram);
        ui.box(midPanelW,0.02f,(-1.f+midPanelW)+0.05f,(0.9f-0.02f) - ((float)bI * 0.04),file,selectedFileIndex == bI ? glm::vec4(colorData.LigidPainterThemeColor,0.7) : colorData.buttonColor,midPanelW-0.01f - (cI/30.f),false,false,0.91f+((buttonHover||selectedFileIndex == bI)/10000.f),10000,selectedFileIndex == bI ? glm::vec4(colorData.LigidPainterThemeColor,1) : colorData.buttonColorHover,buttonHover);
        glDisable(GL_DEPTH_TEST);
		glUseProgram(programs.iconsProgram);
        ui.iconBox(0.01f,0.02f,((-1.f+midPanelW)+0.05f)-(midPanelW-0.01f - (cI/30.f))-0.02f,(0.9f-0.02f) - ((float)bI * 0.04),0.92f,icons.L,0.f,colorData.iconColor,colorData.iconColorHover);
		bI++;

        if(std::filesystem::is_directory(fileName)){
            cI++;  
            //--------------------------------------------------------------------------------------
            for (const auto & entry2 : std::filesystem::directory_iterator(fileName)){
		        std::string fileName2 = entry2.path().string();
		        std::string file2 = util.getLastWordBySeparatingWithChar(fileName2,folderDistinguisher);

    	        bool buttonHover2 = ui.isMouseOnButton(window,midPanelW,0.02f,(-1.f+midPanelW)+0.05f - screenGapX,(0.9f-0.02f) - ((float)bI * 0.04),mouseXpos,mouseYpos,false);
                if(buttonHover2 && firstClick){
                    selectedFileIndex = bI;
                    selectedFile = fileName2;
                }
		        glUseProgram(programs.uiProgram);
		        glEnable(GL_DEPTH_TEST);
                ui.box(midPanelW,0.02f,(-1.f+midPanelW)+0.05f,(0.9f-0.02f) - ((float)bI * 0.04),file2,selectedFileIndex == bI ? glm::vec4(colorData.LigidPainterThemeColor,0.7) : colorData.buttonColor,midPanelW-0.01f - (cI/30.f),false,false,0.91f+((buttonHover2||selectedFileIndex == bI)/10000.f),10000,selectedFileIndex == bI ? glm::vec4(colorData.LigidPainterThemeColor,1) : colorData.buttonColorHover,buttonHover2);
                glDisable(GL_DEPTH_TEST);
		        glUseProgram(programs.iconsProgram);
		        ui.iconBox(0.01f,0.02f,((-1.f+midPanelW)+0.05f)-(midPanelW-0.01f - (cI/30.f))-0.02f,(0.9f-0.02f) - ((float)bI * 0.04),0.92f,icons.L,0.f,colorData.iconColor,colorData.iconColorHover);
		        bI++;
                
                if(std::filesystem::is_directory(fileName2)){
                    cI++;   
                    //--------------------------------------------------------------------------------------
                    for (const auto & entry3 : std::filesystem::directory_iterator(fileName2)){
		                std::string fileName3 = entry3.path().string();
		                std::string file3 = util.getLastWordBySeparatingWithChar(fileName3,folderDistinguisher);

    	                bool buttonHover3 = ui.isMouseOnButton(window,midPanelW,0.02f,(-1.f+midPanelW)+0.05f - screenGapX,(0.9f-0.02f) - ((float)bI * 0.04),mouseXpos,mouseYpos,false);
                        if(buttonHover3 && firstClick){
                            selectedFileIndex = bI;
                            selectedFile = fileName3;
                        }
		                glUseProgram(programs.uiProgram);
		                glEnable(GL_DEPTH_TEST);
                        ui.box(midPanelW,0.02f,(-1.f+midPanelW)+0.05f,(0.9f-0.02f) - ((float)bI * 0.04),file3,selectedFileIndex == bI ? glm::vec4(colorData.LigidPainterThemeColor,0.7) : colorData.buttonColor,midPanelW-0.01f - (cI/30.f),false,false,0.91f+((buttonHover3||selectedFileIndex == bI)/10000.f),10000,selectedFileIndex == bI ? glm::vec4(colorData.LigidPainterThemeColor,1) : colorData.buttonColorHover,buttonHover3);
                        glDisable(GL_DEPTH_TEST);
		                glUseProgram(programs.iconsProgram);
		                ui.iconBox(0.01f,0.022f,((-1.f+midPanelW)+0.05f)- (midPanelW-0.01f - ((cI-1)/30.f))-0.02f,(0.9f-0.02f) - ((float)bI * 0.04),0.92f,icons.I,0.f,colorData.iconColor,colorData.iconColorHover);
		                ui.iconBox(0.01f,0.02f,((-1.f+midPanelW)+0.05f)- (midPanelW-0.01f - ((cI)/30.f))-0.02f,(0.9f-0.02f) - ((float)bI * 0.04),0.92f,icons.L,0.f,colorData.iconColor,colorData.iconColorHover);
		                bI++;
                        if(std::filesystem::is_directory(fileName3)){

                            //--------------------------------------------------------------------------------------
                            
                        
                            //--------------------------------------------------------------------------------------

                        }

	                }   
                    //--------------------------------------------------------------------------------------
                    cI--;  

                    
                }

	        }   
            //--------------------------------------------------------------------------------------
            cI--;  


        }

	}
}

int projectFolderState = 0;
void Render::projectFolderManagerPanel(std::vector<UIElement> &UIElements,Programs renderPrograms,Cubemaps cubemaps,SkyBoxShaderData skyBoxShaderData,
                                        float &createProjectPanelBlurVal,std::string &projectPath,double screenGapX,GLFWwindow* window,Icons icons,double mouseXpos,double mouseYpos,
                                        bool firstClick,bool &displayProjectFolderManager){
        glDisable(GL_DEPTH_TEST);
        Utilities util;
        UserInterface ui;
        ColorData colorData;
        ColorData2 colorData2;
        GlSet gls;
		
        glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
		glUseProgram(renderPrograms.iconsProgram);
		gls.uniformMatrix4fv(renderPrograms.iconsProgram, "Projection", projection);
		glUseProgram(renderPrograms.uiProgram);
		gls.uniformMatrix4fv(renderPrograms.uiProgram, "TextProjection", projection);
		glUseProgram(renderPrograms.renderTheTextureProgram);
		gls.uniform1i(renderPrograms.renderTheTextureProgram, "txtr", 14);
		gls.uniformMatrix4fv(renderPrograms.renderTheTextureProgram, "TextProjection", projection);
		glUseProgram(renderPrograms.renderTheTextureBlur);
		gls.uniformMatrix4fv(renderPrograms.renderTheTextureBlur, "TextProjection", projection);

		UIElements[UIbackfaceCullingCheckBox].checkBox.text = "Import Textures";
		UIElements[UIuseUVCheckBox].checkBox.text = "Import Nodes";
		#if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
		#else
			char folderDistinguisher = '/'; 
		#endif
		
		renderBlurySkybox(cubemaps,skyBoxShaderData,renderPrograms,UIElements,createProjectPanelBlurVal,1.0f,1.0f,false);
		
		glUseProgram(renderPrograms.uiProgram);
		
		//Left column
		ui.box(0.025f,1.0f,(-1.f+0.025f),0.f,"",glm::vec4(colorData.panelColorSnd.r,colorData.panelColorSnd.g,colorData.panelColorSnd.b,1),+0.19f,false,false,0.91f,10000,colorData.panelColorSnd,0);
		
        ui.box(1.0f,0.025f,0.f,(1.f-0.025f),"",glm::vec4(colorData.panelColorSnd.r,colorData.panelColorSnd.g,colorData.panelColorSnd.b,1),+0.19f,false,false,0.91f,10000,colorData.panelColorSnd,0); //Upbar



		glUseProgram(renderPrograms.uiProgram);
		glm::vec4 saveButtonColor = colorData.textColor;
		if(ui.isMouseOnButton(window,0.025f,0.02f,-0.03f-screenGapX,0.95f,mouseXpos,mouseYpos,false)){
			saveButtonColor = glm::vec4(colorData.LigidPainterThemeColor,1);

			//nodePanel.pointerCursor = true;
			if(firstClick){
				//Save project folder
			}
		}
		ui.renderText(renderPrograms.uiProgram,"Save",-0.05f,0.95f,0.00022f,saveButtonColor,0.9f,false);
		
		glm::vec4 loadButtonColor = colorData.textColor;
		if(ui.isMouseOnButton(window,0.025f,0.02f,0.07f-screenGapX,0.95f,mouseXpos,mouseYpos,false)){
			loadButtonColor = glm::vec4(colorData.LigidPainterThemeColor,1);
			//nodePanel.pointerCursor = true;
			if(firstClick){
				//Load project folder
			}
		}
		ui.renderText(renderPrograms.uiProgram,"Load",0.05f,0.95f,0.00022f,loadButtonColor,0.9f,false);


		
        float midPanelW = 0.15f;
		glEnable(GL_DEPTH_TEST);
        ui.box(midPanelW,1.0f,(-1.f+0.15f+0.05f),0.f,"",colorData.panelColorSnd,+0.19f,false,false,0.91f,10000,colorData.panelColorSnd,0);
		glDisable(GL_DEPTH_TEST);
		
		glUseProgram(renderPrograms.iconsProgram);
		//Close
        bool closeIconHover = ui.isMouseOnButton(window,0.01f,0.02f,(-1.f+0.025f)-screenGapX,0.92f,mouseXpos,mouseYpos,false);
		ui.iconBox(0.01f,0.02f,(-1.f+0.025f),0.94f,0.92f,icons.Undo,closeIconHover,colorData.iconColor,colorData.iconColorHover);
        if(closeIconHover && firstClick)
            displayProjectFolderManager = false;
		
		
        bool texturesIconHover = ui.isMouseOnButton(window,0.01f,0.02f,(-1.f+0.025f)-screenGapX,0.82f,mouseXpos,mouseYpos,false);
		ui.iconBox(0.01f,0.02f,(-1.f+0.025f),0.82f,0.92f,icons.Texture,texturesIconHover,colorData.iconColor,colorData.iconColorHover);//Textures
        if(texturesIconHover && firstClick)
            projectFolderState = 0;

        bool tdmodelsIconHover = ui.isMouseOnButton(window,0.01f,0.02f,(-1.f+0.025f)-screenGapX,0.74f,mouseXpos,mouseYpos,false);
		ui.iconBox(0.01f,0.02f,(-1.f+0.025f),0.74f,0.92f,icons.TDModel,tdmodelsIconHover,colorData.iconColor,colorData.iconColorHover);//3DModels
        if(tdmodelsIconHover && firstClick)
            projectFolderState = 1;
		
        bool materialsIconHover = ui.isMouseOnButton(window,0.01f,0.02f,(-1.f+0.025f)-screenGapX,0.66f,mouseXpos,mouseYpos,false);
		ui.iconBox(0.01f,0.02f,(-1.f+0.025f),0.66f,0.92f,icons.Material,materialsIconHover,colorData.iconColor,colorData.iconColorHover);//Materials
        if(materialsIconHover && firstClick)
            projectFolderState = 2;
		
        bool nodesIconHover = ui.isMouseOnButton(window,0.01f,0.02f,(-1.f+0.025f)-screenGapX,0.58f,mouseXpos,mouseYpos,false);
		ui.iconBox(0.01f,0.02f,(-1.f+0.025f),0.58f,0.92f,icons.Mark,nodesIconHover,colorData.iconColor,colorData.iconColorHover);//Nodes
        if(nodesIconHover && firstClick)
            projectFolderState = 3;
		
        bool shadersIconHover = ui.isMouseOnButton(window,0.01f,0.02f,(-1.f+0.025f)-screenGapX,0.50f,mouseXpos,mouseYpos,false);
		ui.iconBox(0.01f,0.02f,(-1.f+0.025f),0.50f,0.92f,icons.BrushFolder,shadersIconHover,colorData.iconColor,colorData.iconColorHover);//Shaders
        if(shadersIconHover && firstClick)
            projectFolderState = 4;

		glUseProgram(renderPrograms.uiProgram);

        std::string currentFolder = "";
        if(projectFolderState == 0)
            currentFolder = "Textures";
        if(projectFolderState == 1)
            currentFolder = "3DModels";
        if(projectFolderState == 2)
            currentFolder = "Materials";
        if(projectFolderState == 3)
            currentFolder = "Nodes";
        if(projectFolderState == 4)
            currentFolder = "Shaders";
        

        drawTheFolder(projectPath + folderDistinguisher + currentFolder,screenGapX,mouseXpos,mouseYpos,window,midPanelW,icons,renderPrograms,firstClick);
        glEnable(GL_DEPTH_TEST);

        if(projectFolderState == 0){
            Texture texture;
            if(firstClick){
                glDeleteTextures(1,&fileTxtr);
                fileTxtr = texture.getTexture(selectedFile,0,0,0);
            }

            glUseProgram(renderPrograms.renderTheTextureProgram);

            float rad = 0.4f;
            std::vector<float> renderVertices = { 
		        rad,  rad*2, 0.0f,1,1,0,0,0,  // top right
		        rad,  -rad*2, 0.0f,1,0,0,0,0,  // bottom right
		        -rad,  rad*2, 0.0f,0,1,0,0,0,  // top left 
		        
                rad,  -rad*2, 0.0f,1,0,0,0,0,  // bottom right
		        -rad,  -rad*2, 0.0f,0,0,0,0,0,  // bottom left
		        -rad,  rad*2, 0.0f,0,1,0,0,0   // top left
	        };
            glActiveTexture(GL_TEXTURE14);
            glBindTexture(GL_TEXTURE_2D,fileTxtr);
            gls.drawArrays(renderVertices,0);
        }        
        if(projectFolderState == 1){
            glUseProgram(renderPrograms.solidRenderer);
            glm::vec3 mostFar = fileModel.getMostFarVector();
            gls.uniform3fv(renderPrograms.solidRenderer,"mostFar",mostFar);


	        glm::mat4 view = glm::lookAt(glm::vec3(5,0,0), glm::vec3(0), glm::vec3(0.0, 1.0, 0.0)); 
            
	        gls.uniformMatrix4fv(renderPrograms.solidRenderer,"view",view);


            if(firstClick){
                fileModel.loadModel(selectedFile,true);
            }

            fileModel.Draw();
        }
}