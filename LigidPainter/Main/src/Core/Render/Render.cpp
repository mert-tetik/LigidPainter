#include<iostream>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/Model/model.h"

#include "Core/Render/Render.h"
#include "Core/LigidPainter.h"
#include "Core/UserInterface.h"
#include "Core/Utilities.h"
#include "Core/gl.h"
#include "Core/Texture.h"

#include "stb_image.h"
#include "stb_image_write.h"

#include "tinyfiledialogs.h"

struct UndoActions
{
	GLubyte* undoTextures; 
	unsigned int activeMaterial; 
};

std::vector<UndoActions> undoList; 



int currentMaterialIndex = 0;

Programs renderPrograms;

int renderMaxScreenWidth;
int renderMaxScreenHeight;

//--------------------RENDER UI --------------------\\






float orgTextureDemonstratorWidth = 0.4f;
float orgTextureDemonstratorHeight = 0.8f;


unsigned int currentBrushMaskTexture;


SaturationValShaderData saturationValShaderData;
glm::vec3 hueVal;		




void Render::renderSkyBox(SkyBoxShaderData data) {
	GlSet gls;

	gls.useSkyBoxShader(renderPrograms.skyboxProgram,data);
	
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);

	std::vector<float> skyboxVertices = {
	    // positions          
	    -1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f,  1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f,  1.0f ,0,0,  0,0,0,
	    -1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f,  1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f,  1.0f ,0,0,  0,0,0,
	    -1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	    -1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	    -1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f,  1.0f ,0,0,  0,0,0
	};

	gls.drawArrays(skyboxVertices,false);
	glDepthFunc(GL_LESS);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
	
	glUseProgram(renderPrograms.uiProgram);
}
glm::mat4 Render::setMatrices() {
    GlSet gl;
	glm::mat4 textProjection = glm::ortho(0.0f, 2.0f, -1.0f, 1.0f);
	glUseProgram(renderPrograms.iconsProgram);
	gl.uniformMatrix4fv(renderPrograms.iconsProgram, "Projection", textProjection);
	glUseProgram(renderPrograms.uiProgram);
	gl.uniformMatrix4fv(renderPrograms.uiProgram, "TextProjection", textProjection);

	glm::mat4 renderTextureProjection = glm::ortho(0.0f, 1.77777777778f, 0.0f, 1.0f);//1920 - 1080 -> 1.77777777778 - 1
	glUseProgram(renderPrograms.blurProgram);
	gl.uniformMatrix4fv(renderPrograms.blurProgram, "renderTextureProjection", renderTextureProjection);
	glUseProgram(renderPrograms.outProgram);
	gl.uniformMatrix4fv(renderPrograms.outProgram, "renderTextureProjection", renderTextureProjection);
	glUseProgram(renderPrograms.uiProgram);
	gl.uniformMatrix4fv(renderPrograms.uiProgram, "renderTextureProjection", renderTextureProjection);

	glm::mat4 model = glm::mat4(1.0f);
	gl.uniformMatrix4fv(renderPrograms.uiProgram, "model", model);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 1000.0f);
	glUseProgram(renderPrograms.uiProgram);
	gl.uniformMatrix4fv(renderPrograms.uiProgram, "projection",projection);

	return projection;
	//pd.viewMat = view;
}
ViewUpdateData Render::updateViewMatrix(glm::vec3 cameraPos, glm::vec3 originPos,bool mirrorX,bool mirrorY,bool mirrorZ) {
	glm::mat4 view;
    GlSet gl;
	view = glm::lookAt(cameraPos, originPos, glm::vec3(0.0, 1.0, 0.0)); 

	glm::vec3 mirrorVec = glm::vec3(1.0f - (int)mirrorX*2, 1.0f -(int)mirrorY * 2, 1.0f - (int)mirrorZ * 2);
	glm::mat4 mirroredView;
	mirroredView = glm::lookAt(cameraPos * mirrorVec, originPos * mirrorVec, glm::vec3(0.0, 1.0, 0.0));

	glUseProgram(renderPrograms.uiProgram);
	gl.uniformMatrix4fv(renderPrograms.uiProgram, "view", view);

	gl.uniform3fv(renderPrograms.uiProgram, "viewPos", cameraPos);

	glm::vec3 mirroredCameraPos = cameraPos * mirrorVec;

	gl.uniform3fv(renderPrograms.uiProgram, "mirroredViewPos", mirroredCameraPos);

	gl.uniformMatrix4fv(renderPrograms.uiProgram, "mirroredView", mirroredView);

	ViewUpdateData viewUpdateData;

	viewUpdateData.cameraPos = cameraPos;
	viewUpdateData.mirroredCameraPos = mirroredCameraPos;
	viewUpdateData.mirroredView = mirroredView;
	viewUpdateData.view = view;

	return viewUpdateData;
}
void Render::getUnprojection(glm::vec3 vPos, glm::vec3 cameraPos, glm::vec3 originPos) { //Not used
	/*GlSet glset;
	ProjectionData pd = glset.setMatrices(cameraPos, originPos);
	return glm::project(vPos, pd.modelMat * pd.viewMat, pd.projMat, glm::vec4(0, 0, 1920, 1080));*/
}

