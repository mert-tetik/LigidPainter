#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "LigidPainter.h"
#include "UserInterface.h"
#include "gl.h"
#include "Texture.h"
#include <vector>
#include "stb_image.h"
#include "stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

using namespace std;

Programs txtrPrograms;

unsigned int Texture::getTexture(std::string path, unsigned int desiredWidth, unsigned int desiredHeight,bool update) {
	//Leave desiredWidth 0 if no resize wanted
	bool applyResize;
	applyResize = desiredWidth;

	unsigned int textureID;

	GlSet glset;

	if(!update){
		glset.genTextures(textureID);
		glset.bindTexture(textureID);
	}
	else{
		textureID = 0;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	int width, height, nrChannels;

	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 3);
	GLubyte* resizedPixelsX = NULL;
	if (applyResize) {
		resizedPixelsX = new GLubyte[desiredWidth * desiredHeight * 3];
		stbir_resize_uint8(data, width, height, 0, resizedPixelsX, desiredWidth, desiredHeight, 0, 3);
	}

	if (data != NULL)
	{
		if(applyResize)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, desiredWidth, desiredHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, resizedPixelsX);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glset.generateMipmap();
	}
	else
	{
		const char* reason = "[unknown reason]";
		if (stbi_failure_reason())
		{
			reason = stbi_failure_reason();
		}
		std::cout << "Failed to load texture!" << " Reason : " << reason<< std::endl;
	}

	stbi_image_free(data);
	delete(resizedPixelsX);
	return textureID;
}

void Texture::downloadTexture(const char* path, const char* name, int format, int width, int height, GLubyte* pixels, int channels) {
	//0 -> jpg | 1 -> png
	stbi_flip_vertically_on_write(true);
	string stbName = name;
	string stbPath;
	string stbPathx = path;
	if (format == 0) {
		stbPath = stbPathx + "\\" + stbName + ".jpg";
		stbi_write_jpg(stbPath.c_str(), width, height, channels, pixels, width *channels);
	}
	else if (format == 1) {
		stbPath = stbPathx + "\\" + stbName + ".png";
		stbi_write_png(stbPath.c_str(), width, height, channels, pixels, width * channels);
	}
}

