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

std::vector<std::string> selectedFiles;
std::string selectedFile = "";
unsigned int fileTxtr = 0;
Model fileModel;
Node fileNode;
Font fileFont;

int lastBI = 0;

int countSlashes(std::string s){
    int counter = 0;
    #if defined(_WIN32) || defined(_WIN64)
	    char folderDistinguisher = '\\';
	#else
		char folderDistinguisher = '/'; 
	#endif
    for (size_t i = 0; i < s.size(); i++)
    {
        if(s[i] == folderDistinguisher)
            counter++;
    }
    return counter;
}

bool doesContains(std::string fullPath,std::string n){
    if(fullPath == n)  
        return false;
    for (size_t i = 0; i < n.size(); i++)
    {
        if(i == fullPath.size())
            return false;
        
        if(n[i] != fullPath[i])
            return false;
    }
    return true;
}

bool isSelected(std::string fileName){
    for (size_t i = 0; i < selectedFiles.size(); i++)
    {
        if(selectedFiles[i] == fileName){
            return true;
        }
    }
    
    return false;
}
bool selectTheFile(std::string path,bool doRemove){
    bool isContains = false;
    for (size_t i = 0; i < selectedFiles.size(); i++)
    {
        if(selectedFiles[i] == path){
            if(doRemove)
                selectedFiles.erase(selectedFiles.begin() + i);
            isContains = true;
        }
    }
    if(!isContains){
        selectedFiles.push_back(path);
    }
    return !isContains;
}


std::vector<std::string> hiddenFolders;

std::string contextMenuFile;
glm::vec2 contextMenuPos;

void hideTheFolder(std::string path){
    bool isContains = false;
    for (size_t i = 0; i < hiddenFolders.size(); i++)
    {
        if(hiddenFolders[i] == path){
            hiddenFolders.erase(hiddenFolders.begin() + i);
            isContains = true;
        }
    }
    if(!isContains){
        hiddenFolders.push_back(path);
    }
    
}

