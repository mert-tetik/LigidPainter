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



GetTextureData Texture::getTexture(std::string path, unsigned int desiredWidth, unsigned int desiredHeight) {
	//Leave desiredWidth 0 if no resize wanted
	bool applyResize;
	applyResize = desiredWidth;

	GetTextureData getTextureData;
	unsigned int textureID;
	GlSet glset;
	glset.genTextures(textureID);
	glset.bindTexture(textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
	getTextureData.Id = textureID;
	return getTextureData;
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
unsigned int Texture::createScreenPaintTexture(GLubyte* &screenTexture,GLFWwindow* window) {
	std::fill_n(screenTexture, 1920 * 1080* 3, 0);
	GlSet glset;

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
	return textureID;
}
void Texture::refreshScreenDrawingTexture() {
	GlSet glset;
	CommonData commonData;
	GLubyte* screenTextureX = new GLubyte[1920 * 1080 * 3];//Deleted
	std::fill_n(screenTextureX, 1920 * 1080 * 3, 0);
	glset.activeTexture(GL_TEXTURE4);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1920, 1080, GL_RGB, GL_UNSIGNED_BYTE, screenTextureX); //Refresh Screen Texture
	glset.generateMipmap();
	delete(screenTextureX);
}
GLubyte* Texture::updateMaskTexture(unsigned int FBOScreen, unsigned int screenSize_x, unsigned int screenSize_y, float brushRotationRangeBarValue) { //rotationValue = rotationBarValue
	CommonData commonData;
	GlSet glset;
	UserInterface ui;
	glset.viewport(1080, 1080);

	float rotation = ((brushRotationRangeBarValue +0.11f) * 4.54545454545) * 360; // -0.11 - 0.11 --> 0 - 360

	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(-0.5f, -0.5f, 0.0f));
	trans = glm::rotate(trans, glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0));
	trans = glm::scale(trans, glm::vec3(0.8, 0.8, 0.8));
	glset.uniformMatrix4fv(commonData.program, "renderTrans", trans);

	glm::mat4 renderTextureProjection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);//1920 - 1080 -> 1.77777777778 - 1
	glset.uniformMatrix4fv(commonData.program, "renderTextureProjection", renderTextureProjection);

	glClearColor(0, 0, 0, 1.0f);

	std::vector<float> renderVertices = { 
	// first triangle
	 0.75f,  0.75f, 0.0f,1,1,0,0,0,  // top right
	 0.75f,  0.25f, 0.0f,1,0,0,0,0,  // bottom right
	 0.25f,  0.75f, 0.0f,0,1,0,0,0,  // top left 
	// second triangle	  ,0,0,0,
	 0.75f,  0.25f, 0.0f,1,0,0,0,0,  // bottom right
	 0.25f,  0.25f, 0.0f,0,0,0,0,0,  // bottom left
	 0.25f,  0.75f, 0.0f,0,1,0,0,0   // top left
	};

	std::vector<float> renderVerticesX = { 
	// first triangle
	 0.5f,  0.5f, 0.0f,1,1,0,0,0,  // top right
	 0.5f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
	 0.0f,  0.5f, 0.0f,0,1,0,0,0,  // top left 
	// second triangle	  ,0,0,0,
	 0.5f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
	 0.0f,  0.0f, 0.0f,0,0,0,0,0,  // bottom left
	 0.0f,  0.5f, 0.0f,0,1,0,0,0   // top left
	};
	
	glset.uniform1i(commonData.program, "isTwoDimensional", 1);
	glset.uniform1i(commonData.program, "isRenderTextureMode", 1);
	glset.uniform1i(commonData.program, "isRenderTextureModeV", 1);
	glset.uniform1i(commonData.program, "renderMaskBrush", 1);
	glset.uniform1i(commonData.program, "renderMaskBrushBlury", 0);

	glset.bindFramebuffer(FBOScreen);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Setup

	glset.uniform1i(commonData.program, "uiMaskTexture", 1);
	
	//Rotation
	glset.drawArrays(renderVertices, false);
	GLubyte* renderedImageXX = new GLubyte[540 * 540 * 3 * sizeof(GLubyte)];
	glReadPixels(0, 0, 540, 540, GL_RGB, GL_UNSIGNED_BYTE, renderedImageXX);

	glset.activeTexture(GL_TEXTURE12);
	glset.texImage(renderedImageXX, 540, 540, GL_RGB);
	glset.generateMipmap();
	//Rotation

	glset.uniform1i(commonData.program, "uiMaskTexture", 12);
	glset.uniform1i(commonData.program, "renderMaskBrushBlury", 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glset.viewport(1920, 1080);
	trans = glm::mat4(1.0f);
	glset.uniformMatrix4fv(commonData.program, "renderTrans", trans);

	renderTextureProjection = glm::ortho(0.0f, 1.77777777778f, 0.0f, 1.0f);//1920 - 1080 -> 1.77777777778 - 1
	glset.uniformMatrix4fv(commonData.program, "renderTextureProjection", renderTextureProjection);


	//Horizontal Blur
	glset.drawArrays(renderVerticesX, false);
	GLubyte* renderedImageX = new GLubyte[540 * 540 * 3 * sizeof(GLubyte)];
	glReadPixels(0, 0, 540, 540, GL_RGB, GL_UNSIGNED_BYTE, renderedImageX);

	glset.activeTexture(GL_TEXTURE12);
	glset.texImage(renderedImageX, 540, 540, GL_RGB);
	glset.generateMipmap();
	//Horizontal Blur

	//Vertical Blur setup
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glset.uniform1i(commonData.program, "isRenderVerticalBlur", 1);
	//Vertical Blur setup

	//Vertical blur
	glset.drawArrays(renderVerticesX, false);
	GLubyte* renderedImage = new GLubyte[540 * 540 * 3 * sizeof(GLubyte)];
	glReadPixels(0, 0, 540, 540, GL_RGB, GL_UNSIGNED_BYTE, renderedImage);

	glset.activeTexture(GL_TEXTURE12);
	glset.texImage(renderedImage, 540, 540, GL_RGB);
	glset.generateMipmap();

	//Verical blur

	//Finish


	trans = glm::mat4(1.0f);
	glset.uniformMatrix4fv(commonData.program, "renderTrans", trans);
	ui.setViewportBgColor();
	glset.uniform1i(commonData.program, "isRenderVerticalBlur", 0);
	glset.uniform1i(commonData.program, "renderMaskBrush", 0);
	glset.uniform1i(commonData.program, "isTwoDimensional", 0);
	glset.uniform1i(commonData.program, "isRenderTextureModeV", 0);
	glset.uniform1i(commonData.program, "isRenderTextureMode", 0);
	glset.bindFramebuffer(0);
	glset.viewport(screenSize_x, screenSize_y);

	delete(renderedImageX);
	delete(renderedImageXX);
	return renderedImage;
}