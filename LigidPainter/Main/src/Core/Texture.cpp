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



vector<float> gaussianFunc(float radius,float mean){
	// vector<float> oneDimensionalGauss = {0.0003,	0.0004,	0.0007,	0.0012,	0.0019,	0.0029,	0.0044,	0.0064,	0.0090,	0.0124,	0.0166,	0.0216,	0.0274,	0.0337,	0.0404,	0.0470,	0.0532,	0.0587,	0.0629,	0.0655,	0.0665,	0.0655,	0.0629,	0.0587,	0.0532,	0.0470,	0.0404,	0.0337,	0.0274,	0.0216,	0.0166,	0.0124,	0.0090,	0.0064,	0.0044,	0.0029,	0.0019,	0.0012,	0.0007,	0.0004,	0.0003};
	vector<float> oneDimensionalGauss;
	
	float sigma = (radius / sqrt(-2.0 * log(1 - 0.995))) ;

	//float radiusX = ceil(sqrt(-2.0 * sigma * sigma * log(1.0 - 0.995)));
 
	//cout << result << ' ';


	const float pi = 3.14159265359f;

	float summ = 0;

	for (int i = 0; i < radius*2 + 1; i++)
	{
    	float denominator = sigma * sqrt(2.0f * pi);
    	float expon = exp( (-1.0f/2.0f) * (pow((float)i-((float)radius) - mean , 2.0f) / pow(sigma,2.0f)));

    	float a = 1.0f / denominator;
    	float result = a * expon;
		oneDimensionalGauss.push_back(result);
		summ += result;
		//cout << result << ' ';

	}
	//cout << "\n\n\n\n\n\n";

	cout << oneDimensionalGauss.size() << ' '; 
	
	//cout << "Summ : "<< summ << '\n';

    return oneDimensionalGauss;
}



GLubyte* Texture::updateMaskTexture(unsigned int FBOScreen,  int screenSize_x, int screenSize_y, float brushRotationRangeBarValue,bool renderTiny,float brushBorderRangeBarValue,float brushBlurVal,OutShaderData outShaderData) { //rotationValue = rotationBarValue
	GlSet glset;
	UserInterface ui;
	glset.viewport(1080, 1080);

	float rotation = ((brushRotationRangeBarValue +0.11f) * 4.54545454545) * 360; // -0.11 - 0.11 --> 0 - 360

	float scaleVal = ((brushBorderRangeBarValue+0.11f)/2.0f*8.18181818182) + 0.3f;
	float scaleValTiny = scaleVal/4.0f;

	glset.useOutShader(txtrPrograms.outProgram,outShaderData);

	//Rotate and scale
	glm::mat4 trans = glm::mat4(1.0f);
	if(!renderTiny){
		trans = glm::translate(trans, glm::vec3(-0.5f, -0.5f, 0.0f));
		trans = glm::rotate(trans, glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0));
		trans = glm::scale(trans, glm::vec3(scaleVal, scaleVal, scaleVal));
		glset.uniformMatrix4fv(txtrPrograms.outProgram, "renderTrans", trans);
	}
	else{
		trans = glm::translate(trans, glm::vec3(-0.875f, -0.875f, 0.0f));
		trans = glm::rotate(trans, glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0));
		trans = glm::scale(trans, glm::vec3(scaleValTiny, scaleValTiny, scaleValTiny));
		glset.uniformMatrix4fv(txtrPrograms.outProgram, "renderTrans", trans);
	}
	//Rotate and scale

	int size;
	if(renderTiny)
		size = 135;
	else
		size = 540;
	
	//16:9 ---> 1:1 (makes easier to get vertices into the middle)
	glUseProgram(txtrPrograms.outProgram);
	glm::mat4 renderTextureProjection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);
	glset.uniformMatrix4fv(txtrPrograms.outProgram, "renderTextureProjection", renderTextureProjection);
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
	glset.uniform1i(txtrPrograms.outProgram, "isTwoDimensional", 1);
	glset.uniform1i(txtrPrograms.outProgram, "renderMaskBrush", 1);
	glset.bindFramebuffer(FBOScreen);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glset.uniform1i(txtrPrograms.outProgram, "modifiedMaskTexture", 1);
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
	glset.uniform1i(txtrPrograms.outProgram, "modifiedMaskTexture", 12);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glset.viewport(1920, 1080);
	trans = glm::mat4(1.0f);

	glset.uniformMatrix4fv(txtrPrograms.outProgram, "renderTrans", trans);
	renderTextureProjection = glm::ortho(0.0f, 1.77777777778f, 0.0f, 1.0f);//1920 - 1080 -> 1.77777777778 - 1
	glset.uniformMatrix4fv(txtrPrograms.outProgram, "renderTextureProjection", renderTextureProjection);
	//Get back to previous projection after rendering rotated & scaled mask texture


	glUseProgram(txtrPrograms.blurProgram);

	BlurShaderData blurShaderData;

	blurShaderData.brushBlurVal = brushBlurVal;
	blurShaderData.inputTexture = 12; 
	blurShaderData.isRenderVerticalBlur = 0;
	blurShaderData.renderTextureProjection = renderTextureProjection; 




	glset.useBlurShader(txtrPrograms.blurProgram,blurShaderData);

	

	vector<float> oneDimensionalGaussian = gaussianFunc(min(brushBlurVal,60.0f),0.0f);




	glset.uniform1i(txtrPrograms.blurProgram , "kernel_sizeF" , oneDimensionalGaussian.size());


	for (int i = 0; i < oneDimensionalGaussian.size(); i++)
	{
		std::string target = "oneDimensionalGaussF1[" + std::to_string(i) + "]";
		glset.uniform1f(txtrPrograms.blurProgram , target.c_str() , oneDimensionalGaussian[i]);
	}


	//Horizontal Blur
	glset.uniform1i(txtrPrograms.blurProgram, "isRenderVerticalBlur", 0); 
	glset.uniform1i(txtrPrograms.blurProgram, "inputTexture", 12); 

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
	
	blurShaderData.isRenderVerticalBlur = 1;
	glset.useBlurShader(txtrPrograms.blurProgram,blurShaderData);
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

	
	glUseProgram(txtrPrograms.uiProgram);


	ui.setViewportBgColor();
	glset.bindFramebuffer(0);
	glViewport(-(textureMaxScreenWidth - screenSize_x)/2, -(textureMaxScreenHeight - screenSize_y), textureMaxScreenWidth, textureMaxScreenHeight);

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