void drawTheFolder(std::string path,float screenGapX,double mouseXpos,double mouseYpos,GLFWwindow* window,float midPanelW,Icons icons,Programs programs,bool firstClick,ProjectManager &projectManager,bool firstClickR){
    Utilities util;
    UserInterface ui;
    ColorData colorData;
    ColorData2 colorData2;

    #if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
		#else
			char folderDistinguisher = '/'; 
		#endif

    projectManager.projectManagerScrollVal = util.restrictBetween(projectManager.projectManagerScrollVal,0,min(-lastBI + 46,0));
    int bI = projectManager.projectManagerScrollVal;
    
    glEnable(GL_DEPTH_TEST);
    //Slider
    float sliderWidth = 1.f / (std::max(lastBI / 46.f,1.f));
    ui.box(0.005f,sliderWidth,(-1.f+0.15f+0.05f) + midPanelW - 0.005f + screenGapX,((1.f - (0.025f*2.f)) - sliderWidth) + (projectManager.projectManagerScrollVal * 0.04f / std::max(lastBI / 46.f,1.f)),"",glm::vec4(0.7f,0.7f,0.7f,0.7f),0,false,false,0.99f,10000,colorData.iconColor,0);
    glDisable(GL_DEPTH_TEST);
    
    lastBI = 0;
    if(firstClickR)
        contextMenuFile = "";
    for (const auto & entry : std::filesystem::recursive_directory_iterator(path)){
        int cI = 0;
		std::string fileName = entry.path().string();
		std::string file = util.getLastWordBySeparatingWithChar(fileName,folderDistinguisher);

        bool isHidden = false;
        for (size_t i = 0; i < hiddenFolders.size(); i++)
        {
            if(doesContains(fileName,hiddenFolders[i]))
                isHidden = true;
        }
        
        if(!isHidden){
            bool buttonHover = ui.isMouseOnButton(window,midPanelW,0.02f,(-1.f+midPanelW)+0.05f,(0.9f-0.02f) - ((float)bI * 0.04),mouseXpos,mouseYpos,false);
            if(buttonHover && firstClickR){
                contextMenuFile = fileName;
                contextMenuPos.x = mouseXpos/glfwGetVideoMode(glfwGetPrimaryMonitor())->width*2.f -1.f;
                contextMenuPos.y = (0.9f-0.02f) - ((float)bI * 0.04);
            }
            if(buttonHover){
                if((firstClick || firstClickR)){
                    if((!glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && firstClick) || (firstClickR && !isSelected(fileName))){
                        selectedFiles.clear();
                    }
                    selectTheFile(fileName,firstClick);
                    selectedFile = fileName;
                }
            }
            
            if(contextMenuFile != "")
                buttonHover = false;
            glEnable(GL_DEPTH_TEST);
            glUseProgram(programs.uiProgram);
            if(fileName == contextMenuFile){
                bool copyPathHover = false;
                if(ui.isMouseOnButton(window,0.06f,0.02f,contextMenuPos.x,contextMenuPos.y,mouseXpos,mouseYpos,0))
                    copyPathHover = true;
                if(copyPathHover && firstClick)
                    glfwSetClipboardString(window,std::filesystem::absolute(fileName.c_str()).string().c_str());
                ui.box(0.07f,0.02f,contextMenuPos.x + screenGapX,contextMenuPos.y,"Copy Path",isSelected(fileName) ? glm::vec4(colorData.LigidPainterThemeColor,0.7) : glm::vec4(0.98f,0.98f,0.98f,1),0.06f,false,false,0.93f,10000,isSelected(fileName) ? glm::vec4(colorData.LigidPainterThemeColor,1) : colorData.buttonColorHover,copyPathHover);

                bool renameHover = false;
                if(ui.isMouseOnButton(window,0.06f,0.02f,contextMenuPos.x,contextMenuPos.y - 0.04f,mouseXpos,mouseYpos,0))
                    renameHover = true;
                ui.box(0.07f,0.02f,contextMenuPos.x + screenGapX,contextMenuPos.y - 0.04,"Rename",isSelected(fileName) ? glm::vec4(colorData.LigidPainterThemeColor,0.7) : glm::vec4(0.98f,0.98f,0.98f,1),0.06f ,false,false,0.93f,10000,isSelected(fileName) ? glm::vec4(colorData.LigidPainterThemeColor,1) : colorData.buttonColorHover,renameHover);

                
                if(std::filesystem::is_directory(fileName)){
                    bool pasteHover = false;
                    if(ui.isMouseOnButton(window,0.06f,0.02f,contextMenuPos.x,contextMenuPos.y - 0.08f,mouseXpos,mouseYpos,0))
                        pasteHover = true;
                    if(pasteHover && firstClick){
                        if(std::filesystem::exists(glfwGetClipboardString(window)))//Does exists?
                            if(!std::filesystem::is_directory(glfwGetClipboardString(window)))//Is a file?
                                std::filesystem::copy(glfwGetClipboardString(window),fileName);
                    }
                    ui.box(0.07f,0.02f,contextMenuPos.x + screenGapX,contextMenuPos.y - 0.08f,"Paste The Path",isSelected(fileName) ? glm::vec4(colorData.LigidPainterThemeColor,0.7) : glm::vec4(0.98f,0.98f,0.98f,1), 0.06f,false,false,0.93f,10000,isSelected(fileName) ? glm::vec4(colorData.LigidPainterThemeColor,1) : colorData.buttonColorHover,pasteHover);
                    
                    bool addFileHover = false;
                    if(ui.isMouseOnButton(window,0.06f,0.02f,contextMenuPos.x ,contextMenuPos.y - 0.12f,mouseXpos,mouseYpos,0))
                        addFileHover = true;
                    if(addFileHover && firstClick){
                        char* addFilePath = tinyfd_openFileDialog("Select the file","",0,{},{},0);
                        if(addFilePath)
                            if(std::filesystem::exists(addFilePath))//Does exists?
                                if(!std::filesystem::is_directory(addFilePath))//Is a file?
                                    std::filesystem::copy(addFilePath,fileName);

                    }
                    ui.box(0.07f,0.02f,contextMenuPos.x + screenGapX,contextMenuPos.y - 0.12f,"Add File",isSelected(fileName) ? glm::vec4(colorData.LigidPainterThemeColor,0.7) : glm::vec4(0.98f,0.98f,0.98f,1), 0.06f,false,false,0.93f,10000,isSelected(fileName) ? glm::vec4(colorData.LigidPainterThemeColor,1) : colorData.buttonColorHover,addFileHover);
                }
                else{
                    bool deleteHover = false;
                    if(ui.isMouseOnButton(window,0.06f,0.02f,contextMenuPos.x,contextMenuPos.y - 0.08f,mouseXpos,mouseYpos,0))
                        deleteHover = true;
                    if(deleteHover && firstClick){
                        for (size_t i = 0; i < selectedFiles.size(); i++)
                        {
                            std::filesystem::remove(selectedFiles[i]);
                        }
                        
                    }
                    ui.box(0.07f,0.02f,contextMenuPos.x + screenGapX,contextMenuPos.y - 0.08,"Delete",isSelected(fileName) ? glm::vec4(colorData.LigidPainterThemeColor,0.7) : glm::vec4(0.98f,0.98f,0.98f,1),0.06f ,false,false,0.93f,10000,isSelected(fileName) ? glm::vec4(colorData.LigidPainterThemeColor,1) : colorData.buttonColorHover,deleteHover);
                }
                
            }
            if(bI >= 0)
                ui.box(midPanelW,0.02f,(-1.f+midPanelW)+0.05f+screenGapX,(0.9f-0.02f) - ((float)bI * 0.04),file,(isSelected(fileName)) || (fileName == contextMenuFile) ? glm::vec4(colorData.LigidPainterThemeColor,0.7) : glm::vec4(0.98f,0.98f,0.98f,1),midPanelW-0.01f - (cI/30.f) + ((countSlashes(path) - countSlashes(fileName)) / 50.f) ,false,false,0.91f+((buttonHover||isSelected(fileName))/10000.f),10000,isSelected(fileName) ? glm::vec4(colorData.LigidPainterThemeColor,1) : glm::vec4(0.9f,0.9f,0.9f,1),buttonHover,glm::vec4(0.06,0.12,0.15,1.0),glm::vec4(0.06,0.12,0.15,1.0),0.00022f,true);

            if(std::filesystem::is_directory(fileName) && bI >= 0){
                bool hideButtonHover = buttonHover;
                glUseProgram(programs.iconsProgram);
                ui.iconBox(0.006f,0.012f,(-1.f+midPanelW)+ 0.04f +screenGapX - (midPanelW-0.01f - (cI/30.f) + ((countSlashes(path) - countSlashes(fileName)) / 50.f)),(0.9f-0.02f) - ((float)bI * 0.04),0.94f,icons.ArrowDown,hideButtonHover,colorData.iconColor,colorData.iconColorHover);
                if(hideButtonHover && firstClick){
                    hideTheFolder(fileName);
                }
            }
            bI++;
		    lastBI++;
        }
	}
    if(contextMenuFile == path){
        bool pasteHover = false;
        if(ui.isMouseOnButton(window,0.06f,0.02f,contextMenuPos.x,contextMenuPos.y - 0.08f,mouseXpos,mouseYpos,0))
            pasteHover = true;
        if(pasteHover && firstClick){
            if(std::filesystem::exists(glfwGetClipboardString(window)))//Does exists?
                if(!std::filesystem::is_directory(glfwGetClipboardString(window)))//Is a file?
                    std::filesystem::copy(glfwGetClipboardString(window),path);
        }
        ui.box(0.07f,0.02f,contextMenuPos.x + screenGapX,contextMenuPos.y - 0.08f,"Paste The Path",isSelected(path) ? glm::vec4(colorData.LigidPainterThemeColor,0.7) : glm::vec4(0.98f,0.98f,0.98f,1), 0.06f,false,false,0.93f,10000,isSelected(path) ? glm::vec4(colorData.LigidPainterThemeColor,1) : colorData.buttonColorHover,pasteHover);
        
        bool addFileHover = false;
        if(ui.isMouseOnButton(window,0.06f,0.02f,contextMenuPos.x,contextMenuPos.y - 0.12f,mouseXpos,mouseYpos,0))
            addFileHover = true;
        if(addFileHover && firstClick){
            char* addFilePath = tinyfd_openFileDialog("Select the file","",0,{},{},0);
            if(addFilePath)
                if(std::filesystem::exists(addFilePath))//Does exists?
                    if(!std::filesystem::is_directory(addFilePath))//Is a file?
                        std::filesystem::copy(addFilePath,path);
        }
        ui.box(0.07f,0.02f,contextMenuPos.x + screenGapX,contextMenuPos.y - 0.12f,"Add File",isSelected(path) ? glm::vec4(colorData.LigidPainterThemeColor,0.7) : glm::vec4(0.98f,0.98f,0.98f,1), 0.06f,false,false,0.93f,10000,isSelected(path) ? glm::vec4(colorData.LigidPainterThemeColor,1) : colorData.buttonColorHover,addFileHover);

    }
    if(firstClick)
        contextMenuFile = "";
    if(firstClickR && projectManager.elementPanelHover && contextMenuFile == ""){
        contextMenuFile = path;
        contextMenuPos.x = mouseXpos/glfwGetVideoMode(glfwGetPrimaryMonitor())->width*2.f -1.f;
        contextMenuPos.y = (0.9f-0.02f) - ((float)bI * 0.04);

    }
}

