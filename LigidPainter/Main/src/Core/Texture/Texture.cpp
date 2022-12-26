#include<iostream>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/LigidPainter.h"
#include "Core/UserInterface.h"
#include "Core/gl.h"
#include "Core/Texture/Texture.h"

#include "Core/Texture Generator/TextureGenerator.h"

#include "stb_image.h"
#include "stb_image_write.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

using namespace std;

Programs txtrPrograms;
int textureMaxScreenWidth;
int textureMaxScreenHeight;

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
	std::fill_n(screenTexture, (textureMaxScreenWidth)* (textureMaxScreenHeight), 0);
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

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, textureMaxScreenWidth, textureMaxScreenHeight, 0, GL_RED, GL_UNSIGNED_BYTE, screenTexture);
	glset.generateMipmap();


	//Mirrored screen painting texture
	GLubyte* mirroredScreenTexture = new GLubyte[1080 * 1080];
	std::fill_n(mirroredScreenTexture, 1080 * 1080, 0);

	glset.activeTexture(GL_TEXTURE3);
	unsigned int textureIDMir;
	glset.genTextures(textureIDMir);
	glset.bindTexture(textureIDMir);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1080, 1080, 0, GL_RED, GL_UNSIGNED_BYTE, mirroredScreenTexture);
	glset.generateMipmap();

	delete(mirroredScreenTexture);

	screenPaintingReturnData.normalId = textureID;
	screenPaintingReturnData.mirroredId = textureIDMir;

	return screenPaintingReturnData;
}
void Texture::refreshScreenDrawingTexture() {

	refreshScreenTxtr();

	GlSet glset;
	GLubyte* screenTextureX = new GLubyte[(textureMaxScreenWidth) * (textureMaxScreenHeight)];
	std::fill_n(screenTextureX, (textureMaxScreenWidth) * (textureMaxScreenHeight), 0);
	glset.activeTexture(GL_TEXTURE4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, textureMaxScreenWidth, textureMaxScreenHeight, 0, GL_RED, GL_UNSIGNED_BYTE, screenTextureX);
	glset.generateMipmap();
	delete[] screenTextureX;

	//Mirrored
	GLubyte* screenTextureM = new GLubyte[1080 * 1080];//Deleted
	std::fill_n(screenTextureM, 1080 * 1080, 0);
	glset.activeTexture(GL_TEXTURE3);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1080,1080, GL_RED, GL_UNSIGNED_BYTE, screenTextureM); //Refresh Screen Texture
	glset.generateMipmap();
	delete[] screenTextureM;
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

unsigned int Texture::loadCubemap(std::vector<std::string> faces,unsigned int textureSlot)
{
	GlSet glset;
	glset.activeTexture(textureSlot);
    unsigned int textureID;
    glset.genTextures(textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void Texture::sendProgramsToTextures(Programs apptxtrPrograms){
	txtrPrograms = apptxtrPrograms;
}
void Texture::sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight){
	textureMaxScreenHeight = maxScreenHeight;
	textureMaxScreenWidth = maxScreenWidth;
}