void Render::drawLightObject(glm::vec3 lightPos) {
	// glm::mat4 model = glm::mat4(1.0f);
	// model = glm::mat4(1.0f);
	// model = glm::translate(model, lightPos);
	// model = glm::scale(model, glm::vec3(0.2f));

	// int lightColorLoc = glGetUniformLocation(renderPrograms.uiProgram, "lightColor");
	// glUniform3f(lightColorLoc, 10.0f, 10.0f, 10.0f);
	// int isLightSourceLoc = glGetUniformLocation(renderPrograms.uiProgram, "isLightSource");
	// glUniform1i(isLightSourceLoc, 1);
	// int modelLoc = glGetUniformLocation(renderPrograms.uiProgram, "model");
	// glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	// glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Render::getDepthTexture(unsigned int FBOScreen,  int screenSizeX,  int screenSizeY,ScreenDepthShaderData screenDepthShaderData,Model &model,bool renderDefault,vector<unsigned int> &albedoTextures) {
	Texture txtr;
    GlSet gl;

	gl.viewport(1920, 1080);
	gl.bindFramebuffer(FBOScreen);
	
	screenDepthShaderData.renderMirrored = 0;
	gl.useScreenDepthShader(renderPrograms.screenDepthProgram, screenDepthShaderData);
	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	model.Draw(currentMaterialIndex,renderPrograms.PBRProgram,false,albedoTextures);

	GLubyte* screen = txtr.getTextureFromProgram(GL_TEXTURE5, 1920, 1080, 3);
	gl.activeTexture(GL_TEXTURE9);
	gl.texImage(screen, 1920, 1080, GL_RGB);
	gl.generateMipmap();
	delete[]screen;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//Mirrored
	screenDepthShaderData.renderMirrored = 1;
	gl.useScreenDepthShader(renderPrograms.screenDepthProgram, screenDepthShaderData);

	
	model.Draw(currentMaterialIndex,renderPrograms.PBRProgram,false,albedoTextures);

	GLubyte* screenMirrored = txtr.getTextureFromProgram(GL_TEXTURE5, 1920, 1080, 3);
	gl.activeTexture(GL_TEXTURE8);
	gl.texImage(screenMirrored, 1920, 1080, GL_RGB);
	gl.generateMipmap();
	delete[] screenMirrored;

	glUseProgram(renderPrograms.uiProgram);
	
	glViewport(-(renderMaxScreenWidth - screenSizeX)/2, -(renderMaxScreenHeight - screenSizeY), renderMaxScreenWidth, renderMaxScreenHeight);

	gl.bindFramebuffer(0);
}