int projectFolderState = 0;
bool showInfo = false;
void Render::projectFolderManagerPanel(std::vector<UIElement> &UIElements,Programs renderPrograms,Cubemaps cubemaps,SkyBoxShaderData skyBoxShaderData,
                                        float &createProjectPanelBlurVal,std::string &projectPath,double screenGapX,GLFWwindow* window,Icons icons,double mouseXpos,double mouseYpos,
                                        bool firstClick,bool &displayProjectFolderManager,std::vector<Font> &fonts,ProjectManager &projectManager,std::vector<aTexture> &albedoTextures
                                        ,int txtrRes,std::vector<NodeScene> &materials, std::vector<Node> &appNodes, ContextMenu &addNodeContexMenu, Model &model,bool firstClickR
                                        ,int & chosenSkyboxIndex){
        glDisable(GL_DEPTH_TEST);
        Utilities util;
        UserInterface ui;
        ColorData colorData;
        ColorData2 colorData2;
        GlSet gls;

	    glm::mat4 view = glm::lookAt(glm::vec3(-5,0,0), glm::vec3(0), glm::vec3(0.0, 1.0, 0.0)); 
        skyBoxShaderData.view = view;

		
        glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
		glUseProgram(renderPrograms.iconsProgram);
		gls.uniformMatrix4fv(renderPrograms.iconsProgram, "Projection", projection);
		glUseProgram(renderPrograms.uiProgram);
		gls.uniformMatrix4fv(renderPrograms.uiProgram, "TextProjection", projection);
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

		#if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
		#else
			char folderDistinguisher = '/'; 
		#endif
		
		renderBlurySkybox(cubemaps,skyBoxShaderData,renderPrograms,UIElements,createProjectPanelBlurVal,1.0f,1.0f,0.,0.,false);
		
		glUseProgram(renderPrograms.uiProgram);
		
		glEnable(GL_DEPTH_TEST);
        ui.box(1.0f,0.025f,0.f + screenGapX,(1.f-0.025f),"",glm::vec4(1,1,1,1),+0.19f,false,false,0.91f,10000,colorData.panelColorSnd,0); //Upbar
		
        //Left column
		ui.box(0.025f,1.0f,(-1.f+0.025f) + screenGapX,0.f,"",glm::vec4(0.95f,0.95f,0.95f,1),+0.19f,false,false,0.92f,10000,colorData.panelColorSnd,0);



		glUseProgram(renderPrograms.uiProgram);


		
        float midPanelW = 0.15f;
		glEnable(GL_DEPTH_TEST);
        ui.box(midPanelW,1.0f,(-1.f+0.15f+0.05f) + screenGapX,0.f,"",glm::vec4(1,1,1,1.f),+0.19f,false,false,0.91f,10000,colorData.panelColorSnd,0);
        projectManager.elementPanelHover = ui.isMouseOnButton(window,midPanelW,1.0f,(-1.f+0.15f+0.05f),0.f,mouseXpos,mouseYpos,0);
		glDisable(GL_DEPTH_TEST);
		
		glUseProgram(renderPrograms.iconsProgram);
		//Close
        bool closeIconHover = ui.isMouseOnButton(window,0.01f,0.02f,(-1.f+0.025f),0.92f,mouseXpos,mouseYpos,false);
		ui.iconBox(0.01f,0.02f,(-1.f+0.025f)+ screenGapX,0.94f,0.92f,icons.Undo,closeIconHover,glm::vec4(0.06,0.12,0.15,1.0),glm::vec4(0.06,0.12,0.15,0.5));
        if(closeIconHover && firstClick)
            displayProjectFolderManager = false;
		
		
        bool texturesIconHover = ui.isMouseOnButton(window,0.01f,0.02f,(-1.f+0.025f),0.82f,mouseXpos,mouseYpos,false);
		ui.iconBox(0.01f,0.02f,(-1.f+0.025f) + screenGapX,0.82f,0.92f,icons.Texture,texturesIconHover,glm::vec4(0.06,0.12,0.15,1.0),glm::vec4(0.06,0.12,0.15,0.5));//Textures
        if(texturesIconHover && firstClick)
            projectFolderState = 0;

        bool tdmodelsIconHover = ui.isMouseOnButton(window,0.01f,0.02f,(-1.f+0.025f),0.74f,mouseXpos,mouseYpos,false);
		ui.iconBox(0.01f,0.02f,(-1.f+0.025f) + screenGapX,0.74f,0.92f,icons.TDModel,tdmodelsIconHover,glm::vec4(0.06,0.12,0.15,1.0),glm::vec4(0.06,0.12,0.15,0.5));//3DModels
        if(tdmodelsIconHover && firstClick)
            projectFolderState = 1;
		
        bool materialsIconHover = ui.isMouseOnButton(window,0.01f,0.02f,(-1.f+0.025f),0.66f,mouseXpos,mouseYpos,false);
		ui.iconBox(0.01f,0.02f,(-1.f+0.025f) + screenGapX,0.66f,0.92f,icons.Material,materialsIconHover,glm::vec4(0.06,0.12,0.15,1.0),glm::vec4(0.06,0.12,0.15,0.5));//Materials
        if(materialsIconHover && firstClick)
            projectFolderState = 2;
		
        bool nodesIconHover = ui.isMouseOnButton(window,0.01f,0.02f,(-1.f+0.025f),0.58f,mouseXpos,mouseYpos,false);
		ui.iconBox(0.01f,0.02f,(-1.f+0.025f) + screenGapX,0.58f,0.92f,icons.Mark,nodesIconHover,glm::vec4(0.06,0.12,0.15,1.0),glm::vec4(0.06,0.12,0.15,0.5));//Nodes
        if(nodesIconHover && firstClick)
            projectFolderState = 3;
		
        bool shadersIconHover = ui.isMouseOnButton(window,0.01f,0.02f,(-1.f+0.025f),0.50f,mouseXpos,mouseYpos,false);
		ui.iconBox(0.01f,0.02f,(-1.f+0.025f) + screenGapX,0.50f,0.92f,icons.BrushFolder,shadersIconHover,glm::vec4(0.06,0.12,0.15,1.0),glm::vec4(0.06,0.12,0.15,0.5));//Shaders
        if(shadersIconHover && firstClick)
            projectFolderState = 4;
        
        bool fontsIconHover = ui.isMouseOnButton(window,0.01f,0.02f,(-1.f+0.025f),0.42f,mouseXpos,mouseYpos,false);
		ui.iconBox(0.01f,0.02f,(-1.f+0.025f) + screenGapX,0.42f,0.92f,icons.JpgFile,fontsIconHover,glm::vec4(0.06,0.12,0.15,1.0),glm::vec4(0.06,0.12,0.15,0.5));//Shaders
        if(fontsIconHover && firstClick){
            projectFolderState = 5;

        }

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
        if(projectFolderState == 5)
            currentFolder = "Fonts";

        float rad = 0.07f;
        glm::vec2 pos = glm::vec2(-0.585f + screenGapX,0.85f);
        std::vector<float> folderVertices = { 
	        rad + pos.x,  rad*2 + pos.y, 0.0f,1,1,0,0,0,  // top right
	        rad + pos.x,  -rad*2 + pos.y, 0.0f,1,0,0,0,0,  // bottom right
	        -rad + pos.x,  rad*2 + pos.y, 0.0f,0,1,0,0,0,  // top left 
    
            rad + pos.x,  -rad*2 + pos.y, 0.0f,1,0,0,0,0,  // bottom right
	        -rad + pos.x,  -rad*2 + pos.y, 0.0f,0,0,0,0,0,  // bottom left
	        -rad + pos.x,  rad*2 + pos.y, 0.0f,0,1,0,0,0   // top left
	    };
        
		ui.box(midPanelW,0.025f, (-1.f+0.15f+0.05f) + screenGapX,0.925f,currentFolder,glm::vec4(0.92,0.92,0.92,1.0),+0.14f,false,false,0.92f,10000,colorData.panelColorSnd,0,glm::vec4(0.26,0.32,0.35,1.0),glm::vec4(0.26,0.32,0.35,1.0),0.00022f,true);
        

        drawTheFolder(projectPath + folderDistinguisher + currentFolder,screenGapX,mouseXpos,mouseYpos,window,midPanelW,icons,renderPrograms,firstClick,projectManager,firstClickR);
        glEnable(GL_DEPTH_TEST);

        if(projectFolderState == 0){

            glUseProgram(renderPrograms.iconsProgram);

            Texture texture;
            if(firstClick){
                glDeleteTextures(1,&fileTxtr);
                fileTxtr = texture.getTexture(selectedFile,0,0,0);
            }

            glUseProgram(renderPrograms.textureDisplayer);

            float rad = 0.3f;
            float posX = 0.18f;
            std::vector<float> renderVertices = { 
		        rad+posX,  rad*2, 0.0f,1,1,0,0,0,  // top right
		        rad+posX,  -rad*2, 0.0f,1,0,0,0,0,  // bottom right
		        -rad+posX,  rad*2, 0.0f,0,1,0,0,0,  // top left 
		        
                rad+posX,  -rad*2, 0.0f,1,0,0,0,0,  // bottom right
		        -rad+posX,  -rad*2, 0.0f,0,0,0,0,0,  // bottom left
		        -rad+posX,  rad*2, 0.0f,0,1,0,0,0   // top left
	        };
            glActiveTexture(GL_TEXTURE14);
            glBindTexture(GL_TEXTURE_2D,fileTxtr);
            gls.drawArrays(renderVertices,0);
    		gls.uniform1i(renderPrograms.textureDisplayer, "roundCor",0);

            glActiveTexture(GL_TEXTURE14);
            glBindTexture(GL_TEXTURE_2D,icons.TextureFolder);

        }        
        if(projectFolderState == 1){
            glUseProgram(renderPrograms.solidRenderer);
            glm::vec3 mostFar = fileModel.getMostFarVector();
            gls.uniform3fv(renderPrograms.solidRenderer,"mostFar",mostFar);            
	        gls.uniformMatrix4fv(renderPrograms.solidRenderer,"view",view);


            if(firstClick){
                fileModel.loadModel(selectedFile,true);
            }

            fileModel.Draw();

            

            glActiveTexture(GL_TEXTURE14);
            glBindTexture(GL_TEXTURE_2D,icons.TDModelFolder);

        }
        
        if(projectFolderState == 2){
            glActiveTexture(GL_TEXTURE14);
            glBindTexture(GL_TEXTURE_2D,icons.MaterialFolder);
        }
        if(projectFolderState == 3){

            if(firstClick){
                Load load;
                fileNode = load.createNode(selectedFile,"Node"); 
            }
            fileNode.height = ((fileNode.inputs.size() + fileNode.rangeBarCount + fileNode.outputs.size())/25.f + 0.07 * !fileNode.isMainOut) * 0.5f;
			fileNode.width = 0.12f * 0.5f;
			fileNode.positionX = 0.7f;

            ui.node(fileNode,renderPrograms,icons);
            glUseProgram(renderPrograms.uiProgram);
            ui.renderText(renderPrograms.uiProgram,fileNode.fragSource,-0.5f,+0.9f,0.00022f,colorData.panelColor,0.91f,false);
            glActiveTexture(GL_TEXTURE14);
            glBindTexture(GL_TEXTURE_2D,icons.NodeFolder);
        }
        if(projectFolderState == 4){
            glActiveTexture(GL_TEXTURE14);
            glBindTexture(GL_TEXTURE_2D,icons.ShaderFolder);
        }
        if(projectFolderState == 5){
            if(firstClick && selectedFile.size() > 3)
            {
                if((selectedFile[selectedFile.size()-1] == 'f' || selectedFile[selectedFile.size()-1] == 'F') && (selectedFile[selectedFile.size()-2] == 't' || selectedFile[selectedFile.size()-2] == 'T') && (selectedFile[selectedFile.size()-3] == 't' || selectedFile[selectedFile.size()-3] == 'T')){
                    Load load;
                    fileFont = load.createFont(selectedFile);
                }
            }
            glUseProgram(renderPrograms.uiProgram);
            glDisable(GL_DEPTH_TEST);
            ui.renderText(renderPrograms.uiProgram,"Carpe Diem",-0.35f,0.f,0.0022f,colorData.panelColor,0.91f,false,fileFont,0.5f);
            glEnable(GL_DEPTH_TEST);
            glActiveTexture(GL_TEXTURE14);
            glBindTexture(GL_TEXTURE_2D,icons.FontFolder);
        }

        glUseProgram(renderPrograms.renderTheTextureProgram);
        
        gls.drawArrays(folderVertices,0);

    if(showInfo){
        if(ui.isMouseOnButton(window,0.f,0.f,0.95f-screenGapX,0.3f,mouseXpos,mouseYpos,0)){
        }
        else{
           if(firstClick) 
                showInfo = false;
        }
        if(glfwGetKey(window,GLFW_KEY_ENTER) == GLFW_PRESS)
            showInfo = false;
        if(glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
            showInfo = false;

        ui.container(0.f,0.f,0.95f,0.3f,0.2f,colorData.panelColor,renderPrograms,icons.Circle,colorData.panelColor,0.f);
        glUseProgram(renderPrograms.uiProgram);


        //Version 1.4
		uint64_t h1 = 0xAB428C9F; 
        uint64_t h2 = 0xFF8A1C1C; 
        uint64_t h3 = 0x4B4B9AAA; 
        std::ifstream rf(projectPath + folderDistinguisher + util.getLastWordBySeparatingWithChar(projectPath,folderDistinguisher) + ".ligid", std::ios::out | std::ios::binary);
		
        std::string Cdt = ""; //Creation date
	    std::string Ldt = ""; //Last opening date
		
		if(rf) {
	
			uint64_t c1; 
        	uint64_t c2; 
        	uint64_t c3; 
        	rf.read(reinterpret_cast<char*>(&c1),sizeof(uint64_t));
        	rf.read(reinterpret_cast<char*>(&c2),sizeof(uint64_t));
        	rf.read(reinterpret_cast<char*>(&c3),sizeof(uint64_t));
	
        	if(c1 == h1 && c2 == h2 && c3 == h3){
				
				uint64_t timestrsize;
        		rf.read(reinterpret_cast<char*>(&timestrsize),sizeof(uint64_t));
        		for (size_t i = 0; i < timestrsize; i++)
        		{
					char c;
        		    rf.read(reinterpret_cast<char*>(&c),sizeof(char));
					Cdt.push_back(c);
        		}
				
				uint64_t timestrsize2;
        		rf.read(reinterpret_cast<char*>(&timestrsize2),sizeof(uint64_t));
        		for (size_t i = 0; i < timestrsize2; i++)
        		{
					char c;
        		    rf.read(reinterpret_cast<char*>(&c),sizeof(char));
					Ldt.push_back(c);
        		}
        	}
        }
        else{
            Cdt = "Can't locate the ligid file";
        }
        
        ui.renderText(renderPrograms.uiProgram,"Project path : ",-0.3f,0.18f,0.00022f,colorData.textColor,0.96f,0);
        ui.renderText(renderPrograms.uiProgram,std::filesystem::absolute(projectPath).string(),-0.3f,0.15f,0.00022f,colorData.textColor,0.96f,0);
        
        ui.renderText(renderPrograms.uiProgram,"Project size in bytes : ",-0.3f,0.08f,0.00022f,colorData.textColor,0.96f,0);
        ui.renderText(renderPrograms.uiProgram,std::to_string(util.getFolderSizeInBytes(projectPath)) + " bytes (" + std::to_string(util.getFolderSizeInBytes(projectPath)/1000000.f) + " MB)",-0.3f,0.05f,0.00022f,colorData.textColor,0.96f,0);
        
        ui.renderText(renderPrograms.uiProgram,"Project creation date : ",-0.3f,-0.02f,0.00022f,colorData.textColor,0.96f,0);
        ui.renderText(renderPrograms.uiProgram, Cdt,-0.3f,-0.05f,0.00022f,colorData.textColor,0.96f,0);
        
        ui.renderText(renderPrograms.uiProgram,"Project last opening date : ",-0.3f,-0.12f,0.00022f,colorData.textColor,0.96f,0);
        ui.renderText(renderPrograms.uiProgram, Ldt,-0.3f,-0.15f,0.00022f,colorData.textColor,0.96f,0);
    }
    glUseProgram(renderPrograms.uiProgram);


    bool saveButtonHover = false;
    if(ui.isMouseOnButton(window,0.025f,0.025f,-1.0f+0.08f,(1.f-0.025f),mouseXpos,mouseYpos,0))
        saveButtonHover = true;
    if(saveButtonHover && firstClick){
        ProjectFolder project;
        project.saveFolder(projectPath,albedoTextures,txtrRes,chosenSkyboxIndex,UIElements);
    }
    ui.box(0.03f,0.025f,-1.0f+0.05f+0.03f + screenGapX,(1.f-0.025f),"Save",glm::vec4(0.7f),0.02f,false,false,0.91f + saveButtonHover/1000.f,1000.f,colorData.buttonColor,0.f,glm::vec4(0.06,0.12,0.15,1.0),glm::vec4(0.06,0.12,0.15,1.0),0.00022f,true);
		
    ui.box(0.0005f,0.025f,(-1.0f+0.05f+0.03f)+0.03f + screenGapX,(1.f-0.025f),"",glm::vec4(0.7f),0.f,false,false,0.93f,1000.f,colorData.iconColor,0.f);
		
    bool saveAsButtonHover = false;
    if(ui.isMouseOnButton(window,0.04f,0.025f,(-1.0f+0.05f+0.03f)+0.03f + 0.04f,(1.f-0.025f),mouseXpos,mouseYpos,0))
        saveAsButtonHover = true;
    if(saveAsButtonHover && firstClick){
        ProjectFolder project;
        project.saveFolder(projectPath,albedoTextures,txtrRes,chosenSkyboxIndex,UIElements);
        std::string pathCheck = project.duplicateFolder(projectPath);
        if(pathCheck.size())
            projectPath = pathCheck;        
    }
    ui.box(0.04f,0.025f,(-1.0f+0.05f+0.03f)+0.03f + 0.04f + screenGapX,(1.f-0.025f),"Save As",glm::vec4(0.7f),0.035f,false,false,0.91f + saveAsButtonHover/1000.f,1000.f,colorData.buttonColor,0.f,glm::vec4(0.06,0.12,0.15,1.0),glm::vec4(0.06,0.12,0.15,1.0),0.00022f,true);
        
    ui.box(0.0005f,0.025f,((-1.0f+0.05f+0.03f)+0.03f + 0.04f) + 0.04f + screenGapX,(1.f-0.025f),"",glm::vec4(0.7f),0.f,false,false,0.93f,1000.f,colorData.iconColor,0.f);

    bool loadButtonHover = false;
    if(ui.isMouseOnButton(window,0.03f,0.025f,((-1.0f+0.05f+0.03f)+0.03f + 0.04f) + 0.04f + 0.03f,(1.f-0.025f),mouseXpos,mouseYpos,0))
        loadButtonHover = true;
    if(loadButtonHover && firstClick){
        ProjectFolder project;
        LigidPainter lp;
        if(lp.ligidMessageBox("Another project will be loaded!",-0.12f,"Unsaved data will be lost. Do you want to proceed?",-0.22f)){
            char const* lFilterPatterns[1] = { "*.ligid" };
			auto projectPathCheck = tinyfd_openFileDialog("Select LigidPainter Project File", "", 1, lFilterPatterns, "", false);
            
            if(projectPathCheck){
                project.readFolder(projectPathCheck,materials,appNodes,addNodeContexMenu,model,UIElements,albedoTextures,fonts,chosenSkyboxIndex);
                projectPath = projectPathCheck;
            }
        }
    }
    ui.box(0.03f,0.025f,((-1.0f+0.05f+0.03f)+0.03f + 0.04f) + 0.04f + 0.03f + screenGapX,(1.f-0.025f),"Load",glm::vec4(0.7f),0.02f,false,false,0.91f + loadButtonHover/1000.f,1000.f,colorData.buttonColor,0.f,glm::vec4(0.06,0.12,0.15,1.0),glm::vec4(0.06,0.12,0.15,1.0),0.00022f,true);
        
    ui.box(0.0005f,0.025f,(((-1.0f+0.05f+0.03f)+0.03f + 0.04f) + 0.04f + 0.03f) + 0.03f + screenGapX,(1.f-0.025f),"",glm::vec4(0.7f),0.f,false,false,0.93f,1000.f,colorData.iconColor,0.f);
        
    bool infoButtonHover = false;
    if(ui.isMouseOnButton(window,0.03f,0.025f,(((-1.0f+0.05f+0.03f)+0.03f + 0.04f) + 0.04f + 0.03f) + 0.03f + 0.03f,(1.f-0.025f),mouseXpos,mouseYpos,0))
            infoButtonHover = true;
    if(infoButtonHover && firstClick)
            showInfo = !showInfo;
    ui.box(0.03f,0.025f,(((-1.0f+0.05f+0.03f)+0.03f + 0.04f) + 0.04f + 0.03f) + 0.03f + 0.03f + screenGapX,(1.f-0.025f),"Info",glm::vec4(0.7f),0.018f,false,false,0.91f + infoButtonHover/1000.f,1000.f,colorData.buttonColor,0.f,glm::vec4(0.06,0.12,0.15,1.0),glm::vec4(0.06,0.12,0.15,1.0),0.00022f,true);

    ui.box(0.0005f,0.025f,(((-1.0f+0.05f+0.03f)+0.03f + 0.04f) + 0.04f + 0.03f) + 0.03f*3 + screenGapX,(1.f-0.025f),"",glm::vec4(0.7f),0.f,false,false,0.93f,1000.f,colorData.iconColor,0.f);
}