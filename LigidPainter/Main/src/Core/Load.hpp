#ifndef LGDLOAD_H
#define LGDLOAD_H

#include<iostream>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <vector>
#include <filesystem>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/LigidPainter.h"
#include "Core/UI/UserInterface.h"
#include "Core/gl.h"
#include "Core/Texture/Texture.h"
#include "Core/Load.hpp"

#include "Core/Texture Generator/TextureGenerator.h"

#include "stb_image.h"
#include "stb_image_write.h"
#include "stb_image_resize.h"

#include "freetype/freetype.h"

struct character {
	unsigned int TextureID;
	glm::ivec2   Size;
	glm::ivec2   Bearing;
	unsigned int Advance;
};

struct InitializedTextures;

class Load{
public:
	void sendCharsToUI(std::map<char, character> characters);

	unsigned int loadCubemap(std::vector<std::string> faces,unsigned int textureSlot)
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
	
	Cubemaps loadCubemaps(){
		Texture txtr;
		std::vector<std::string> faces
		{
		    "LigidPainter/Resources/Cubemap/Skybox/px.png",
		    "LigidPainter/Resources/Cubemap/Skybox/nx.png",
		    "LigidPainter/Resources/Cubemap/Skybox/ny.png",
		    "LigidPainter/Resources/Cubemap/Skybox/py.png",
		    "LigidPainter/Resources/Cubemap/Skybox/pz.png",
		    "LigidPainter/Resources/Cubemap/Skybox/nz.png"
		};
		unsigned int cubemapTexture = loadCubemap(faces,GL_TEXTURE13);  
		std::vector<std::string> bluryfaces
		{
		    "LigidPainter/Resources/Cubemap/Skybox/pxblur.png",
		    "LigidPainter/Resources/Cubemap/Skybox/nxblur.png",
		    "LigidPainter/Resources/Cubemap/Skybox/nyblur.png",
		    "LigidPainter/Resources/Cubemap/Skybox/pyblur.png",
		    "LigidPainter/Resources/Cubemap/Skybox/pzblur.png",
		    "LigidPainter/Resources/Cubemap/Skybox/nzblur.png"
		};
		unsigned int cubemapTextureBlury = loadCubemap(bluryfaces,GL_TEXTURE13);
	
		Cubemaps cubemaps;
		cubemaps.blurycubemap = cubemapTextureBlury;
		cubemaps.cubemap = cubemapTexture;
	
		return cubemaps;
	}
	