//------------CtrlZ------------
bool doCtrlZ;
void ctrlZCheck(GLFWwindow* window,std::vector<unsigned int> &albedoTextures) {
	Texture txtr;
	GlSet glset;

	

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && doCtrlZ && undoList.size() != 0) { //MAX 20
		
		//Refresh the screen mask texture (Prevent bugs where might be accur trying undo while in the middle of painting)
		txtr.refreshScreenDrawingTexture();
		
		//Bind the related texture
		glset.activeTexture(GL_TEXTURE0);
		currentMaterialIndex = undoList[undoList.size() - 1].activeMaterial;
		glset.bindTexture(albedoTextures[currentMaterialIndex]);

		//Change the texture to the last texture
		glset.texImage(undoList[undoList.size() - 1].undoTextures, 1080, 1080, GL_RGB);
		glset.generateMipmap();

		//Delete the texture
		GLubyte* undoTexture = undoList[undoList.size() - 1].undoTextures;
		delete[] undoTexture;

		//Remove the last element
		undoList.pop_back();
		doCtrlZ = false;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE || glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE) {
		doCtrlZ = true   ;
	}
}
//------------CtrlZ------------

void drawBrushIndicator(float distanceX,int screenWidth,int screenHeight,double mouseXpos,double mouseYpos,glm::vec3 color) {

	float sizeX = distanceX; //Match the size of the window
	float screenGapX = ((float)renderMaxScreenWidth - (float)screenWidth)/(((float)renderMaxScreenWidth)/2.0f)/2.0f; 
	
	GlSet glset;
	glset.uniform1i(renderPrograms.uiProgram, "drawBrushIndicator", 1); //TODO : Create shader for brush indicator
	std::vector<float> paintingSquare{
		// first triangle
		( sizeX / renderMaxScreenWidth / 1.0f + (float)mouseXpos / renderMaxScreenWidth / 0.5f - 1.0f)+screenGapX,  sizeX / renderMaxScreenHeight / 1.0f - (float)mouseYpos / renderMaxScreenHeight / 0.5f + 1.0f , 1.0f,1,1,0,0,0,  // top right
		( sizeX / renderMaxScreenWidth / 1.0f + (float)mouseXpos / renderMaxScreenWidth / 0.5f - 1.0f)+screenGapX, -sizeX / renderMaxScreenHeight / 1.0f - (float)mouseYpos / renderMaxScreenHeight / 0.5f + 1.0f , 1.0f,1,0,0,0,0,  // bottom right
		(-sizeX / renderMaxScreenWidth / 1.0f + (float)mouseXpos / renderMaxScreenWidth / 0.5f - 1.0f)+screenGapX,  sizeX / renderMaxScreenHeight / 1.0f - (float)mouseYpos / renderMaxScreenHeight / 0.5f + 1.0f , 1.0f,0,1,0,0,0,  // top left 
		( sizeX / renderMaxScreenWidth / 1.0f + (float)mouseXpos / renderMaxScreenWidth / 0.5f - 1.0f)+screenGapX, -sizeX / renderMaxScreenHeight / 1.0f - (float)mouseYpos / renderMaxScreenHeight / 0.5f + 1.0f , 1.0f,1,0,0,0,0,  // bottom right
		(-sizeX / renderMaxScreenWidth / 1.0f + (float)mouseXpos / renderMaxScreenWidth / 0.5f - 1.0f)+screenGapX, -sizeX / renderMaxScreenHeight / 1.0f - (float)mouseYpos / renderMaxScreenHeight / 0.5f + 1.0f , 1.0f,0,0,0,0,0,  // bottom left
		(-sizeX / renderMaxScreenWidth / 1.0f + (float)mouseXpos / renderMaxScreenWidth / 0.5f - 1.0f)+screenGapX,  sizeX / renderMaxScreenHeight / 1.0f - (float)mouseYpos / renderMaxScreenHeight / 0.5f + 1.0f , 1.0f,0,1,0,0,0  // top left
	};
	glset.uiDataToShaders(color/glm::vec3(255.0f));
	glset.uniform1f(renderPrograms.uiProgram, "uiOpacity", 0.2f);
	glset.drawArrays(paintingSquare, false);
	glset.uniform1f(renderPrograms.uiProgram, "uiOpacity", 0.5f);

	glset.uniform1i(renderPrograms.uiProgram, "drawBrushIndicator", 0);
}

