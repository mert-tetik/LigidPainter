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
#include "Core/Utilities.h"
#include "Core/UI/UserInterface.h"
#include "Core/gl.h"
#include "Core/Texture/Texture.h"
#include "Core/Load.hpp"

#include "Core/ProcessTheNodeFile.hpp"

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
		unsigned int cubePos;
	    for (unsigned int i = 0; i < faces.size(); i++)
	    {
			
			if(i == 0)
				cubePos = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
			if(i == 1)
				cubePos = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
			if(i == 2)
				cubePos = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
			if(i == 3)
				cubePos = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
			if(i == 4)
				cubePos = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
			if(i == 5)
				cubePos = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;


	        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
	        if (data)
	        {

	            glTexImage2D(cubePos, 
	                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
	            );
	            stbi_image_free(data);
				std::cout << "Loaded " << faces[i] << std::endl;
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
		//px,nx,ny,py,pz,nz
	}
	
	Cubemaps loadCubemaps(){
		Texture txtr;
		std::vector<std::string> faces
		{
		    "LigidPainter/Resources/Cubemap/Skybox/sky2/px.png",
		    "LigidPainter/Resources/Cubemap/Skybox/sky2/nx.png",
		    "LigidPainter/Resources/Cubemap/Skybox/sky2/ny.png",
		    "LigidPainter/Resources/Cubemap/Skybox/sky2/py.png",
		    "LigidPainter/Resources/Cubemap/Skybox/sky2/pz.png",
		    "LigidPainter/Resources/Cubemap/Skybox/sky2/nz.png"
		};
		unsigned int cubemapTexture = loadCubemap(faces,GL_TEXTURE13);  
		
		std::vector<std::string> facesblur
		{
		    "LigidPainter/Resources/Cubemap/Skybox/ambient/pxb.png",
		    "LigidPainter/Resources/Cubemap/Skybox/ambient/nxb.png",
		    "LigidPainter/Resources/Cubemap/Skybox/ambient/nyb.png",
		    "LigidPainter/Resources/Cubemap/Skybox/ambient/pyb.png",
		    "LigidPainter/Resources/Cubemap/Skybox/ambient/pzb.png",
		    "LigidPainter/Resources/Cubemap/Skybox/ambient/nzb.png"
		};
		unsigned int cubemapTextureblur = loadCubemap(facesblur,GL_TEXTURE13);  

	
		Cubemaps cubemaps;
		cubemaps.blurycubemap = cubemapTextureblur;
		cubemaps.cubemap = cubemapTexture;
		// cubemaps.cubemap2 = cubemapTexture2;
		// cubemaps.cubemap3 = cubemapTexture3;
		// cubemaps.cubemap4 = cubemapTexture4;
		// cubemaps.cubemap5 = cubemapTexture5;
		// cubemaps.cubemap6 = cubemapTexture6;
	
		return cubemaps;
	}
	
	LigidCursors loadCursors(){
	
		GLFWimage images[1];
		stbi_set_flip_vertically_on_load(false);
		images[0].pixels = stbi_load("LigidPainter/Resources/Ico/PointerIcon.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
		GLFWcursor* pointerCursor = glfwCreateCursor(images,15,0);
		stbi_image_free(images[0].pixels);
	
		images[0].pixels = stbi_load("LigidPainter/Resources/Ico/DefaultIcon.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
		GLFWcursor* defaultCursor = glfwCreateCursor(images,7,0);
		stbi_image_free(images[0].pixels);
	
		images[0].pixels = stbi_load("LigidPainter/Resources/Ico/DropperCursor.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
		GLFWcursor* dropperCursor = glfwCreateCursor(images,0,30);
		stbi_image_free(images[0].pixels);
		
		images[0].pixels = stbi_load("LigidPainter/Resources/Ico/hSlideCursor.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
		GLFWcursor* hSlideCursor = glfwCreateCursor(images,15,0);
		stbi_image_free(images[0].pixels);
		
		images[0].pixels = stbi_load("LigidPainter/Resources/Ico/vSlideCursor.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
		GLFWcursor* vSlideCursor = glfwCreateCursor(images,15,0);
		stbi_image_free(images[0].pixels);
	
		std::cout << "Loaded Cursors" << std::endl; 


		LigidCursors cursors;
		cursors.pointerCursor = pointerCursor;
		cursors.defaultCursor = defaultCursor;
		cursors.dropperCursor = dropperCursor;
		cursors.hSlideCursor = hSlideCursor;
		cursors.vSlideCursor = vSlideCursor;
	
		return cursors;
	}
	
	Icons loadIcons(){
		GlSet glset;
		Texture txtr;
		Icons icons;
	
		glset.activeTexture(GL_TEXTURE6);
		icons.dropperIcon = txtr.getTexture("LigidPainter/Resources/Icons/Dropper.jpg",0,0,false);
		icons.TDModel = txtr.getTexture("LigidPainter/Resources/Icons/3DModel.li",0,0,false);
		icons.ColorPicker = txtr.getTexture("LigidPainter/Resources/Icons/ColorPicker.jpg",0,0,false);
		icons.Export = txtr.getTexture("LigidPainter/Resources/Icons/Export.jpg",0,0,false);
		icons.Folder = txtr.getTexture("LigidPainter/Resources/Icons/Folder.jpg",0,0,false);
		icons.ImportMask = txtr.getTexture("LigidPainter/Resources/Icons/ImportMask.jpg",0,0,false);
		icons.JpgFile = txtr.getTexture("LigidPainter/Resources/Icons/JpgFile.jpg",0,0,false);
		icons.MaskGausBlur = txtr.getTexture("LigidPainter/Resources/Icons/MaskGausBlur.png",0,0,false);
		icons.Mirror = txtr.getTexture("LigidPainter/Resources/Icons/Mirror.jpg",0,0,false);
		icons.Panel = txtr.getTexture("LigidPainter/Resources/Icons/Panel.jpg",0,0,false);
		icons.PngFile = txtr.getTexture("LigidPainter/Resources/Icons/PngFile.jpg",0,0,false);
		icons.Sphere = txtr.getTexture("LigidPainter/Resources/Icons/Sphere.jpg",0,0,false);
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
		icons.BLR = txtr.getTexture("LigidPainter/Resources/Sprites/BLR.jpg",0,0,false);
		icons.BRR = txtr.getTexture("LigidPainter/Resources/Sprites/BRR.jpg",0,0,false);
		icons.PanelButtonL = txtr.getTexture("LigidPainter/Resources/Sprites/PanelButtonL.jpg",0,0,false);
		icons.PanelButtonR = txtr.getTexture("LigidPainter/Resources/Sprites/PanelButtonR.jpg",0,0,false);
		icons.Plus = txtr.getTexture("LigidPainter/Resources/Icons/Plus.jpg",0,0,false);
		icons.Minus = txtr.getTexture("LigidPainter/Resources/Icons/Minus.jpg",0,0,false);
		icons.ArrowDown = txtr.getTexture("LigidPainter/Resources/Icons/ArrowDown.jpg",0,0,false);
		icons.Texture = txtr.getTexture("LigidPainter/Resources/Icons/Texture.jpg",0,0,false);
		icons.X = txtr.getTexture("LigidPainter/Resources/Icons/X.jpg",0,0,false);
		icons.O = txtr.getTexture("LigidPainter/Resources/Icons/O.jpg",0,0,false);
		icons.CircularX = txtr.getTexture("LigidPainter/Resources/Icons/CircularX.jpg",0,0,false);
		icons.Undo = txtr.getTexture("LigidPainter/Resources/Icons/Undo.jpg",0,0,false);
		icons.LoadFolder = txtr.getTexture("LigidPainter/Resources/Icons/AddFolder.jpg",0,0,false);
		icons.Gear = txtr.getTexture("LigidPainter/Resources/Icons/Gear.jpg",0,0,false);
		icons.Mark = txtr.getTexture("LigidPainter/Resources/Icons/Mark.jpg",0,0,false);
		icons.TextureGenerator = txtr.getTexture("LigidPainter/Resources/Icons/TextureGenerator.jpg",0,0,false);
		icons.Pointer = txtr.getTexture("LigidPainter/Resources/Icons/Pointer.jpg",0,0,false);
		icons.Trash = txtr.getTexture("LigidPainter/Resources/Icons/Trash.jpg",0,0,false);
		icons.ThreeDProject = txtr.getTexture("LigidPainter/Resources/Icons/3DProject.jpg",0,0,false);
		icons.TwoDProject = txtr.getTexture("LigidPainter/Resources/Icons/2DProject.jpg",0,0,false);
		icons.ImportProject = txtr.getTexture("LigidPainter/Resources/Icons/ImportProject.jpg",0,0,false);
		icons.MaterialImport = txtr.getTexture("LigidPainter/Resources/Icons/MaterialImport.jpg",0,0,false);
		icons.MaterialExport = txtr.getTexture("LigidPainter/Resources/Icons/MaterialExport.jpg",0,0,false);
		icons.AlignLeft = txtr.getTexture("LigidPainter/Resources/Icons/AlignLeft.jpg",0,0,false);
		icons.AlignMid = txtr.getTexture("LigidPainter/Resources/Icons/AlignMid.jpg",0,0,false);
		icons.AlignRight = txtr.getTexture("LigidPainter/Resources/Icons/AlignRight.jpg",0,0,false);
		icons.ExportFolder = txtr.getTexture("LigidPainter/Resources/Icons/ExportFolder.jpg",0,0,false);
		icons.BrushFolder = txtr.getTexture("LigidPainter/Resources/Icons/BrushFolder.png",0,0,false);
		icons.LCircle = txtr.getTexture("LigidPainter/Resources/Icons/LCircle.jpg",0,0,false);
		icons.RCircle = txtr.getTexture("LigidPainter/Resources/Icons/RCircle.jpg",0,0,false);
	
		return icons;
	}
	
	BrushTexture loadBrushMaskTextures(std::vector<aTexture> &albedoTextures){
		GlSet glset;
		Texture txtr;
	
		std::vector<aTexture> maskTextures;
		std::vector<aTexture> clrTextures;
		std::vector<aTexture> normalTextures;
	
		const char* maskpath = "./LigidPainter/Resources/Textures/Mask";

		for (const auto & entry : std::filesystem::directory_iterator(maskpath)){
			glset.activeTexture(GL_TEXTURE1);//Raw mask
			std::string fileName = entry.path().string();
			if(fileName.size() > 3){
		 		if(fileName[fileName.size()-1] != 't' && fileName[fileName.size()-2] != 'x' && fileName[fileName.size()-3] != 't'){
		 			aTexture brushTxtr;
					brushTxtr.id = txtr.getTexture(fileName,0,0,false);
					brushTxtr.name = fileName;
					brushTxtr.folderIndex = 3;
					albedoTextures.push_back(brushTxtr);
		 		}
		 	}
		}
		const char* colorpath = "./LigidPainter/Resources/Textures/Sticker";

		for (const auto & entry : std::filesystem::directory_iterator(colorpath)){
			glset.activeTexture(GL_TEXTURE1);//Raw mask
			std::string fileName = entry.path().string();
			if(fileName.size() > 3){
		 		if(fileName[fileName.size()-1] != 't' && fileName[fileName.size()-2] != 'x' && fileName[fileName.size()-3] != 't'){
		 			aTexture brushTxtr;
					brushTxtr.id = txtr.getTexture(fileName,0,0,false);
					brushTxtr.name = fileName;
					brushTxtr.folderIndex = 4;
					albedoTextures.push_back(brushTxtr);
		 		}
		 	}
		}

		const char* normalpath = "./LigidPainter/Resources/Textures/NormalMap";

		for (const auto & entry : std::filesystem::directory_iterator(normalpath)){
			glset.activeTexture(GL_TEXTURE1);//Raw mask
			std::string fileName = entry.path().string();
			if(fileName.size() > 3){
		 		if(fileName[fileName.size()-1] != 't' && fileName[fileName.size()-2] != 'x' && fileName[fileName.size()-3] != 't'){
		 			aTexture brushTxtr;
					brushTxtr.id = txtr.getTexture(fileName,0,0,false);
					brushTxtr.name = fileName;
					brushTxtr.folderIndex = 5;
					albedoTextures.push_back(brushTxtr);
		 		}
		 	}
		}
	
		BrushTexture brushTextures;
		brushTextures.maskTextures = maskTextures;
		brushTextures.colorTextures = clrTextures;
		brushTextures.normalTextures = normalTextures;
		return brushTextures;
	}
	
	
	
	
	std::map<char, character> chars;
	
	void uploadChars() {
		GlSet glset;
		FT_Library ft;
		FT_Init_FreeType(&ft);
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
		FT_Face face;
		std::string fontFile = "arial.ttf";
		if (FT_New_Face(ft, ("LigidPainter/Resources/fonts/" + fontFile).c_str(), 0, &face)) {
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
			std::cout << "Loaded " << fontFile << std::endl; 
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
		textures.enlargedTexture = enlargedTexture;
	
		textures.modifiedMaskTexture = modifiedMaskTexture;
		textures.rawMaskTexture = rawMaskTexture;
	
		return textures;
	}

	Programs getProgram() {//Prepare shader program | Usen once

		GlSet gl;
		//Ui program
		unsigned int uiProgram = gl.createProgram("./LigidPainter/Resources/Shaders/UI/ui");


		//Skybox program
		unsigned int skyboxProgram = gl.createProgram("./LigidPainter/Resources/Shaders/3D/skybox");



		//Blur program
		unsigned int blurProgram = gl.createProgram("./LigidPainter/Resources/Shaders/Texture Processing/Texture Generator/blur");



		//Icons program
		unsigned int iconsProgram = gl.createProgram("./LigidPainter/Resources/Shaders/UI/icons");


		//Skybox Blur program
		unsigned int skyboxblurProgram = gl.createProgram("./LigidPainter/Resources/Shaders/skyboxblur");



		//PBR program
		unsigned int PBRProgram = gl.createProgram("./LigidPainter/Resources/Shaders/3D/PBR");



		//Saturation Value program
		unsigned int saturationValBoxProgram = gl.createProgram("./LigidPainter/Resources/Shaders/UI/saturationValBox");



		//Saturation Value program
		unsigned int screenDepthProgram = gl.createProgram("./LigidPainter/Resources/Shaders/Texture Processing/screenDepth");



		//Hue program
		unsigned int hueProgram = gl.createProgram("./LigidPainter/Resources/Shaders/UI/hue");



		//Axis pointer program
		unsigned int axisPointerProgram = gl.createProgram("./LigidPainter/Resources/Shaders/3D/axisPointer");



		//Out program
		unsigned int outProgram = gl.createProgram("./LigidPainter/Resources/Shaders/Texture Processing/out");



		//Out program
		unsigned int twoDPaintingProgram = gl.createProgram("./LigidPainter/Resources/Shaders/Texture Processing/2dPainting");



		//renderTheTexture Program
		unsigned int renderTheTextureProgram = gl.createProgram("./LigidPainter/Resources/Shaders/UI/renderTheTexture");



		//Prefiltered map
		unsigned int prefilterMapProgram = gl.createProgram("./LigidPainter/Resources/Shaders/3D/prefilterMap");
		
		
		
		//BRDF
		unsigned int brdfProgram = gl.createProgram("./LigidPainter/Resources/Shaders/3D/BRDF");
		
		
		
		//Blury skybox
		unsigned int blurySkyboxProgram = gl.createProgram("./LigidPainter/Resources/Shaders/skyboxblur");



		//Normal map generator
		unsigned int normalGenProgram = gl.createProgram("./LigidPainter/Resources/Shaders/Texture Processing/Texture Generator/normalGen");




		//Normal map generator
		unsigned int noisyTextureProgram = gl.createProgram("./LigidPainter/Resources/Shaders/Texture Processing/Texture Generator/noisyTexture");
		
		
		
		
		//Curve 
		unsigned int curveProgram = gl.createProgram("./LigidPainter/Resources/Shaders/UI/curve");
		
		
		
		
		//Ramp 
		unsigned int rampProgram = gl.createProgram("./LigidPainter/Resources/Shaders/UI/ramp");




		//Dynamic painting program
		unsigned int dynamicPaintingProgram = gl.createProgram("./LigidPainter/Resources/Shaders/Texture Processing/Texture Generator/dynamicPainting");
		
		
		
		
		//Dynamic painting program
		unsigned int paintOverProgram = gl.createProgram("./LigidPainter/Resources/Shaders/UI/paintover");




		//Texture Displayer program
		unsigned int textureDisplayer = gl.createProgram("./LigidPainter/Resources/Shaders/UI/textureDisplayer");
		
		
		
		
		//brushCursor program
		unsigned int brushCursor = gl.createProgram("./LigidPainter/Resources/Shaders/UI/brushCursor");
		
		
		
		
		//materialResult program
		unsigned int materialResult = gl.createProgram("./LigidPainter/Resources/Shaders/3D/materialResult");
		
		
		
		
		//materialResult program
		unsigned int dotsProgram = gl.createProgram("./LigidPainter/Resources/Shaders/UI/dots");
		
		
		
		//Render textures blury
		unsigned int renderTheTextureBlur = gl.createProgram("./LigidPainter/Resources/Shaders/UI/renderTheTextureBlur");
		
		

		
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
		glPrograms.renderTheTextureProgram = renderTheTextureProgram;   
		glPrograms.prefilterMapProgram = prefilterMapProgram;   
		glPrograms.brdfProgram = brdfProgram;   
		glPrograms.blurySkyboxProgram = blurySkyboxProgram;   
		glPrograms.normalGenProgram = normalGenProgram;   
		glPrograms.noisyTextureProgram = noisyTextureProgram;
		glPrograms.curveProgram = curveProgram;
		glPrograms.rampProgram = rampProgram;
		glPrograms.dynamicPaintingProgram = dynamicPaintingProgram;
		glPrograms.paintOverProgram = paintOverProgram;
		glPrograms.textureDisplayer = textureDisplayer;
		glPrograms.brushCursor = brushCursor;
		glPrograms.materialResult = materialResult;
		glPrograms.dotsProgram = dotsProgram;
		glPrograms.renderTheTextureBlur = renderTheTextureBlur;


		return glPrograms;
	}
	Node createNode(std::string nodeName){
		processNode process;
		ProcessHppNode node;
		node = process.processNodeFile("./LigidPainter/Resources/Nodes/" + nodeName + ".node");

		Node resultNode;
		resultNode.title = node.title;

		resultNode.upBarColor.r = node.color[0]/255.f;
		resultNode.upBarColor.g = node.color[1]/255.f; 
		resultNode.upBarColor.b = node.color[2]/255.f;
		resultNode.upBarColor.a = 1.f;

		resultNode.useModel = node.useModel;

		for (size_t i = 0; i < node.lists.size(); i++)
		{
			ListBox listbox;
			for (size_t ii = 0; ii < node.lists[i].size()-1; ii++)
			{
				listbox.elements.push_back(node.lists[i][ii]);
			}
			resultNode.listBoxes.push_back(listbox);
		}

		float rangeBarCount = 0;
		//Load inputs
		for (int i = 0; i < node.inputs.size(); i++)
		{
			NodeInput input;

			input.element = node.inputs[i].element;
			input.text = node.inputs[i].title;
			input.type = node.inputs[i].type;

			
			if(input.element == "range"){
				if(input.type == "float"){
					input.value.r = stof(node.inputs[i].values[0])/ 255.f;
				}
				else if(input.type == "vec2"){
					input.value.r = stof(node.inputs[i].values[0])/ 255.f;
					input.value.g = stof(node.inputs[i].values[1])/ 255.f;
				}		
				else if(input.type == "vec3"){
					input.value.r = stof(node.inputs[i].values[0])/ 255.f;
					input.value.g = stof(node.inputs[i].values[1])/ 255.f;
					input.value.b = stof(node.inputs[i].values[2])/ 255.f;
				}	
			}
			else{
				if(input.type == "float"){
					input.color.r = stof(node.inputs[i].values[0]);
				}
				else if(input.type == "vec2"){
					input.color.r = stof(node.inputs[i].values[0]);
					input.color.g = stof(node.inputs[i].values[1]);
				}		
				else if(input.type == "vec3"){
					input.color.r = stof(node.inputs[i].values[0]);
					input.color.g = stof(node.inputs[i].values[1]);
					input.color.b = stof(node.inputs[i].values[2]);
				}	
			}

			//node.inputs[i].list;
			//node.inputs[i].listIndex;

			resultNode.inputs.push_back(input);
			if(resultNode.inputs[i].type == "float"){
				if(resultNode.inputs[i].element == "range")
					rangeBarCount += 1.3f;

				resultNode.inputs[i].rangeBarsPointerPressed.push_back(false);
			}
			if(resultNode.inputs[i].type == "vec2"){
				if(resultNode.inputs[i].element == "range")
					rangeBarCount += 1.3f*2;

				if(resultNode.inputs[i].element == "image" || resultNode.inputs[i].element == "color")
					rangeBarCount += 1.3f;
				resultNode.inputs[i].rangeBarsPointerPressed.push_back(false);
				resultNode.inputs[i].rangeBarsPointerPressed.push_back(false);
			}
			if(resultNode.inputs[i].type == "vec3"){
				if(resultNode.inputs[i].element == "range")
					rangeBarCount += 1.3f*3;
				if(resultNode.inputs[i].element == "image" || resultNode.inputs[i].element == "color")
					rangeBarCount += 1.3f;
				resultNode.inputs[i].rangeBarsPointerPressed.push_back(false);
				resultNode.inputs[i].rangeBarsPointerPressed.push_back(false);
				resultNode.inputs[i].rangeBarsPointerPressed.push_back(false);
			}

			if(input.element == "ramp"){
				rangeBarCount += 1.3f*8;
			}

		}
		if(resultNode.listBoxes.size())
			rangeBarCount += 1.3f;
		for (size_t i = 0; i < resultNode.listBoxes.size(); i++)
		{
			rangeBarCount += 1.3f;
		}
		

		resultNode.rangeBarCount = rangeBarCount;
		//Load outputs
		for (int i = 0; i < node.outputs.size(); i++){
			NodeInput output;

			output.text = node.outputs[i].title;
			output.type = node.outputs[i].type;

			resultNode.outputs.push_back(output);
		}
		//node.lists

		resultNode.backColor = glm::vec4(0.2,0.2,0.2,0.5);
		//resultNode.positionX = spawningPosX;
		//resultNode.positionY = spawningPosY;
		resultNode.title = nodeName;
		resultNode.width = 0.12f;

		//UPDATE OTHERS
		const char* defaultVertexShader = 
			"#version 330 core\n"
			"layout(location = 0) in vec3 aPos;\n"
			"layout(location = 1) in vec3 aNormal;\n"
			"layout(location = 2) in vec2 aTexCoords;\n"
			"layout(location = 3) in vec3 aTangent;\n"
			"layout(location = 4) in vec3 aBitangent;\n"
			
			"uniform mat4 view;\n"
			"uniform mat4 projection;\n"
			"uniform mat4 modelMatrix;\n"
			"uniform int is3D;\n"
			
			"out vec2 tex_coords;\n"
			"out vec3 normal;\n"
			"out vec3 posModel;\n"
			"out vec4 posScene;\n"
			"out vec3 tangent;\n"
			"out vec3 bitangent;\n"
			
			"void main() {\n"
				"tangent = aTangent;\n"
				"bitangent = aBitangent;\n"
			    "posModel = aPos;\n"
			    "tex_coords = aTexCoords;\n"
			    "normal = aNormal;\n"
				"vec4 res;\n"
				"if(is3D == 1){\n"
					"vec4 tPos = modelMatrix * vec4(aPos,1.0);\n"
			    	"res = projection * view * vec4(tPos.xyz, 1.0);\n"
				"}\n"
				"else{\n"
			    	"res = projection * vec4(tex_coords,0, 1);\n" 
				"}\n"
				"posScene = projection * view * vec4(aPos, 0.5);\n"
			    "gl_Position = res;\n"
			"}\0";

		//Compile the fragment shader
		const char* shaderSource = node.code.c_str();
		unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragShader, 1, &shaderSource, NULL);
		glCompileShader(fragShader);
		//Test the shader
		int success;
		char infoLog[512];
		glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
		//Print the error if any occurs
		if (!success){glGetShaderInfoLog(fragShader, 512, NULL, infoLog);std::cout << "ERROR::SHADER::COMPILATION_FAILED " << infoLog << std::endl;};

		//Compile the vertex shader
		unsigned int vertShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertShader, 1, &defaultVertexShader, NULL);
		glCompileShader(vertShader);

		//Test the shader
		glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
		//Print the error if any occurs
		if (!success){glGetShaderInfoLog(vertShader, 512, NULL, infoLog);std::cout << "ERROR::SHADER::COMPILATION_FAILED " << infoLog << std::endl;};

		unsigned int program = glCreateProgram();
		glAttachShader(program, vertShader);
		glAttachShader(program, fragShader);
		glLinkProgram(program);

		glDeleteShader(vertShader);
		glDeleteShader(fragShader);

		resultNode.program = program;

		resultNode.fragSource = shaderSource;

		return resultNode;
	}

	std::vector<Node> loadNodes(char folderDistinguisher){
		GlSet glset;
		Texture txtr;
		Utilities util;
		std::vector<Node> appNodes;

		const char* path = "./LigidPainter/Resources/Nodes";
	
	
		for (const auto & entry : std::filesystem::directory_iterator(path)){
			std::string fileName = entry.path().string();
			std::string file = util.getLastWordBySeparatingWithChar(fileName,folderDistinguisher);
			std::string raw = file;

			for (size_t i = 0; i < 5; i++)
			{
				raw.pop_back();
			}
			
			appNodes.push_back(createNode(raw));				
		}
		return appNodes;
	}
	std::vector<Node> createOutputNode(std::vector<Node> appNodes){	
		ColorData colorData;

		std::vector<Node> result;


		//Unique output node
		Node mainOutNode;
		mainOutNode.backColor = colorData.nodePanelColor;
		mainOutNode.height = 0.05f;
		mainOutNode.width = 0.12f;
		mainOutNode.title = "Shader Out";

		NodeInput mainOutInput;
		mainOutInput.type = "vec3";
		mainOutInput.text = "Shader";
		mainOutInput.element = "none";
		mainOutInput.nodeConnectionIndex = 1;
		mainOutInput.inputConnectionIndex = 0;
		mainOutInput.pressed = false;

		mainOutNode.inputs.push_back(mainOutInput);
		mainOutNode.isMainOut = true;
		mainOutNode.positionX = 0;
		mainOutNode.positionY = -0.f;
		mainOutNode.rangeBarCount = 0;
		mainOutNode.upBarColor = glm::vec4(0,0,0,1);

		//Add to material
		result.push_back(mainOutNode);
		
		//Pull the 01 PBR node 
		NodeConnection connection;
		connection.nodeConnectionIndex = 0;
		connection.inputConnectionIndex = 0;
		connection.connectionPressed = false;

		Node pbrNode;
		for (size_t i = 0; i < appNodes.size(); i++)
		{
			if(appNodes[i].title == "01 PBR"){
				pbrNode = appNodes[i];
				break;
			}
		}
		

		pbrNode.outputs[0].connections.clear();
		pbrNode.outputs[0].connections.push_back(connection);

		pbrNode.outputs[0].isConnectedToShaderInput = true;
		pbrNode.outputs[0].pressed = false;
		pbrNode.outputs[0].connectionHover = false;
		
		pbrNode.stateChanged = true;
		pbrNode.marked = true;

		pbrNode.positionX = -0.55;
		pbrNode.positionY =-0.f;

		//Add to material
		result.push_back(pbrNode);

		return result;
	}

	unsigned int createPrefilterMap(Programs programs,Cubemaps &cubemaps,int windowMaxWidth, int windowMaxHeight){
		GlSet glset;
		
		//Is required for rendering the skybox
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL);

		//Coordinates necessary to render a skybox
		std::vector<float> cube = {
    	    // back face
    	    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
    	     1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
    	     1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
    	     1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
    	    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
    	    -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
    	    // front face
    	    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
    	     1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
    	     1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
    	     1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
    	    -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
    	    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
    	    // left face
    	    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
    	    -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
    	    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
    	    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
    	    -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
    	    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
    	    // right face
    	     1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
    	     1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
    	     1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
    	     1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
    	     1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
    	     1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
    	    // bottom face
    	    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
    	     1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
    	     1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
    	     1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
    	    -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
    	    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
    	    // top face
    	    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
    	     1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
    	     1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
    	     1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
    	    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
    	    -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
    	};

		//Skybox will be rendered to that FBO
		unsigned int captureFBO, captureRBO;
		glGenFramebuffers(1, &captureFBO);
		glGenRenderbuffers(1, &captureRBO);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

		
		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		//For rendering every side of the skybox
		glm::mat4 captureViews[] = 
		{
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};	


			//Create the cube map texture
			glActiveTexture(GL_TEXTURE28);
			unsigned int prefilterMap;
			glGenTextures(1, &prefilterMap);
			glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
			for (unsigned int txtrI = 0; txtrI < 6; ++txtrI)
			{
				//Every side
    			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + txtrI, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
			}

			//Texture Parameters
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//Generate mipmap after creating the txtr
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);


			//Use the related program for rendering
			glUseProgram(programs.prefilterMapProgram);
			//Slot 13 = skybox
			glset.uniform1i(programs.prefilterMapProgram,"environmentMap",13);
			glset.uniformMatrix4fv(programs.prefilterMapProgram,"projection",captureProjection);

			//Bind the skybox to the Slot 13
			glActiveTexture(GL_TEXTURE13);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemaps.cubemap);

			//Bind the framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

			//5 levels will be created
			unsigned int maxMipLevels = 5;
			for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
			{
				//Every level

			    //resize framebuffer according to mip-level size.
			    unsigned int mipWidth  = 128 * std::pow(0.5, mip);
			    unsigned int mipHeight = 128 * std::pow(0.5, mip);
			    glViewport(0, 0, mipWidth, mipHeight);

				//Adjust the roughness value
			    float roughness = (float)mip / (float)(maxMipLevels - 1);
				glset.uniform1f(programs.prefilterMapProgram,"roughness",roughness);

				//Create the texture
			    for (unsigned int txtrI = 0; txtrI < 6; ++txtrI)
			    {
					//Every side

					glset.uniformMatrix4fv(programs.prefilterMapProgram,"view",captureViews[txtrI]);
			        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
			                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + txtrI, prefilterMap, mip);

			        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			        glset.drawArrays(cube,false);
			    }
			}
		
		//Set everything to default
		glBindFramebuffer(GL_FRAMEBUFFER, 0); 
		glViewport(0, 0, windowMaxWidth, windowMaxHeight);

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);  

		//Bind the result cube map to 16th slote
		glActiveTexture(GL_TEXTURE16);
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);

		//Delete the framebuffer
		glDeleteFramebuffers(1,&captureFBO);
		glDeleteRenderbuffers(1,&captureRBO);

		return prefilterMap;
	}

	// unsigned int createPrefilterMap(Programs programs,Cubemaps cubemaps,WindowData windowData){

	// }

	void getDefaultNodeScene(std::vector<NodeScene> &nodeScenes,std::vector<Node> appNodes,const char* name){
		std::vector<Node> mainOutNodes;
		mainOutNodes = createOutputNode(appNodes);		

		NodeScene emptyNodeScene;
		emptyNodeScene.index = 0;
		emptyNodeScene.sceneName = name;
		emptyNodeScene.nodes = mainOutNodes;
		emptyNodeScene.stateChanged = true;
		emptyNodeScene.nodes[0].positionX = 0;
		emptyNodeScene.nodes[0].positionY = 0;

		GlSet glset;
		glActiveTexture(GL_TEXTURE28);
		unsigned int renderTexture;
		glset.genTextures(renderTexture);
		glset.bindTexture(renderTexture);
		glset.texImage(nullptr,100,100,GL_RGBA);
		glset.generateMipmap();

		emptyNodeScene.renderedTexture = renderTexture;

		unsigned int outTexture;
		glset.genTextures(outTexture);
		glset.bindTexture(outTexture);
		glset.texImage(nullptr,1024,1024,GL_RGBA);
		glset.generateMipmap();

		emptyNodeScene.outTexture = outTexture;

		nodeScenes.push_back(emptyNodeScene);
	}
	unsigned int getPaintingFBO(WindowData windowData,unsigned int screenPaintingTexture){
		GlSet glset;

		//Framebuffer used in drawToScreen.cpp
		unsigned int paintingFBO;
		glset.genFramebuffers(paintingFBO);
		glset.bindFramebuffer(paintingFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  screenPaintingTexture, 0);

		unsigned int RBO;
		glset.genRenderbuffers(RBO);
		glset.bindRenderBuffer(RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, windowData.windowMaxWidth, windowData.windowMaxHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

		glClear(GL_DEPTH_BUFFER_BIT);

		glset.bindFramebuffer(0);
		glset.bindRenderBuffer(0);

		return paintingFBO;
	}
	unsigned int createScreenFrameBufferObject(int maxScreenWidth,int maxScreenHeight) {
		GlSet gl;
		gl.activeTexture(GL_TEXTURE5);
		unsigned int FBO;
		gl.genFramebuffers(FBO);
		gl.bindFramebuffer(FBO);
		unsigned int textureColorbuffer;
		gl.genTextures(textureColorbuffer);
		gl.bindTexture(textureColorbuffer);
		gl.texImage(NULL, 1920,1080,GL_RGBA);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
		unsigned int RBO;
		gl.genRenderbuffers(RBO);
		gl.bindRenderBuffer(RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return FBO;
	}
	unsigned int getBrdflutTexture(){
		Texture txtr;
		GlSet glset;

		glActiveTexture(GL_TEXTURE15);
		unsigned int BRDFTexture = txtr.getTexture("LigidPainter/Resources/Source/ibl_brdf_lut.png",540,540,false);
		glset.bindTexture(BRDFTexture);

		return BRDFTexture;
	}
};


#endif