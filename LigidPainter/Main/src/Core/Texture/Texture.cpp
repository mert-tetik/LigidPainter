#include<iostream>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/LigidPainter.h"
#include "Core/UI/UserInterface.h"
#include "Core/Utilities.h"
#include "Core/gl.h"
#include "Core/Texture/Texture.h"

#include "Core/Texture Generator/TextureGenerator.h"

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
	
	Utilities util;
	if(!util.illegalCharCheck(path)){
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
		
		char* filedata;
		uint64_t len;
		
		unsigned char* data;

		int liRes = 0;
		filedata = util.processLiFile(path.c_str(),len,liRes);

		if(liRes){
			data = stbi_load_from_memory((stbi_uc*)filedata,len,&width,&height,&nrChannels,4);
		}
		else{
			data = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);
		}


		
		//stbi_load_from_memory(filedata,0,);
		GLubyte* resizedPixelsX = NULL;
		if (applyResize) {
			resizedPixelsX = new GLubyte[desiredWidth * desiredHeight * 4];
			stbir_resize_uint8(data, width, height, 0, resizedPixelsX, desiredWidth, desiredHeight, 0, 4);
		}

		if (data != NULL)
		{
			if(applyResize)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, desiredWidth, desiredHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, resizedPixelsX);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glset.generateMipmap();

			std::cout << "Loaded " << path << std::endl;
		}
		else
		{
			const char* reason = "[unknown reason]";
			if (stbi_failure_reason())
			{
				reason = stbi_failure_reason();
			}
			std::cout << "Failed to load texture! " << path << " Reason : " << reason<< std::endl;
		}
		

		stbi_image_free(data);
		delete[] resizedPixelsX;
		return textureID;
	}
	else{
		UserInterface ui;
		ui.alert("ERROR : Texture can't be imported. There are illegal characters in the path.",400,false);
		return 0;
	}

	
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

GLubyte* Texture::getTextureFromProgram(unsigned int texture, int width, int height,int channels) {
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
	std::fill_n(screenTexture, (glfwGetVideoMode(glfwGetPrimaryMonitor())->width)* (glfwGetVideoMode(glfwGetPrimaryMonitor())->height * 4), 0);
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

	glset.texImage(nullptr,glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height,GL_RGBA);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glset.generateMipmap();


	//Mirrored screen painting texture
	GLubyte* mirroredScreenTexture = new GLubyte[1080 * 1080 * 4];
	std::fill_n(mirroredScreenTexture, 1080 * 1080 * 4, 0);

	glset.activeTexture(GL_TEXTURE3);
	unsigned int textureIDMir;
	glset.genTextures(textureIDMir);
	glset.bindTexture(textureIDMir);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1080, 1080, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glset.generateMipmap();

	delete(mirroredScreenTexture);

	screenPaintingReturnData.normalId = textureID;
	screenPaintingReturnData.mirroredId = textureIDMir;

	return screenPaintingReturnData;
}
void Texture::refreshScreenDrawingTexture() {

	refreshScreenTxtr();

	GlSet glset;

	GLubyte* screenTexture3M = new GLubyte[(glfwGetVideoMode(glfwGetPrimaryMonitor())->width) * (glfwGetVideoMode(glfwGetPrimaryMonitor())->height * 4)];
	std::fill_n(screenTexture3M, (glfwGetVideoMode(glfwGetPrimaryMonitor())->width) * (glfwGetVideoMode(glfwGetPrimaryMonitor())->height * 4), 0);
	glset.activeTexture(GL_TEXTURE8);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, screenTexture3M);
	glset.generateMipmap();
	delete[] screenTexture3M;
	
	GLubyte* screenTextureX = new GLubyte[(glfwGetVideoMode(glfwGetPrimaryMonitor())->width) * (glfwGetVideoMode(glfwGetPrimaryMonitor())->height * 4)];
	std::fill_n(screenTextureX, (glfwGetVideoMode(glfwGetPrimaryMonitor())->width) * (glfwGetVideoMode(glfwGetPrimaryMonitor())->height * 4), 0);
	glset.activeTexture(GL_TEXTURE4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, screenTextureX);
	glset.generateMipmap();
	delete[] screenTextureX;

	//Mirrored
	GLubyte* screenTextureM = new GLubyte[1080 * 1080*4];//Deleted
	std::fill_n(screenTextureM, 1080 * 1080*4, 0);
	glset.activeTexture(GL_TEXTURE3);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1080,1080, GL_RGBA16F, GL_UNSIGNED_BYTE, screenTextureM); //Refresh Screen Texture
	glset.generateMipmap();
	delete[] screenTextureM;
}
void Texture::deleteOpenglTexture(aTexture texture){
	glDeleteTextures(1,&texture.id);
	for (size_t i = 0; i < texture.undoList.size(); i++)
	{
		glDeleteTextures(1,&texture.undoList[i]);
	}
}

void Texture::sendProgramsToTextures(Programs apptxtrPrograms){
	txtrPrograms = apptxtrPrograms;
}
void Texture::sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight){
}

GLubyte* Texture::resizeTexture(GLubyte* data,int w,int h,int ow,int oh){
	
	// glViewport(0,0,ow,oh);

	// glActiveTexture(GL_TEXTURE28);
	// unsigned int bufferTxtr;
	// glGenTextures(1,&bufferTxtr);

	// unsigned int FBO;
	// glGenFramebuffers(1,&FBO);
	// glBindFramebuffer(GL_FRAMEBUFFER,FBO);
	// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferTxtr, 0);

	
	// std::vector<float> renderVertices = { 
	// 	// first triangle
	// 	 1.0f,  1.0f, 0.0f,1,1,0,0,0,  // top right
	// 	 1.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
	// 	 0.0f,  1.0f, 0.0f,0,1,0,0,0,  // top left 
	// 	// second triangle	  ,0,0,0,
	// 	 1.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
	// 	 0.0f,  0.0f, 0.0f,0,0,0,0,0,  // bottom left
	// 	 0.0f,  1.0f, 0.0f,0,1,0,0,0   // top left
	// };

	// GlSet gl;

	// glActiveTexture(GL_TEXTURE13);
	
	// gl.drawArrays(renderVertices,false);

	// glReadPixels(0,0,);


	// glUseProgram(txtrPrograms.renderTheTextureProgram);



	// LigidPainter lp;
	// lp.setViewportToDefault();
	
	// glBindFramebuffer(GL_FRAMEBUFFER,0);
	// glDeleteFramebuffers(1,&FBO);
	return {};
}