void drawAxisPointer(AxisPointerShaderData axisPointerShaderData) {
	GlSet glset;
		//Axis Pointer
		std::vector<float>axisPointer{
		0.0f, -10000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //Y
		0.0f, 10000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

		-10000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //X
		10000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 0.0f, -10000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //Z
		0.0f, 0.0f, 10000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	};
	glset.useAxisPointerShader(renderPrograms.axisPointerProgram,axisPointerShaderData);
	
	glset.blendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
	glset.drawArrays(axisPointer, true);
	glset.blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(renderPrograms.uiProgram);
}


void Render::exportTexture(bool JPG,bool PNG,const char* exportPath,const char* exportFileName,vector<unsigned int> &albedoTextures){

	//Create the export folder
	std::string exportPathStr = exportPath;

	if(exportFileName[0] != '/'){
		exportPathStr.append("/");
	}
	
	exportPathStr.append(exportFileName);

	std::filesystem::create_directories(exportPathStr.c_str());

    
	GlSet gl;
	Texture txtr;

	
	
	for (size_t i = 0; i < albedoTextures.size(); i++) //Export all the albedo textures
	{
		//Give a number to the texture name
		std::string exportFileNameStr = exportFileName;
		exportFileNameStr.append("_Albedo_");
		exportFileNameStr.append(to_string(i));

		//Bind the related texture
		gl.activeTexture(GL_TEXTURE0);
		gl.bindTexture(albedoTextures[i]);

		//Get the texture array
    	GLubyte* exportTxtr = txtr.getTextureFromProgram(GL_TEXTURE0,1080,1080,3);

		//Export
		if (JPG) {
			txtr.downloadTexture(exportPathStr.c_str(), exportFileNameStr.c_str(), 0, 1080, 1080, exportTxtr, 3);
		}
		else if (PNG) {
			txtr.downloadTexture(exportPathStr.c_str(), exportFileNameStr.c_str(), 1, 1080, 1080, exportTxtr, 3);
		}

		//Delete the array after exporting
    	delete[] exportTxtr;
	}
}

void Render::renderTexture(std::vector<float>& vertices,unsigned int width, unsigned int height,unsigned int texture,unsigned int channels,Model &model,bool useModel,vector<unsigned int> &albedoTextures){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GlSet gl;

	if(useModel)
		model.Draw(currentMaterialIndex,renderPrograms.PBRProgram,false,albedoTextures);
	else
		gl.drawArrays(vertices, false); //Render Model

	GLubyte* renderedTexture = new GLubyte[width * 1080 * 3 * sizeof(GLubyte)];
	glReadPixels(0, 0, width, height, channels, GL_UNSIGNED_BYTE, renderedTexture);

	gl.activeTexture(texture);
	gl.texImage(renderedTexture, width, height, channels);
	gl.generateMipmap();
	delete[]renderedTexture;
}