GLubyte* Texture::getTextureFromProgram(int texture, int width, int height,int channels) {
	GLubyte* pixels = new GLubyte[width * height * channels * sizeof(GLubyte)*2];
	glActiveTexture(texture);
	if (channels == 3) {
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	}
	else if(channels == 4){
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	}
	else if (channels == 1) {
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);
	}
	return pixels;
}
TextureData Texture::getTextureData(const char* path) {
	TextureData td;
	int width, height, channelS;
	stbi_info(path, &width, &height, &channelS);
	td.width = width;
	td.height = height;
	td.channels = channelS;
	return td;
}
ScreenPaintingReturnData Texture::createScreenPaintTexture(GLubyte* &screenTexture,GLFWwindow* window) {
	ScreenPaintingReturnData screenPaintingReturnData; 
	std::fill_n(screenTexture, 1920 * 1080* 3, 0);
	GlSet glset;

	//Normal screen painting texture
	glset.activeTexture(GL_TEXTURE4);
	unsigned int textureID;
	glset.genTextures(textureID);
	glset.bindTexture(textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, screenTexture);
	glset.generateMipmap();


	//Mirrored screen painting texture
	GLubyte* mirroredScreenTexture = new GLubyte[1920 * 1080 * 3];
	std::fill_n(mirroredScreenTexture, 1920 * 1080* 3, 0);

	glset.activeTexture(GL_TEXTURE3);
	unsigned int textureIDMir;
	glset.genTextures(textureIDMir);
	glset.bindTexture(textureIDMir);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, mirroredScreenTexture);
	glset.generateMipmap();

	delete(mirroredScreenTexture);

	screenPaintingReturnData.normalId = textureID;
	screenPaintingReturnData.mirroredId = textureIDMir;

	return screenPaintingReturnData;
}
void Texture::refreshScreenDrawingTexture() {
	GlSet glset;
	GLubyte* screenTextureX = new GLubyte[1920 * 1080 * 3];//Deleted
	std::fill_n(screenTextureX, 1920 * 1080 * 3, 0);
	glset.activeTexture(GL_TEXTURE4);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1920, 1080, GL_RGB, GL_UNSIGNED_BYTE, screenTextureX); //Refresh Screen Texture
	glset.generateMipmap();
	delete(screenTextureX);

	//Mirrored
	GLubyte* screenTextureM = new GLubyte[1080 * 1080 * 3];//Deleted
	std::fill_n(screenTextureM, 1080 * 1080 * 3, 0);
	glset.activeTexture(GL_TEXTURE3);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1080, 1080, GL_RGB, GL_UNSIGNED_BYTE, screenTextureM); //Refresh Screen Texture
	glset.generateMipmap();
	delete(screenTextureM);
}
GLubyte* Texture::updateMaskTexture(unsigned int FBOScreen, unsigned int screenSize_x, unsigned int screenSize_y, float brushRotationRangeBarValue,bool renderTiny) { //rotationValue = rotationBarValue
	GlSet glset;
	UserInterface ui;
	glset.viewport(1080, 1080);

	float rotation = ((brushRotationRangeBarValue +0.11f) * 4.54545454545) * 360; // -0.11 - 0.11 --> 0 - 360

	//Rotate and scale
	glm::mat4 trans = glm::mat4(1.0f);
	if(!renderTiny){
		trans = glm::translate(trans, glm::vec3(-0.5f, -0.5f, 0.0f));
		trans = glm::rotate(trans, glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0));
		trans = glm::scale(trans, glm::vec3(0.8, 0.8, 0.8));
		glset.uniformMatrix4fv(txtrPrograms.program, "renderTrans", trans);
	}
	else{
		trans = glm::translate(trans, glm::vec3(-0.875f, -0.875f, 0.0f));
		trans = glm::rotate(trans, glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0));
		trans = glm::scale(trans, glm::vec3(0.2, 0.2, 0.2));
		glset.uniformMatrix4fv(txtrPrograms.program, "renderTrans", trans);
	}
	//Rotate and scale

	int size;
	if(renderTiny)
		size = 135;
	else
		size = 540;
	
	//16:9 ---> 1:1 (makes easier to get vertices into the middle)
	glUseProgram(txtrPrograms.program);
	glm::mat4 renderTextureProjection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);
	glset.uniformMatrix4fv(txtrPrograms.program, "renderTextureProjection", renderTextureProjection);
	//16:9 ---> 1:1

	glClearColor(0, 0, 0, 1.0f); //Back color while rendering mask texture

	std::vector<float> centerVertices = { 
	// first triangle
	 0.75f,  0.75f, 0.0f,1,1,0,0,0,  // top right
	 0.75f,  0.25f, 0.0f,1,0,0,0,0,  // bottom right
	 0.25f,  0.75f, 0.0f,0,1,0,0,0,  // top left 
	// second triangle	  ,0,0,0,
	 0.75f,  0.25f, 0.0f,1,0,0,0,0,  // bottom right
	 0.25f,  0.25f, 0.0f,0,0,0,0,0,  // bottom left
	 0.25f,  0.75f, 0.0f,0,1,0,0,0   // top left
	};

	std::vector<float> cornerVertices = { 
	// first triangle
	 0.5f,  0.5f, 0.0f,1,1,0,0,0,  // top right
	 0.5f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
	 0.0f,  0.5f, 0.0f,0,1,0,0,0,  // top left 
	// second triangle	  ,0,0,0,
	 0.5f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
	 0.0f,  0.0f, 0.0f,0,0,0,0,0,  // bottom left
	 0.0f,  0.5f, 0.0f,0,1,0,0,0   // top left
	};
	std::vector<float> cornerVerticesQrtr = { 
	// first triangle
	 0.125f,  0.125f, 0.0f,1,1,0,0,0,  // top right
	 0.125f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
	 0.0f,  0.125f, 0.0f,0,1,0,0,0,  // top left 
	// second triangle	  ,0,0,0,
	 0.125f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
	 0.0f,  0.0f, 0.0f,0,0,0,0,0,  // bottom left
	 0.0f,  0.125f, 0.0f,0,1,0,0,0   // top left
	};
	
	//Setup
	glset.uniform1i(txtrPrograms.program, "isTwoDimensional", 1);
	glset.uniform1i(txtrPrograms.program, "isRenderTextureMode", 1);
	glset.uniform1i(txtrPrograms.program, "isRenderTextureModeV", 1);
	glset.uniform1i(txtrPrograms.program, "renderMaskBrush", 1);
	glset.bindFramebuffer(FBOScreen);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glset.uniform1i(txtrPrograms.program, "modifiedMaskTexture", 1);
	//Setup

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glPixelStorei(GL_PACK_ALIGNMENT,1);

	//Rotation
	glset.drawArrays(centerVertices, false);
	GLubyte* rotatedMaskTxtr = new GLubyte[size * size * 3 ];
	glReadPixels(0, 0, size, size, GL_RGB, GL_UNSIGNED_BYTE, rotatedMaskTxtr);

	glset.activeTexture(GL_TEXTURE12);
	glset.texImage(rotatedMaskTxtr, size, size, GL_RGB);
	glset.generateMipmap();
	//Rotation

	//Get back to previous projection after rendering rotated & scaled mask texture
	glset.uniform1i(txtrPrograms.program, "modifiedMaskTexture", 12);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glset.viewport(1920, 1080);
	trans = glm::mat4(1.0f);

	glset.uniformMatrix4fv(txtrPrograms.program, "renderTrans", trans);
	renderTextureProjection = glm::ortho(0.0f, 1.77777777778f, 0.0f, 1.0f);//1920 - 1080 -> 1.77777777778 - 1
	glset.uniformMatrix4fv(txtrPrograms.program, "renderTextureProjection", renderTextureProjection);
	//Get back to previous projection after rendering rotated & scaled mask texture


	glUseProgram(txtrPrograms.blurProgram);


	//Horizontal Blur
	glset.uniform1i(9, "isRenderVerticalBlur", 0); 
	glset.uniform1i(9, "inputTexture", 12); 

	if(renderTiny)
		glset.drawArrays(cornerVerticesQrtr, false);
	else
		glset.drawArrays(cornerVertices, false);

	GLubyte* horizontalBlurMaskTxtr = new GLubyte[size * size * 3];
	glReadPixels(0, 0, size, size, GL_RGB, GL_UNSIGNED_BYTE, horizontalBlurMaskTxtr);

	glset.activeTexture(GL_TEXTURE12);
	glset.texImage(horizontalBlurMaskTxtr, size, size, GL_RGB);
	glset.generateMipmap();
	//Horizontal Blur

	//Vertical Blur setup
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glset.uniform1i(9, "isRenderVerticalBlur", 1);
	//Vertical Blur setup

	//Vertical blur
		if(renderTiny)
			glset.drawArrays(cornerVerticesQrtr, false);
		else
			glset.drawArrays(cornerVertices, false);

	GLubyte* verticalBlurMaskTxtr = new GLubyte[size * size * 3 ];
	glReadPixels(0, 0, size, size, GL_RGB, GL_UNSIGNED_BYTE, verticalBlurMaskTxtr);

	glset.activeTexture(GL_TEXTURE12);
	glset.texImage(verticalBlurMaskTxtr, size, size, GL_RGB);
	glset.generateMipmap();

	//Verical blur

	//Finish

	
	glUseProgram(txtrPrograms.program);


	ui.setViewportBgColor();
	glset.uniform1i(txtrPrograms.program, "renderMaskBrush", 0);
	glset.uniform1i(txtrPrograms.program, "isTwoDimensional", 0);
	glset.uniform1i(txtrPrograms.program, "isRenderTextureModeV", 0);
	glset.uniform1i(txtrPrograms.program, "isRenderTextureMode", 0);
	glset.bindFramebuffer(0);
	glset.viewport(screenSize_x, screenSize_y);

	delete(rotatedMaskTxtr);
	delete(horizontalBlurMaskTxtr);
	return verticalBlurMaskTxtr;
}
InitializedTextures Texture::initTextures(const char* maskTexturePath){
	GlSet glset;
	InitializedTextures textures;
	glset.activeTexture(GL_TEXTURE9);
	unsigned int depthTexture;
	glset.genTextures(depthTexture);
	glset.bindTexture(depthTexture);

	glset.activeTexture(GL_TEXTURE8);
	unsigned int mirroredDepthTexture;
	glset.genTextures(mirroredDepthTexture);
	glset.bindTexture(mirroredDepthTexture);

	glset.activeTexture(GL_TEXTURE7);//Albedo
	unsigned int enlargedTexture;
	glset.genTextures(enlargedTexture);
	glset.bindTexture(enlargedTexture);

	glset.activeTexture(GL_TEXTURE12);
	unsigned int modifiedMaskTexture;
	glset.genTextures(modifiedMaskTexture);
	glset.bindTexture(modifiedMaskTexture);

	glset.activeTexture(GL_TEXTURE1);
	unsigned int rawMaskTexture;
	glset.genTextures(rawMaskTexture);
	glset.bindTexture(rawMaskTexture);

	glset.activeTexture(GL_TEXTURE0);
	unsigned int albedoTxtr;
	glset.genTextures(albedoTxtr);
	glset.bindTexture(albedoTxtr);

	glset.activeTexture(GL_TEXTURE1);//Raw mask
	getTexture(maskTexturePath,0,0,false);
	glset.activeTexture(GL_TEXTURE12);//Modified mask
	getTexture(maskTexturePath,0,0,false);

	textures.albedoTxtr = albedoTxtr;
	textures.depthTexture = depthTexture;
	textures.enlargedTexture = enlargedTexture;
	textures.mirroredDepthTexture = mirroredDepthTexture;

	textures.modifiedMaskTexture = modifiedMaskTexture;
	textures.rawMaskTexture = rawMaskTexture;

	return textures;
}
void Texture::sendProgramsToTextures(Programs apptxtrPrograms){
	txtrPrograms = apptxtrPrograms;
}