	LigidCursors loadCursors(){
	
		GLFWimage images[1];
		stbi_set_flip_vertically_on_load(false);
		images[0].pixels = stbi_load("LigidPainter/Resources/Icons/PointerIcon.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
		GLFWcursor* pointerCursor = glfwCreateCursor(images,15,0);
		stbi_image_free(images[0].pixels);
	
		images[0].pixels = stbi_load("LigidPainter/Resources/Icons/DefaultIcon.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
		GLFWcursor* defaultCursor = glfwCreateCursor(images,7,0);
		stbi_image_free(images[0].pixels);
	
		images[0].pixels = stbi_load("LigidPainter/Resources/Icons/DropperCursor.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
		GLFWcursor* dropperCursor = glfwCreateCursor(images,0,30);
		stbi_image_free(images[0].pixels);
	
		LigidCursors cursors;
		cursors.pointerCursor = pointerCursor;
		cursors.defaultCursor = defaultCursor;
		cursors.dropperCursor = dropperCursor;
	
		return cursors;
	}
	
	Icons loadIcons(){
		GlSet glset;
		Texture txtr;
		Icons icons;
	
		glset.activeTexture(GL_TEXTURE6);
		icons.dropperIcon = txtr.getTexture("LigidPainter/Resources/Icons/Dropper.png",0,0,false);
		icons.TDModel = txtr.getTexture("LigidPainter/Resources/Icons/3DModel.jpg",0,0,false);
		icons.BackfaceCulling = txtr.getTexture("LigidPainter/Resources/Icons/BackfaceCulling.jpg",0,0,false);
		icons.ColorPicker = txtr.getTexture("LigidPainter/Resources/Icons/ColorPicker.png",0,0,false);
		icons.Export = txtr.getTexture("LigidPainter/Resources/Icons/Export.jpg",0,0,false);
		icons.Folder = txtr.getTexture("LigidPainter/Resources/Icons/Folder.png",0,0,false);
		icons.ImportMask = txtr.getTexture("LigidPainter/Resources/Icons/ImportMask.png",0,0,false);
		icons.ImportModel = txtr.getTexture("LigidPainter/Resources/Icons/ImportModel.jpg",0,0,false);
		icons.ImportTexture = txtr.getTexture("LigidPainter/Resources/Icons/ImportTexture.jpg",0,0,false);
		icons.JpgFile = txtr.getTexture("LigidPainter/Resources/Icons/JpgFile.png",0,0,false);
		icons.MaskGausBlur = txtr.getTexture("LigidPainter/Resources/Icons/MaskGausBlur.png",0,0,false);
		icons.MaskOpacity = txtr.getTexture("LigidPainter/Resources/Icons/MaskOpacity.png",0,0,false);
		icons.MaskRotation = txtr.getTexture("LigidPainter/Resources/Icons/MaskRotation.png",0,0,false);
		icons.MaskScale = txtr.getTexture("LigidPainter/Resources/Icons/MaskScale.png",0,0,false);
		icons.MaskSpacing = txtr.getTexture("LigidPainter/Resources/Icons/MaskSpacing.png",0,0,false);
		icons.Mirror = txtr.getTexture("LigidPainter/Resources/Icons/Mirror.jpg",0,0,false);
		icons.Panel = txtr.getTexture("LigidPainter/Resources/Icons/Panel.png",0,0,false);
		icons.PngFile = txtr.getTexture("LigidPainter/Resources/Icons/PngFile.png",0,0,false);
		icons.Sphere = txtr.getTexture("LigidPainter/Resources/Icons/Sphere.jpg",0,0,false);
		icons.Triangulate = txtr.getTexture("LigidPainter/Resources/Icons/Triangulate.jpg",0,0,false);
		icons.ArrowRight = txtr.getTexture("LigidPainter/Resources/Icons/ArrowRight.jpg",0,0,false);
		icons.ArrowLeft = txtr.getTexture("LigidPainter/Resources/Icons/ArrowLeft.jpg",0,0,false);
		icons.Painting = txtr.getTexture("LigidPainter/Resources/Icons/Painting.jpg",0,0,false);
		icons.Logo = txtr.getTexture("LigidPainter/Resources/Icons/ligidPainterlogo.jpg",0,0,false);
		icons.AddTexture = txtr.getTexture("LigidPainter/Resources/Icons/AddTexture.jpg",0,0,false);
		icons.Material = txtr.getTexture("LigidPainter/Resources/Icons/Material.jpg",0,0,false);
		icons.Circle = txtr.getTexture("LigidPainter/Resources/Icons/Circle.jpg",0,0,false);
		icons.TL = txtr.getTexture("LigidPainter/Resources/Sprites/TL.jpg",0,0,false);
		icons.TR = txtr.getTexture("LigidPainter/Resources/Sprites/TR.jpg",0,0,false);
		icons.BL = txtr.getTexture("LigidPainter/Resources/Sprites/BL.jpg",0,0,false);
		icons.BR = txtr.getTexture("LigidPainter/Resources/Sprites/BR.jpg",0,0,false);
	
		return icons;
	}
	
	BrushMaskTextures loadBrushMaskTextures(){
		GlSet glset;
		Texture txtr;
	
		std::vector<unsigned int> maskTextures;
		std::vector<std::string> maskTextureNames;
	
		const char* path = "./LigidPainter/Resources/Textures";
	
	
		for (const auto & entry : std::filesystem::directory_iterator(path)){
			glset.activeTexture(GL_TEXTURE1);//Raw mask
			std::string fileName = entry.path().string();
			if(fileName.size() > 3){
		 		if(fileName[fileName.size()-1] != 't' && fileName[fileName.size()-2] != 'x' && fileName[fileName.size()-3] != 't'){
		 			maskTextures.push_back(txtr.getTexture(fileName,0,0,false));
		 			maskTextureNames.push_back(fileName);
		 		}
		 	}
		}
	
		BrushMaskTextures brushMasks;
		brushMasks.names = maskTextureNames;
		brushMasks.textures = maskTextures;
	
		return brushMasks;
	}
	
	
	
	
	std::map<char, character> chars;
	
	void uploadChars() {
		GlSet glset;
		FT_Library ft;
		FT_Init_FreeType(&ft);
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
		FT_Face face;
		if (FT_New_Face(ft, "LigidPainter/Resources/fonts/arial.ttf", 0, &face)) {
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		}
		else {
			glset.activeTexture(GL_TEXTURE2);
			FT_Set_Pixel_Sizes(face, 0, 100);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
			// load first 128 characters of ASCII set
			for (unsigned char c = 0; c < 128; c++)
			{
				// Load character glyph 
				if (FT_Load_Char(face, c, FT_LOAD_RENDER))
				{
					std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
					continue;
				}
				
				// generate texture
				unsigned int texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RED,
					face->glyph->bitmap.width,
					face->glyph->bitmap.rows,
					0,
					GL_RED,
					GL_UNSIGNED_BYTE,
					face->glyph->bitmap.buffer
				);
	
				// set texture options
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
				// now store character for later use
				character Character = {
					texture,
					glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
					glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					static_cast<unsigned int>(face->glyph->advance.x)
				};
				chars.insert(std::pair<char, character>(c, Character));
			}
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	
		sendCharsToUI(chars);
	
		// destroy FreeType once we're finished
		FT_Done_Face(face);
		FT_Done_FreeType(ft);
	}
	
	
	InitializedTextures initTextures(const char* maskTexturePath){
		GlSet glset;
		Texture txtr;
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
		txtr.getTexture(maskTexturePath,0,0,false);
		glset.activeTexture(GL_TEXTURE12);//Modified mask
		txtr.getTexture(maskTexturePath,0,0,false);
	
		textures.albedoTxtr = albedoTxtr;
		textures.depthTexture = depthTexture;
		textures.enlargedTexture = enlargedTexture;
		textures.mirroredDepthTexture = mirroredDepthTexture;
	
		textures.modifiedMaskTexture = modifiedMaskTexture;
		textures.rawMaskTexture = rawMaskTexture;
	
		return textures;
	}

	Programs getProgram() {//Prepare shader program | Usen once

		GlSet gl;
		//Ui program
		unsigned int uiProgram = gl.createProgram("LigidPainter/Resources/Shaders/UI/ui");


		//Skybox program
		unsigned int skyboxProgram = gl.createProgram("LigidPainter/Resources/Shaders/3D/skybox");



		//Blur program
		unsigned int blurProgram = gl.createProgram("LigidPainter/Resources/Shaders/Texture Processing/Texture Generator/blur");



		//Icons program
		unsigned int iconsProgram = gl.createProgram("LigidPainter/Resources/Shaders/UI/icons");


		//Skybox Blur program
		unsigned int skyboxblurProgram = gl.createProgram("LigidPainter/Resources/Shaders/skyboxblur");



		//PBR program
		unsigned int PBRProgram = gl.createProgram("LigidPainter/Resources/Shaders/3D/PBR");



		//Saturation Value program
		unsigned int saturationValBoxProgram = gl.createProgram("LigidPainter/Resources/Shaders/UI/saturationValBox");



		//Saturation Value program
		unsigned int screenDepthProgram = gl.createProgram("LigidPainter/Resources/Shaders/Texture Processing/screenDepth");



		//Hue program
		unsigned int hueProgram = gl.createProgram("LigidPainter/Resources/Shaders/UI/hue");



		//Axis pointer program
		unsigned int axisPointerProgram = gl.createProgram("LigidPainter/Resources/Shaders/3D/axisPointer");



		//Out program
		unsigned int outProgram = gl.createProgram("LigidPainter/Resources/Shaders/Texture Processing/out");



		//Out program
		unsigned int twoDPaintingProgram = gl.createProgram("LigidPainter/Resources/Shaders/Texture Processing/2dPainting");


		Programs glPrograms;

		glPrograms.blurProgram = blurProgram;
		glPrograms.iconsProgram = iconsProgram;
		glPrograms.skyboxblurProgram = skyboxblurProgram;
		glPrograms.uiProgram = uiProgram;
		glPrograms.skyboxProgram = skyboxProgram;
		glPrograms.PBRProgram = PBRProgram;
		glPrograms.saturationValBoxProgram = saturationValBoxProgram;
		glPrograms.screenDepthProgram = screenDepthProgram;
		glPrograms.hueProgram = hueProgram;
		glPrograms.axisPointerProgram = axisPointerProgram;
		glPrograms.outProgram = outProgram;  
		glPrograms.twoDPaintingProgram = twoDPaintingProgram;  

		return glPrograms;
	}
};
#endif