void Render::renderTextures(unsigned int FBOScreen, bool exportImage, bool JPG, bool PNG, const char* exportPath, int screenSizeX,  int screenSizeY,const char* exportFileName, OutShaderData outShaderData,Model &model,bool renderDefault,vector<unsigned int> &albedoTextures,bool paintOut,bool isRenderTexture,bool paintRender,bool firstPaint) {
	int maxHistoryHold = 20;
	
	if(firstPaint){
		//UNDO
		Texture txtr;

		GLubyte* originalImage = txtr.getTextureFromProgram(GL_TEXTURE0, 1080, 1080, 3);

		//Send the texture to the undoList before processing the texture (will be used for ctrl z)
		UndoActions undoAct;
		undoAct.activeMaterial = currentMaterialIndex;
		undoAct.undoTextures = originalImage; 
		undoList.push_back(undoAct);


    	//Delete the first element from undoList if undoList's count is greated than max history holding value which is 20
		if (undoList.size() > maxHistoryHold){
			//Delete the texture
			GLubyte* undoTexture = undoList[0].undoTextures;
			delete[] undoTexture;

			//Remove the element
			undoList.erase(undoList.begin());
		}
	}

	if(isRenderTexture){

		std::vector<float> renderVertices = { //Render backside of the uv
		// first triangle
		 1.0f,  1.0f, 0.0f,1,1,0,0,0,  // top right
		 1.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
		 0.0f,  1.0f, 0.0f,0,1,0,0,0,  // top left 
		// second triangle	  ,0,0,0,
		 1.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
		 0.0f,  0.0f, 0.0f,0,0,0,0,0,  // bottom left
		 0.0f,  1.0f, 0.0f,0,1,0,0,0   // top left
		};
		Texture txtr;

		


    	GlSet gl;
		//Setup
		glm::mat4 projection = glm::ortho(0.0f, 1.77777777778f, 0.0f, 1.0f);
		outShaderData.renderTextureProjection = projection;
		gl.useOutShader(renderPrograms.outProgram, outShaderData);


		gl.uniform1i(renderPrograms.outProgram, "isTwoDimensional", 0);
		gl.viewport(1920, 1080);
		gl.bindFramebuffer(FBOScreen);
		glClearColor(0,0,0,1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Setup



		gl.uniform1i(renderPrograms.outProgram,"renderPaintedTxtrMask",1);


		//Prevent uv stacking
		model.Draw(currentMaterialIndex,renderPrograms.PBRProgram,false,albedoTextures);

		GLubyte* paintedMask = new GLubyte[1080 * 1080 * 3 * sizeof(GLubyte)];
		glReadPixels(0, 0, 1080, 1080, GL_RGB, GL_UNSIGNED_BYTE, paintedMask);
		gl.activeTexture(GL_TEXTURE11);
		gl.texImage(paintedMask, 1080, 1080, GL_RGB);
		gl.generateMipmap();
		delete[]paintedMask;


		UserInterface ui;
		ui.setViewportBgColor();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		gl.uniform1i(renderPrograms.outProgram,"renderPaintedTxtrMask",0);



		//Render painted image
		model.Draw(currentMaterialIndex,renderPrograms.PBRProgram,false,albedoTextures);

		if (!paintOut)
			gl.drawArrays(renderVertices, false);

		GLubyte* renderedImage = new GLubyte[1080 * 1080 * 3 * sizeof(GLubyte)];
		glReadPixels(0, 0, 1080, 1080, GL_RGB, GL_UNSIGNED_BYTE, renderedImage);
		gl.activeTexture(GL_TEXTURE0);
		gl.texImage(renderedImage, 1080, 1080, GL_RGB);
		gl.generateMipmap();
		delete[]renderedImage;
		//Render painted image

		txtr.refreshScreenDrawingTexture();

		//Render uv mask
		gl.uniform1i(renderPrograms.outProgram, "whiteRendering", 1);
		renderTexture(renderVertices,1080, 1080,GL_TEXTURE7,GL_RGB,model,true,albedoTextures);
		gl.uniform1i(renderPrograms.outProgram, "whiteRendering", 0);
		//Render uv mask

		//interpret the albedo with ui mask texture
		gl.uniform1i(renderPrograms.outProgram, "interpretWithUvMask", 1);
		renderTexture(renderVertices,1080, 1080,GL_TEXTURE0,GL_RGB,model, false,albedoTextures);//Render enlarged texture
		gl.uniform1i(renderPrograms.outProgram, "interpretWithUvMask", 0);
		//interpret the albedo with ui mask texture

		//Download enlarged texture
		if (exportImage) {
    	    exportTexture(JPG,PNG,exportPath,exportFileName,albedoTextures);
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Finish
		gl.bindFramebuffer(0);
		glViewport(-(renderMaxScreenWidth - screenSizeX)/2, -(renderMaxScreenHeight - screenSizeY), renderMaxScreenWidth, renderMaxScreenHeight);

		//Finish
		glUseProgram(renderPrograms.uiProgram);
	}
	else if(paintRender){
		GlSet gl;
		UserInterface ui;
		Texture txtr;
		
		glm::mat4 projection = glm::ortho(0.0f, 1.77777777778f, 0.0f, 1.0f);
		outShaderData.renderTextureProjection = projection;
		gl.useOutShader(renderPrograms.outProgram, outShaderData);

		gl.uniform1i(renderPrograms.outProgram, "isTwoDimensional", 0);
		gl.viewport(1920, 1080);
		gl.bindFramebuffer(FBOScreen);

		ui.setViewportBgColor();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		gl.uniform1i(renderPrograms.outProgram,"renderPaintedTxtrMask",0);
		//Render painted image
		model.Draw(currentMaterialIndex,renderPrograms.PBRProgram,false,albedoTextures);

		GLubyte* renderedImage = new GLubyte[1080 * 1080 * 3 * sizeof(GLubyte)];
		glReadPixels(0, 0, 1080, 1080, GL_RGB, GL_UNSIGNED_BYTE, renderedImage);
		gl.activeTexture(GL_TEXTURE0);
		gl.texImage(renderedImage, 1080, 1080, GL_RGB);
		gl.generateMipmap();
		delete[]renderedImage;
		gl.bindFramebuffer(0);
		txtr.refreshScreenDrawingTexture();

		glViewport(-(renderMaxScreenWidth - screenSizeX)/2, -(renderMaxScreenHeight - screenSizeY), renderMaxScreenWidth, renderMaxScreenHeight);
	}
	

}
glm::vec3 getScreenHoverPixel(double mouseXpos,double mouseYpos, int screenSizeY){
    glm::vec3 screenHoverPixel;
	GLubyte* screenPixel = new GLubyte[1 * 1 * 3];//Color val
	glReadPixels(mouseXpos,(int)((double)screenSizeY - mouseYpos), 1, 1, GL_RGB, GL_UNSIGNED_BYTE, screenPixel);//Read the pixel from the framebuffer
	screenHoverPixel.r = screenPixel[0];
	screenHoverPixel.g = screenPixel[1];
	screenHoverPixel.b = screenPixel[2];
	delete[] screenPixel;  
    return screenHoverPixel;
}
glm::vec3 Render::getColorBoxValue(unsigned int FBOScreen,float colorBoxPickerValue_x, float colorBoxPickerValue_y,  int screenSizeX,  int screenSizeY) {
	std::vector<float> colorBox = { //Render color box into the screen
	// first triangle
	 0.0f,  1.0f, 0.0f,1,1,1,1,1,  // top right
	 0.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
	 1.0f,  1.0f, 0.0f,0,1,0,0,0,  // top left 
	// second triangle	  ,0,0,0,
	 0.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right7
	 1.0f,  0.0f, 0.0f,0,0,0,0,0,  // bottom left
	 1.0f,  1.0f, 0.0f,0,1,0,0,0   // top left
	};

    GlSet gl;
	glm::mat4 projection = glm::ortho(0.0f, 1.77777777778f, 0.0f, 1.0f);
	saturationValShaderData.renderTextureProjection = projection;
	saturationValShaderData.boxColor = hueVal / 255.0f;
    gl.useSaturationValBoxShader(renderPrograms.saturationValBoxProgram,saturationValShaderData);

	//Setup
	gl.viewport(1920, 1080);
	gl.bindFramebuffer(FBOScreen);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Setup

	//Render color box
	GLubyte* colorBoxPixel = new GLubyte[1 * 1 * 3];//Color val
	gl.drawArrays(colorBox, false); //Render Model
	glReadPixels(1080.0f - ((colorBoxPickerValue_x * -1.0f + 0.1f) * 5.0f * 1080.0f), (colorBoxPickerValue_y + 0.2f) * 2.5f * 1080.0f, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, colorBoxPixel);
	//Render color box

	//Finish
	gl.bindFramebuffer(0);
	glViewport(-(renderMaxScreenWidth - screenSizeX)/2, -(renderMaxScreenHeight - screenSizeY), renderMaxScreenWidth, renderMaxScreenHeight);

	glUseProgram(renderPrograms.uiProgram);


	//Get color value to the color vec
    glm::vec3 colorBoxPixelVal = glm::vec3(0);
	colorBoxPixelVal.r = colorBoxPixel[0];
	colorBoxPixelVal.g = colorBoxPixel[1];
	colorBoxPixelVal.b = colorBoxPixel[2];
	delete[]colorBoxPixel;
    return colorBoxPixelVal;
	//Finish
}

int renderDepthCounter = 0;
glm::vec3 colorBoxVal = glm::vec3(0);


bool lastRenderSphere = false;
bool lastRenderPlane = false;
bool renderDefault = false;


RenderOutData uiOut;


glm::vec3 screenHoverPixel;

int paintRenderCounter = 0;
RenderOutData Render::render(RenderData &renderData, std::vector<float>& vertices, unsigned int FBOScreen, PanelData &panelData, ExportData &exportData,UiData &uidata,float textureDemonstratorButtonPosX,float textureDemonstratorButtonPosY, bool textureDemonstratorButtonPressClicked,float textureDemonstratorWidth, float textureDemonstratorHeight,bool textureDemonstratorBoundariesPressed,Icons &icons,const char* maskTextureFile,int paintingFillNumericModifierVal,float maskPanelSliderValue,std::vector<unsigned int> &maskTextures,std::string &colorpickerHexVal,bool colorpickerHexValTextboxValChanged,bool colorBoxValChanged,bool renderPlane,bool renderSphere,PBRShaderData &pbrShaderData,SkyBoxShaderData &skyBoxShaderData,float brushBlurVal,ScreenDepthShaderData &screenDepthShaderData,AxisPointerShaderData &axisPointerShaderData,OutShaderData &outShaderData,Model &model,vector<unsigned int> &albedoTextures, bool updateHueVal,bool paintingDropperPressed, bool paintRender) {
	GlSet gls;
	ColorData colorData;
	Utilities util;

	colorBoxVal = util.hexToRGBConverter(colorpickerHexVal);

	if(model.meshes.size()-1 < currentMaterialIndex){
		currentMaterialIndex = 0; 
	}
	
	if(textureDemonstratorBoundariesPressed){
		orgTextureDemonstratorWidth = textureDemonstratorWidth;
		orgTextureDemonstratorHeight = textureDemonstratorHeight;
	}


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear before rendering


	//Get screen and mouse info
	int screenSizeX;
	int screenSizeY;
	glfwGetWindowSize(renderData.window, &screenSizeX, &screenSizeY);
	double mouseXpos;
	double mouseYpos;
	glfwGetCursorPos(renderData.window, &mouseXpos, &mouseYpos);


	//Render depth once painting started
	if (renderData.paintingMode) { 
		renderDepthCounter++;
	}
	else {
		renderDepthCounter = 0;
	}
	if (renderDepthCounter == 1) {//Get depth texture
		getDepthTexture(FBOScreen,screenSizeX,screenSizeY,screenDepthShaderData,model,renderDefault,albedoTextures);
	}


	bool isRenderTexture = (renderData.cameraPosChanged && renderData.paintingMode) || exportData.exportImage || uidata.addImageButtonPressed ||(glfwGetMouseButton(renderData.window, 0) == GLFW_RELEASE && renderData.paintingMode); //addImageButtonPressed = albedo texture changed
	
	bool firstPaint = false; //Take the texture to the undo list
	
	if(paintRender)
		paintRenderCounter++;
	if(isRenderTexture)
		paintRenderCounter = 0;
	
	if(paintRenderCounter == 1)
		firstPaint = true;


	if (isRenderTexture || paintRender) { //colorboxvalchanged has to trigger paintingmode to false
		
		if(exportData.exportImage){
			int lastMaterialIndex = currentMaterialIndex;
			currentMaterialIndex = 0;		
			for (size_t i = 0; i < albedoTextures.size(); i++) //Paint outside of the uv's of the albedo textures in one color before exporting
			{
				//Bind the related texture
				gls.activeTexture(GL_TEXTURE0);
				gls.bindTexture(albedoTextures[i]);

				//Render the texture 	
				renderTextures(FBOScreen, (i == albedoTextures.size()-1) ,uidata.exportExtJPGCheckBoxPressed, uidata.exportExtPNGCheckBoxPressed,exportData.path,screenSizeX, screenSizeY,exportData.fileName,outShaderData,model,renderDefault,albedoTextures,true,isRenderTexture,paintRender,firstPaint);

				//Render material by material
				currentMaterialIndex++;
			}

			//Set everything back to normal
			currentMaterialIndex = lastMaterialIndex;
			gls.activeTexture(GL_TEXTURE0);
			gls.bindTexture(albedoTextures[currentMaterialIndex]);

		}
		else
			renderTextures(FBOScreen,exportData.exportImage,uidata.exportExtJPGCheckBoxPressed, uidata.exportExtPNGCheckBoxPressed,exportData.path,screenSizeX, screenSizeY,exportData.fileName,outShaderData,model,renderDefault,albedoTextures,false,isRenderTexture,paintRender,firstPaint);
	}


	renderSkyBox(skyBoxShaderData);
	renderModel(renderData.backfaceCulling,pbrShaderData,model,renderDefault,albedoTextures,renderPrograms,currentMaterialIndex);
	drawAxisPointer(axisPointerShaderData);

	uiOut = renderUi(panelData, uidata, renderData, FBOScreen, renderData.brushBlurRangeBarValue,renderData.brushRotationRangeBarValue, renderData.brushOpacityRangeBarValue, renderData.brushSpacingRangeBarValue,textureDemonstratorButtonPosX,textureDemonstratorButtonPosY,textureDemonstratorButtonPressClicked,icons,colorBoxVal,maskTextureFile,paintingFillNumericModifierVal,exportData.fileName, maskPanelSliderValue,maskTextures,mouseXpos,mouseYpos,screenSizeX,screenSizeY,colorpickerHexVal,renderData.brushBorderRangeBarValue,brushBlurVal,outShaderData,model,albedoTextures,updateHueVal,renderPrograms,currentMaterialIndex,renderMaxScreenWidth,orgTextureDemonstratorWidth, orgTextureDemonstratorHeight, saturationValShaderData,hueVal,currentBrushMaskTexture);

	if (colorBoxValChanged && !colorpickerHexValTextboxValChanged) { //Get value of color box
		colorBoxVal = getColorBoxValue(FBOScreen, renderData.colorBoxPickerValue_x, renderData.colorBoxPickerValue_y,screenSizeX, screenSizeY);
	}
	ctrlZCheck(renderData.window,albedoTextures);



	if(paintingDropperPressed)
		screenHoverPixel = getScreenHoverPixel(mouseXpos,mouseYpos,screenSizeY);

	if(renderData.doPainting)
		drawBrushIndicator(renderData.brushSizeIndicator, screenSizeX, screenSizeY, mouseXpos, mouseYpos, colorBoxVal);
 
	RenderOutData renderOut;
	renderOut.mouseHoverPixel = screenHoverPixel;
	renderOut.maskPanelMaskClicked = uiOut.maskPanelMaskClicked;
	renderOut.maskPanelMaskHover = uiOut.maskPanelMaskHover;
	renderOut.texturePanelButtonHover = uiOut.texturePanelButtonHover;
	renderOut.texturePanelButtonClicked = uiOut.texturePanelButtonClicked;
	renderOut.currentBrushMaskTxtr = uiOut.currentBrushMaskTxtr;
	renderOut.colorpickerHexVal = util.rgbToHexGenerator(colorBoxVal);
	renderOut.colorBoxVal = colorBoxVal;
	return renderOut;
}
void Render::sendProgramsToRender(Programs apprenderPrograms){
	renderPrograms = apprenderPrograms;
}
void Render::sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight){
	renderMaxScreenHeight = maxScreenHeight;
	renderMaxScreenWidth = maxScreenWidth;
}