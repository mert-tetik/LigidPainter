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


class ProjectFolder{
public:
    void initFolder(std::string &path,char* &modelFilePath,std::vector<UIElement> &UIElements,bool transTextures,bool transNodes){
        #if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
		#else
			char folderDistinguisher = '/'; 
		#endif
        
        std::filesystem::create_directories(path);
				
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
		std::filesystem::copy(".\\LigidPainter\\Resources\\3D Models\\", modelpath);
      
        //Fonts
		std::string fontspath = path;
		fontspath += folderDistinguisher;
		fontspath += "Fonts";
		std::filesystem::create_directories(fontspath);
		std::filesystem::copy(".\\LigidPainter\\Resources\\fonts\\", fontspath);


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

		std::string modelPath = modelFilePath;

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

        std::ofstream wf(path + folderDistinguisher + UIElements[UIgenerateTextTextureTextTextBoxElement].textBox.text + ".ligid", std::ios::out | std::ios::binary);
        		
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
    void readFolder(std::string path,std::vector<NodeScene> &materials, std::vector<Node> &appNodes,ContextMenu &addNodeContexMenu,Model &model,std::vector<UIElement> &UIElements,std::vector<aTexture> &albedoTextures,std::vector<Font> &fonts){

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
        
        std::cout << "path is : " << path << std::endl;


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

            materials.push_back(material);
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
        std::vector<std::string> elements{
            "Model", //0
            "Skybox", //1
            "Texture_Resolution", //2
            "Backface_Culling", //3
            "Node_Resolution", //4
            "Image_Background", //5
            "Skybox_Background", //6
            "Realtime_Material_Rendering", //7
            "Sync", //8
            "JPG", //9
            "Selected_Material" //10 
        };

        std::string setting;
        ifstream stRf(path + folderDistinguisher + "ProjectSettings.settings");

        if(stRf.fail()){
            std::cout << "WARNING! ProjectSettings.settings file can't be detected at : " << path << std::endl;
        }

        while (getline(stRf, setting)) {
            std::string stName;
            std::string stValue;
            int stI = 0;
            while (setting[stI] != ' ')
            {
                stName+=setting[stI];
                stI++;
            }
            stI++;
            
            while (stI != setting.size())
            {
                stValue+=setting[stI];
                stI++;
            }
            
            int stVal;
            if(stName != elements[0])
                stVal = std::stoi(stValue); 
            if(stName == elements[0])
                model.loadModel(path + "3DModels" + folderDistinguisher + stValue,true);

            //if(stName == elements[2])
            if(stName == elements[3])
                UIElements[UIbackfaceCullingCheckBox].checkBox.checked = stVal;
            //if(stName == elements[4])
            
            if(stName == elements[5])
                UIElements[UIimageCheckBoxElement].checkBox.checked = stVal;
            if(stName == elements[6])
                UIElements[UIskyboxCheckBox].checkBox.checked = stVal;
            if(stName == elements[7])
                UIElements[UIrealtimeMaterialRenderingCheckBox].checkBox.checked = stVal;
            if(stName == elements[8])
                UIElements[UIstabilizeFpsCheckBox].checkBox.checked = stVal;
            if(stName == elements[9]){
                UIElements[UIjpgCheckBox].checkBox.checked = stVal;
                UIElements[UIpngCheckBox].checkBox.checked = !UIElements[UIjpgCheckBox].checkBox.checked;
            }
            //if(stName == elements[10])
                //UIElements[UIjpgCheckBox].checkBox.checked;

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
                    txtrID = texture.getTexture(fileName,0,0,false);
                    txtr.id = txtrID;
                    txtr.name = file;
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

    void saveFolder(std::string projectPath,std::vector<aTexture> &albedoTextures,int txtrRes){

        #if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
		#else
			char folderDistinguisher = '/'; 
		#endif

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
            path = projectPath + folderDistinguisher + "Textures";

            //Get the path
            if(changedTextures[i].folderIndex == 10000)
                path += folderDistinguisher + changedTextures[i].name + ".png"; //0 Folder 
            else{
                if(albedoTextures[changedTextures[i].folderIndex].folderIndex == 10000)
                    path += folderDistinguisher + albedoTextures[changedTextures[i].folderIndex].name + folderDistinguisher + changedTextures[i].name + ".png"; //1 Folder
                else{
                    if(albedoTextures[albedoTextures[changedTextures[i].folderIndex].folderIndex].folderIndex == 10000)
                        path += folderDistinguisher + albedoTextures[albedoTextures[changedTextures[i].folderIndex].folderIndex].name + albedoTextures[changedTextures[i].folderIndex].name + folderDistinguisher + changedTextures[i].name + ".png"; //2 Folder
                    else{
                        if(albedoTextures[albedoTextures[albedoTextures[changedTextures[i].folderIndex].folderIndex].folderIndex].folderIndex == 10000)
                            path += folderDistinguisher + albedoTextures[albedoTextures[albedoTextures[changedTextures[i].folderIndex].folderIndex].folderIndex].name + folderDistinguisher + albedoTextures[albedoTextures[changedTextures[i].folderIndex].folderIndex].name + albedoTextures[changedTextures[i].folderIndex].name + folderDistinguisher + changedTextures[i].name + ".png"; //3 Folder
                        else{
                            if(albedoTextures[albedoTextures[albedoTextures[albedoTextures[changedTextures[i].folderIndex].folderIndex].folderIndex].folderIndex].folderIndex == 10000)
                                path += folderDistinguisher + albedoTextures[albedoTextures[albedoTextures[albedoTextures[changedTextures[i].folderIndex].folderIndex].folderIndex].folderIndex].name + folderDistinguisher + albedoTextures[albedoTextures[albedoTextures[changedTextures[i].folderIndex].folderIndex].folderIndex].name + folderDistinguisher + albedoTextures[albedoTextures[changedTextures[i].folderIndex].folderIndex].name + albedoTextures[changedTextures[i].folderIndex].name + folderDistinguisher + changedTextures[i].name + ".png"; //4 Folder
                            else{
                                if(albedoTextures[ albedoTextures[albedoTextures[albedoTextures[albedoTextures[changedTextures[i].folderIndex].folderIndex].folderIndex].folderIndex].folderIndex].folderIndex == 10000)
                                    path += folderDistinguisher + albedoTextures[ albedoTextures[albedoTextures[albedoTextures[albedoTextures[changedTextures[i].folderIndex].folderIndex].folderIndex].folderIndex].folderIndex].name + folderDistinguisher + albedoTextures[albedoTextures[albedoTextures[albedoTextures[changedTextures[i].folderIndex].folderIndex].folderIndex].folderIndex].name + folderDistinguisher + albedoTextures[albedoTextures[albedoTextures[changedTextures[i].folderIndex].folderIndex].folderIndex].name + folderDistinguisher + albedoTextures[albedoTextures[changedTextures[i].folderIndex].folderIndex].name + albedoTextures[changedTextures[i].folderIndex].name + folderDistinguisher + changedTextures[i].name + ".png"; //5 Folder
                                else{
                                    if(albedoTextures[albedoTextures[ albedoTextures[albedoTextures[albedoTextures[albedoTextures[changedTextures[i].folderIndex].folderIndex].folderIndex].folderIndex].folderIndex].folderIndex].folderIndex == 10000)
                                        path += folderDistinguisher + albedoTextures[albedoTextures[ albedoTextures[albedoTextures[albedoTextures[albedoTextures[changedTextures[i].folderIndex].folderIndex].folderIndex].folderIndex].folderIndex].folderIndex].name + folderDistinguisher + albedoTextures[ albedoTextures[albedoTextures[albedoTextures[albedoTextures[changedTextures[i].folderIndex].folderIndex].folderIndex].folderIndex].folderIndex].name + folderDistinguisher + albedoTextures[albedoTextures[albedoTextures[albedoTextures[changedTextures[i].folderIndex].folderIndex].folderIndex].folderIndex].name + folderDistinguisher + albedoTextures[albedoTextures[albedoTextures[changedTextures[i].folderIndex].folderIndex].folderIndex].name + folderDistinguisher + albedoTextures[albedoTextures[changedTextures[i].folderIndex].folderIndex].name + albedoTextures[changedTextures[i].folderIndex].name + folderDistinguisher + changedTextures[i].name + ".png"; //6 Folder
                                }
                            }
                        }
                    }
                }
            }

            //Save the texture
            Texture txtr;
            //TODO 1024
            glActiveTexture(GL_TEXTURE28);
            glBindTexture(GL_TEXTURE_2D,changedTextures[i].id);
            GLubyte* pixelData = txtr.getTextureFromProgram(GL_TEXTURE28,1024,1024,4);
            stbi_write_png(path.c_str(), txtrRes, txtrRes, 4, pixelData, txtrRes * 4);
        }
    }
    void duplicateFolder(std::string srcPath){
        char* destinationPath = tinyfd_selectFolderDialog("Select the destination folder","");
        if(destinationPath)
            std::filesystem::copy(srcPath,destinationPath);
    }
private:
};

#endif