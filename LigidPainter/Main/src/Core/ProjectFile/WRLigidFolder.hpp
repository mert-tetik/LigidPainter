#ifndef LIGID_WRITE_READ_FOLDER_FILE 
#define LIGID_WRITE_READ_FOLDER_FILE

#include <iostream>
#include <ostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "../../thirdparty/include/glad/glad.h"

#include "Core/Model/model.h"
#include "Core/UI/UserInterface.h"
#include "Core/Texture/Texture.h"
#include "Core/gl.h"
#include "Core/Load.hpp"
#include "Core/ProjectFile/WRLigidMaterialFile.hpp"

#include "tinyfiledialogs.h"



class ProjectFolder{
private:
    void writeSettingsFile(std::string path,std::string tdModelName,int chosenSkyboxIndex,int chosenTxtResIndex,std::vector<UIElement> &UIElements){
        #if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
		#else
			char folderDistinguisher = '/'; 
		#endif

        Utilities util;

        std::string fileName = "ProjectSettings.settings";
        std::ofstream wf(path + folderDistinguisher + fileName, std::ios::out | std::ios::binary);

		
        if(!wf) {
            std::cout << "ERROR while creating the project settings file! : " << path << std::endl;
            return;
        }

        uint64_t h1 = 0x0000AA; //Version 1.4 
        wf.write(reinterpret_cast<char*>(&h1),sizeof(uint64_t));
    
        wf.write(reinterpret_cast<char*>(&UIElements[UIuseUVCheckBox].checkBox.checked),sizeof(bool));// Use uv checkbox
        wf.write(reinterpret_cast<char*>(&UIElements[UIbackfaceCullingCheckBox].checkBox.checked),sizeof(bool));// Backface culling checkbox
        wf.write(reinterpret_cast<char*>(&UIElements[UIstabilizeFpsCheckBox].checkBox.checked),sizeof(bool));// Sync checkbox
        wf.write(reinterpret_cast<char*>(&UIElements[UIrealtimeMaterialRenderingCheckBox].checkBox.checked),sizeof(bool));// Sync checkbox
        wf.write(reinterpret_cast<char*>(&UIElements[UITDModelSizeRangeBarElement].rangeBar.value),sizeof(float));// 3D Model scale rangebar
        wf.write(reinterpret_cast<char*>(&UIElements[UITDModelPosXRangeBarElement].rangeBar.value),sizeof(float));// 3D Model position X rangebar
        wf.write(reinterpret_cast<char*>(&UIElements[UITDModelPosYRangeBarElement].rangeBar.value),sizeof(float));// 3D Model position Y rangebar
        wf.write(reinterpret_cast<char*>(&UIElements[UITDModelPosZRangeBarElement].rangeBar.value),sizeof(float));// 3D Model position Z rangebar
        wf.write(reinterpret_cast<char*>(&chosenSkyboxIndex),sizeof(int));// Selected skybox index
        wf.write(reinterpret_cast<char*>(&chosenTxtResIndex),sizeof(int));// Selected txtrres index

        std::string model = util.getLastWordBySeparatingWithChar(tdModelName,folderDistinguisher);

        uint64_t modelSize = model.size(); 
        wf.write(reinterpret_cast<char*>(&modelSize),sizeof(uint64_t));

        for (size_t i = 0; i < modelSize; i++) 
        {
            wf.write(reinterpret_cast<char*>(&model[i]),sizeof(char));// 3D Model
        }

        //TODO Export path
    }
    void readSettingsFile(std::string path,int &chosenSkyboxIndex,int& chosenTxtResIndex,ifstream &stRf,std::string &setting,std::vector<UIElement> &UIElements,Model &model){
        #if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
		#else
			char folderDistinguisher = '/'; 
		#endif

        Utilities util;
        
        std::string fileName = "ProjectSettings.settings";
        std::ifstream rf(path + folderDistinguisher + fileName, std::ios::in | std::ios::binary);

        if(!rf){
            std::cout << "ERROR Can't open file " << path + folderDistinguisher + fileName << std::endl;
            return;
        }

        uint64_t h1 = 0x0000AA; //Version 1.4 
        uint64_t r1;
        rf.read(reinterpret_cast<char*>(&r1),sizeof(uint64_t));
        if(r1 == h1){
            rf.read(reinterpret_cast<char*>(&UIElements[UIuseUVCheckBox].checkBox.checked),sizeof(bool));// Use uv checkbox
            rf.read(reinterpret_cast<char*>(&UIElements[UIbackfaceCullingCheckBox].checkBox.checked),sizeof(bool));// Backface culling checkbox
            rf.read(reinterpret_cast<char*>(&UIElements[UIstabilizeFpsCheckBox].checkBox.checked),sizeof(bool));// Sync checkbox
            rf.read(reinterpret_cast<char*>(&UIElements[UIrealtimeMaterialRenderingCheckBox].checkBox.checked),sizeof(bool));// Sync checkbox
            rf.read(reinterpret_cast<char*>(&UIElements[UITDModelSizeRangeBarElement].rangeBar.value),sizeof(float));// 3D Model scale rangebar
            rf.read(reinterpret_cast<char*>(&UIElements[UITDModelPosXRangeBarElement].rangeBar.value),sizeof(float));// 3D Model position X rangebar
            rf.read(reinterpret_cast<char*>(&UIElements[UITDModelPosYRangeBarElement].rangeBar.value),sizeof(float));// 3D Model position Y rangebar
            rf.read(reinterpret_cast<char*>(&UIElements[UITDModelPosZRangeBarElement].rangeBar.value),sizeof(float));// 3D Model position Z rangebar
            rf.read(reinterpret_cast<char*>(&chosenSkyboxIndex),sizeof(int));// Selected skybox index
            rf.read(reinterpret_cast<char*>(&chosenTxtResIndex),sizeof(int));// Selected txtrres index


            std::string modelStr;

            uint64_t modelSize; 
            rf.read(reinterpret_cast<char*>(&modelSize),sizeof(uint64_t));

            for (size_t i = 0; i < modelSize; i++) 
            {
                char c;
                rf.read(reinterpret_cast<char*>(&c),sizeof(char));// 3D Model
                modelStr.push_back(c);
            }
            if(std::filesystem::exists(path + "3DModels" + folderDistinguisher + modelStr))
                model.loadModel(path + "3DModels" + folderDistinguisher + modelStr,true);
            else
                std::cout << "Can't find the 3D Model file at the : " << path + "3DModels" + folderDistinguisher + modelStr << std::endl;
        }
        else{
            std::cout << path + folderDistinguisher + fileName << " is not a project settings file!" << std::endl;
        }
    }
public:
    void initFolder(std::string &path,std::string projectTitle,bool transTextures,bool transNodes,bool transFonts,std::vector<std::string> tdModelPaths,int modelIndex,int &chosenSkyboxIndex,int chosenTxtResIndex,std::vector<UIElement> &UIElements){
        //TODO Process font importing
        #if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
		#else
			char folderDistinguisher = '/'; 
		#endif

        path+=folderDistinguisher;
        path+=projectTitle;
        
        std::filesystem::create_directories(path);
			
        writeSettingsFile(path,tdModelPaths[modelIndex],chosenSkyboxIndex,chosenTxtResIndex,UIElements); //TODO Give index

		//Materials
		std::string materialpath = path;
		materialpath += folderDistinguisher;
		materialpath += "Materials";
		std::filesystem::create_directories(materialpath);
		std::filesystem::copy("./LigidPainter/Resources/Materials/material_0.material", materialpath);
		
        //Models
		std::string modelpath = path;
		modelpath += folderDistinguisher;
		modelpath += "3DModels";
		std::filesystem::create_directories(modelpath);

        for (size_t i = 0; i < tdModelPaths.size(); i++)
        {
            if(std::filesystem::exists(tdModelPaths[i])){
                if(!std::filesystem::is_directory(tdModelPaths[i])){
		            std::filesystem::copy(tdModelPaths[i], modelpath);
                } //Is a file?
                else{
                    std::cout << tdModelPaths[i] << " is a folder and NOT a 3d model file" << std::endl; 
                }
            } //Does exists?
            else{
                std::cout << tdModelPaths[i] << " doesn't exists (File path probably has illegal characters. Renaming them will solve the problem. Afterwards you can include the file from project manager or recreate the project)" << std::endl; 
            }
        }
      
        //Fonts
		std::string fontspath = path;
		fontspath += folderDistinguisher;
		fontspath += "Fonts";
		std::filesystem::create_directories(fontspath);
        if(transFonts){
		    std::filesystem::copy(".\\LigidPainter\\Resources\\fonts\\", fontspath);
        }


		//Shaders
		std::string shaderpath = path;
		shaderpath += folderDistinguisher;
		shaderpath += "Shaders";
		std::filesystem::create_directories(shaderpath);
				
		//Textures
		std::string texturespath = path;
		texturespath += folderDistinguisher;
		texturespath += "Textures";
		std::filesystem::create_directories(texturespath);
				
		std::filesystem::create_directories(texturespath + folderDistinguisher + "Trash");
				
		std::filesystem::create_directories(texturespath + folderDistinguisher + "Export");
			
        std::filesystem::create_directories(texturespath + folderDistinguisher + "Brush Textures");
		
        if(transTextures){
		    std::filesystem::create_directories(texturespath + folderDistinguisher + "Brush Textures" + folderDistinguisher + "Mask");
		    std::filesystem::copy("./LigidPainter/Resources/Textures/Mask", texturespath + folderDistinguisher + "Brush Textures" + folderDistinguisher + "Mask");

		    std::filesystem::create_directories(texturespath + folderDistinguisher + "Brush Textures" + folderDistinguisher + "RGB");
		    std::filesystem::copy("./LigidPainter/Resources/Textures/Sticker", texturespath + folderDistinguisher + "Brush Textures" + folderDistinguisher + "RGB");
    
		    std::filesystem::create_directories(texturespath + folderDistinguisher + "Brush Textures" + folderDistinguisher + "Normal Map");
		    std::filesystem::copy("./LigidPainter/Resources/Textures/NormalMap", texturespath + folderDistinguisher + "Brush Textures" + folderDistinguisher + "Normal Map");
        }
        else{
		    std::filesystem::create_directories(texturespath + folderDistinguisher + "Brush Textures" + folderDistinguisher + "Mask");
		    std::filesystem::copy("./LigidPainter/Resources/Simplified/SimpleBrushTextures/SimpleMask", texturespath + folderDistinguisher + "Brush Textures" + folderDistinguisher + "Mask");

		    std::filesystem::create_directories(texturespath + folderDistinguisher + "Brush Textures" + folderDistinguisher + "RGB");
		    std::filesystem::copy("./LigidPainter/Resources/Simplified/SimpleBrushTextures/SimpleRGB", texturespath + folderDistinguisher + "Brush Textures" + folderDistinguisher + "RGB");
    
		    std::filesystem::create_directories(texturespath + folderDistinguisher + "Brush Textures" + folderDistinguisher + "Normal Map");
		    std::filesystem::copy("./LigidPainter/Resources/Simplified/SimpleBrushTextures/SimpleNormal Map", texturespath + folderDistinguisher + "Brush Textures" + folderDistinguisher + "Normal Map");
        }
			
		//App Nodes
		std::string nodespath = path;
		nodespath += folderDistinguisher;
		nodespath += "Nodes";
		std::filesystem::create_directories(nodespath);

		std::string extension = "";
				
		std::filesystem::copy("./LigidPainter/Resources/Text", path);

        if(transNodes)
		    std::filesystem::copy("./LigidPainter/Resources/Nodes", nodespath);
        else
		    std::filesystem::copy("./LigidPainter/Resources/Simplified/SimpleNodes", nodespath);
        	
		//Version 1.4
		uint64_t h1 = 0xAB428C9F; 
        uint64_t h2 = 0xFF8A1C1C; 
        uint64_t h3 = 0x4B4B9AAA; 

        std::ofstream wf(path + folderDistinguisher + projectTitle + ".ligid", std::ios::out | std::ios::binary);
        		
		if(!wf) {
            std::cout << "ERROR WHILE CREATING LIGID PROJECT FILE! Cannot open file : " << path << std::endl;
        }

        wf.write(reinterpret_cast<char*>(&h1),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(&h2),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(&h3),sizeof(uint64_t));

        time_t ttime2 = time(0);
	    std::string timestr = ctime(&ttime2);
        
        uint64_t timestrsize = timestr.size(); 
        
        wf.write(reinterpret_cast<char*>(&timestrsize),sizeof(uint64_t));
        for (size_t i = 0; i < timestrsize; i++)
        {
            wf.write(reinterpret_cast<char*>(&timestr[i]),sizeof(char));
        }
        
        wf.write(reinterpret_cast<char*>(&timestrsize),sizeof(uint64_t));
        for (size_t i = 0; i < timestrsize; i++)
        {
            wf.write(reinterpret_cast<char*>(&timestr[i]),sizeof(char));
        }
        
    }
    void readFolder(std::string path,std::vector<NodeScene> &materials, std::vector<Node> &appNodes,ContextMenu &addNodeContexMenu,Model &model,std::vector<UIElement> &UIElements,
                    std::vector<aTexture> &albedoTextures,std::vector<Font> &fonts,int& chosenSkyboxIndex,int& chosenTxtResIndex){

        //Version 1.4
		uint64_t h1 = 0xAB428C9F; 
        uint64_t h2 = 0xFF8A1C1C; 
        uint64_t h3 = 0x4B4B9AAA; 
        std::ifstream rf(path, std::ios::out | std::ios::binary);
		if(!rf) {
            std::cout << "ERROR WHILE READING LIGID PROJECT FILE! Cannot open file : " << path << std::endl;
        }


		uint64_t c1; 
        uint64_t c2; 
        uint64_t c3; 
        rf.read(reinterpret_cast<char*>(&c1),sizeof(uint64_t));
        rf.read(reinterpret_cast<char*>(&c2),sizeof(uint64_t));
        rf.read(reinterpret_cast<char*>(&c3),sizeof(uint64_t));

        if(c1 == h1 && c2 == h2 && c3 == h3){
            //Update ligid file


            std::string timestr;
            uint64_t timestrsize; 
            rf.read(reinterpret_cast<char*>(&timestrsize),sizeof(uint64_t));
            for (size_t i = 0; i < timestrsize; i++)
            {
                char c;
                rf.read(reinterpret_cast<char*>(&c),sizeof(char));
                timestr.push_back(c);
            }
            
            
            rf.close();


            std::ofstream ofs;
            ofs.open(path, std::ofstream::out | std::ofstream::trunc);
            ofs.close();

            std::ofstream wf(path, std::ios::out | std::ios::binary);

            wf.write(reinterpret_cast<char*>(&c1),sizeof(uint64_t));
            wf.write(reinterpret_cast<char*>(&c2),sizeof(uint64_t));
            wf.write(reinterpret_cast<char*>(&c3),sizeof(uint64_t));
            
            
            wf.write(reinterpret_cast<char*>(&timestrsize),sizeof(uint64_t));
            for (size_t i = 0; i < timestrsize; i++)
            {
                wf.write(reinterpret_cast<char*>(&timestr[i]),sizeof(char));
            }
            
            
            time_t ttime2 = time(0);
	        std::string timestr2 = ctime(&ttime2);
        
            uint64_t timestrsize2 = timestr.size(); 

            wf.write(reinterpret_cast<char*>(&timestrsize2),sizeof(uint64_t));
            for (size_t i = 0; i < timestrsize2; i++)
            {
                wf.write(reinterpret_cast<char*>(&timestr2[i]),sizeof(char));
            }
            

            time_t crtime = time(0);
	        ctime(&crtime);

            wf.write(reinterpret_cast<char*>(&crtime),sizeof(time_t));

            wf.close();
        }
        else{
            UserInterface ui;
            ui.alert("Error while reading the ligid project file",200,false);
            std::cout << "ERROR WHILE READING LIGID PROJECT FILE! Description header is not recognised at : " << path << " Please try the file on the previous LigidPainter versions" << std::endl;
            return;
        }

        #if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
		#else
			char folderDistinguisher = '/'; 
		#endif
        
        
        while (path[path.size()-1] != '/' && path[path.size()-1] != '\\')
        {
            path.pop_back();
        }
        

        //MATERIALS 
        Utilities util;
        materials.clear();
        
        std::string materialPath = path + folderDistinguisher + "Materials";
        for (const auto & entry : std::filesystem::directory_iterator(materialPath)){
		    std::string fileName = entry.path().string();
		    std::string file = util.getLastWordBySeparatingWithChar(fileName,folderDistinguisher);
		    std::string raw = file;
            NodeScene material;
            
            MaterialFile materialFile;
            material = materialFile.readTheFile(fileName.c_str());

            GlSet gls;
            gls.genTextures(material.renderedTexture);
            glActiveTexture(GL_TEXTURE28);
            gls.bindTexture(material.renderedTexture);
            gls.texImage(nullptr,100,100,GL_RGBA);
			gls.generateMipmap();
            materials.push_back(material);
		}
        if(!materials.size()){
            for (const auto & entry : std::filesystem::directory_iterator("\\LigidPainter\\Resources\\Materials")){
		        std::string fileName = entry.path().string();
		        std::string file = util.getLastWordBySeparatingWithChar(fileName,folderDistinguisher);
		        std::string raw = file;
                NodeScene material;

                MaterialFile materialFile;
                material = materialFile.readTheFile(fileName.c_str());

                GlSet gls;
                gls.genTextures(material.renderedTexture);
                glActiveTexture(GL_TEXTURE28);
                gls.bindTexture(material.renderedTexture);
                gls.texImage(nullptr,100,100,GL_RGBA);
			    gls.generateMipmap();
                materials.push_back(material);
		    }
        }

        //NODES
        appNodes.clear();
        std::string nodePath = path + folderDistinguisher + "Nodes";
        for (const auto & entry : std::filesystem::directory_iterator(nodePath)){
			std::string fileName = entry.path().string();
			std::string file = util.getLastWordBySeparatingWithChar(fileName,folderDistinguisher);
			std::string raw = file;

			for (size_t i = 0; i < 5; i++)
			{
				raw.pop_back();
			}
			Load load;
			appNodes.push_back(load.createNode(fileName,raw));				
		}
        UserInterface ui;
        addNodeContexMenu = ui.createContextMenus(appNodes);

        //Settings

        std::string setting;
        ifstream stRf(path + folderDistinguisher + "ProjectSettings.settings");

        if(stRf.fail()){
            std::cout << "WARNING! ProjectSettings.settings file can't be detected at : " << path << std::endl;
        }
        else{
            readSettingsFile(path,chosenSkyboxIndex,chosenTxtResIndex,stRf,setting,UIElements,model);
        }

       

        //FONTS
        fonts.clear();
        std::string fontsPath = path + "Fonts";
        for (const auto & entry : std::filesystem::directory_iterator(fontsPath)){
            std::string fileName = entry.path().string();
		    std::string file = util.getLastWordBySeparatingWithChar(fileName,folderDistinguisher);
            
            Load load;
            fonts.push_back(load.createFont(fileName));
        }
        
        

        //TEXTURES
        albedoTextures.clear();
        std::string texturesPath = path + "Textures";
        std::vector<std::string> folders;
        folders.push_back(texturesPath);
        Texture texture;
        
        aTexture trashFolder;
	    trashFolder.name = "Trash";
	    trashFolder.isTexture = false;
	    trashFolder.isTrashFolder = true;
	    albedoTextures.push_back(trashFolder);
    
	    aTexture exportFolder;
	    exportFolder.isTexture = false;
	    exportFolder.name = "Export";
	    albedoTextures.push_back(exportFolder);
	    aTexture brushFolder;
	    brushFolder.isTexture = false;
	    brushFolder.name = "Brush Textures";
	    albedoTextures.push_back(brushFolder);

        for (size_t i = 0; i < folders.size(); i++)
        {
            for (const auto & entry : std::filesystem::directory_iterator(folders[i])){
		        std::string fileName = entry.path().string();
		        std::string file = util.getLastWordBySeparatingWithChar(fileName,folderDistinguisher);
		        std::string raw = file;

                if(std::filesystem::is_directory(fileName)){
                    folders.push_back(fileName);
                    if(file != "Trash" && file != "Export" && file != "Brush Textures"){
                        aTexture folder;
                        if(i != 0){
                            if(folders[i] == path+"Textures"+folderDistinguisher+"Trash")
                                folder.folderIndex = 0;
                            else if(folders[i] == path+"Textures"+folderDistinguisher+"Export")
                                folder.folderIndex = 1;
                            else if(folders[i] == path+"Textures"+folderDistinguisher+"Brush Textures")
                                folder.folderIndex = 2;
                            else
                                folder.folderIndex = i-1;                            
                        }
                        folder.name = file;
                        folder.isTexture = false;
                        albedoTextures.push_back(folder);
                    }
                }
                else{

                    aTexture txtr;
                    glActiveTexture(GL_TEXTURE28);

                    unsigned int txtrID;
                    GlSet glset;
                    glset.genTextures(txtrID);
                    txtrID = texture.getTextureForcePNG(fileName,file,false,txtr.width,txtr.height);

                    

                    txtr.id = txtrID;
                    txtr.name = file;
                    util.extensionCheckForTexture(txtr.name);
                    txtr.changed = false;
                    if(i != 0){
                        if(folders[i] == "Trash")
                            txtr.folderIndex = 0;
                        else if(folders[i] == "Export")
                            txtr.folderIndex = 1;
                        else if(folders[i] == "Brush Textures")
                            txtr.folderIndex = 2;
                        else
                            txtr.folderIndex = i-1;
                    }
                    albedoTextures.push_back(txtr);
                }

		    }
        }

    }
    std::string getTheProjectPathOfTheTexture(std::vector<aTexture> albedoTextures,aTexture txtr,std::string projectPath){
        #if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
		#else
			char folderDistinguisher = '/'; 
		#endif

        std::string path;
        path = projectPath + folderDistinguisher + "Textures";


        if(txtr.folderIndex == 10000)
            path += folderDistinguisher + txtr.name ; //0 Folder 
        else{
            if(albedoTextures[txtr.folderIndex].folderIndex == 10000)
                path += folderDistinguisher + albedoTextures[txtr.folderIndex].name + folderDistinguisher + txtr.name ; //1 Folder
            else{
                if(albedoTextures[albedoTextures[txtr.folderIndex].folderIndex].folderIndex == 10000)
                    path += folderDistinguisher + albedoTextures[albedoTextures[txtr.folderIndex].folderIndex].name + folderDistinguisher + albedoTextures[txtr.folderIndex].name + folderDistinguisher + txtr.name ; //2 Folder
                else{
                    if(albedoTextures[albedoTextures[albedoTextures[txtr.folderIndex].folderIndex].folderIndex].folderIndex == 10000)
                        path += folderDistinguisher + albedoTextures[albedoTextures[albedoTextures[txtr.folderIndex].folderIndex].folderIndex].name + folderDistinguisher + albedoTextures[albedoTextures[txtr.folderIndex].folderIndex].name + folderDistinguisher + albedoTextures[txtr.folderIndex].name + folderDistinguisher + txtr.name ; //3 Folder
                    else{
                        if(albedoTextures[albedoTextures[albedoTextures[albedoTextures[txtr.folderIndex].folderIndex].folderIndex].folderIndex].folderIndex == 10000)
                            path += folderDistinguisher + albedoTextures[albedoTextures[albedoTextures[albedoTextures[txtr.folderIndex].folderIndex].folderIndex].folderIndex].name + folderDistinguisher + albedoTextures[albedoTextures[albedoTextures[txtr.folderIndex].folderIndex].folderIndex].name + folderDistinguisher + albedoTextures[albedoTextures[txtr.folderIndex].folderIndex].name + folderDistinguisher + albedoTextures[txtr.folderIndex].name + folderDistinguisher + txtr.name ; //4 Folder
                        else{
                            if(albedoTextures[ albedoTextures[albedoTextures[albedoTextures[albedoTextures[txtr.folderIndex].folderIndex].folderIndex].folderIndex].folderIndex].folderIndex == 10000)
                                path += folderDistinguisher + albedoTextures[ albedoTextures[albedoTextures[albedoTextures[albedoTextures[txtr.folderIndex].folderIndex].folderIndex].folderIndex].folderIndex].name + folderDistinguisher + albedoTextures[albedoTextures[albedoTextures[albedoTextures[txtr.folderIndex].folderIndex].folderIndex].folderIndex].name + folderDistinguisher + albedoTextures[albedoTextures[albedoTextures[txtr.folderIndex].folderIndex].folderIndex].name + folderDistinguisher + albedoTextures[albedoTextures[txtr.folderIndex].folderIndex].name + folderDistinguisher + albedoTextures[txtr.folderIndex].name + folderDistinguisher + txtr.name ; //5 Folder
                            else{
                                if(albedoTextures[albedoTextures[ albedoTextures[albedoTextures[albedoTextures[albedoTextures[txtr.folderIndex].folderIndex].folderIndex].folderIndex].folderIndex].folderIndex].folderIndex == 10000)
                                    path += folderDistinguisher + albedoTextures[albedoTextures[ albedoTextures[albedoTextures[albedoTextures[albedoTextures[txtr.folderIndex].folderIndex].folderIndex].folderIndex].folderIndex].folderIndex].name + folderDistinguisher + albedoTextures[ albedoTextures[albedoTextures[albedoTextures[albedoTextures[txtr.folderIndex].folderIndex].folderIndex].folderIndex].folderIndex].name + folderDistinguisher + albedoTextures[albedoTextures[albedoTextures[albedoTextures[txtr.folderIndex].folderIndex].folderIndex].folderIndex].name + folderDistinguisher + albedoTextures[albedoTextures[albedoTextures[txtr.folderIndex].folderIndex].folderIndex].name + folderDistinguisher + albedoTextures[albedoTextures[txtr.folderIndex].folderIndex].name +  folderDistinguisher + albedoTextures[txtr.folderIndex].name + folderDistinguisher + txtr.name ; //6 Folder
                            }
                        }
                    }
                }
            }
        }
        return path;
    }
    void saveFolder(std::string projectPath,std::vector<aTexture> &albedoTextures,int txtrRes,int& chosenSkyboxIndex,int& chosenTxtResIndex,std::vector<UIElement> &UIElements,Model &model){

        #if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
		#else
			char folderDistinguisher = '/'; 
		#endif
        Utilities util;
        writeSettingsFile(projectPath,util.getLastWordBySeparatingWithChar(model.filePath,folderDistinguisher),chosenSkyboxIndex,chosenTxtResIndex,UIElements); //TODO Give the current td model name

        std::vector<aTexture> changedTextures;
        for (size_t i = 0; i < albedoTextures.size(); i++)
        {
            if(albedoTextures[i].changed && albedoTextures[i].isTexture){
                changedTextures.push_back(albedoTextures[i]);
                albedoTextures[i].changed = false;
            }
        }

        for (size_t i = 0; i < changedTextures.size(); i++)
        {
            std::string path;
            path = getTheProjectPathOfTheTexture(albedoTextures,changedTextures[i],projectPath);

            //Save the texture
            Texture txtr;
            //TODO 1024
            glActiveTexture(GL_TEXTURE28);
            glBindTexture(GL_TEXTURE_2D,changedTextures[i].id);
            GLubyte* pixelData = txtr.getTextureFromProgram(GL_TEXTURE28,1024,1024,4);
            stbi_write_png(path.c_str(), changedTextures[i].width, changedTextures[i].height, 4, pixelData, changedTextures[i].width * 4);
        }
        std::string path;
        if(projectPath[projectPath.size()-1] == '/' || projectPath[projectPath.size()-1] == '\\')
            path = projectPath + "Textures" + folderDistinguisher;
        else
            path = projectPath + folderDistinguisher + "Textures" + folderDistinguisher;
        
        //std::cout <<  "PATH WAS : " << path << std::endl;
        
        for (const auto & entry : std::filesystem::recursive_directory_iterator(path)){
            std::string filePath = entry.path().string();
            bool matched = false;
            for (size_t i = 0; i < albedoTextures.size(); i++)
            {
                std::string txtrpath;
                txtrpath = getTheProjectPathOfTheTexture(albedoTextures,albedoTextures[i],projectPath);


                if(txtrpath == filePath)
                    matched = true;
                std::cout << filePath <<' ' << txtrpath << std::endl;
            }
            if(!matched){
                if(std::filesystem::is_directory(filePath))
                    std::filesystem::remove_all(filePath);
                else
                    std::filesystem::remove(filePath);
            }
        }
    }
    string rmvPath(string &startingPath, string &fullPath) 
    {
        // look for starting path in the fullpath
        int index = fullPath.find(startingPath);

        if (index != string::npos)
        {
            fullPath = fullPath.erase(0, startingPath.length());
        }
        return fullPath;
    }
    string removeLastPath(std::string path){
        #if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
		#else
			char folderDistinguisher = '/'; 
		#endif
        int c = path.size()-1;
        while(path[c] != folderDistinguisher){
            path.erase(path.begin()+c);
            c--;
        }
        return path;
    }
    std::string duplicateFolder(std::string srcPath){
        #if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
		#else
			char folderDistinguisher = '/'; 
		#endif
        
        char* dest = tinyfd_selectFolderDialog("Select the destination folder","");
        if(dest){
            Utilities util;
            std::string destinationPath = (std::string)(dest) + folderDistinguisher +  util.getLastWordBySeparatingWithChar(srcPath,folderDistinguisher);
            if(destinationPath.size()){
                if(std::filesystem::is_directory(destinationPath))
                    std::filesystem::remove_all(destinationPath);

                std::filesystem::create_directories(destinationPath);
                for (auto& p : std::filesystem::recursive_directory_iterator(srcPath)){
                    std::string filePath = p.path().string();

                    if(std::filesystem::is_directory(filePath)){
                        std::filesystem::create_directories(destinationPath + folderDistinguisher + rmvPath(srcPath,filePath));
                    }
                    else{
                        std::filesystem::copy(srcPath + filePath,removeLastPath(destinationPath + folderDistinguisher + rmvPath(srcPath,filePath)));
                    }
                }
            }

        }
        return dest;
    }
private:
};

#endif