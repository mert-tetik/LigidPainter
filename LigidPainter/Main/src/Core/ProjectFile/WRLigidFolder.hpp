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
    void initFolder(std::string &path,NodeScene defaultMaterial,char* &modelFilePath,std::vector<UIElement> &UIElements){
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
		materialpath += folderDistinguisher;
		materialpath += defaultMaterial.sceneName;
				
        MaterialFile materialFile;
		materialFile.writeTheFile(materialpath.c_str(),defaultMaterial);

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
				
		std::filesystem::create_directories(texturespath + folderDistinguisher + "Brush Textures" + folderDistinguisher + "Mask");
		std::filesystem::copy("./LigidPainter/Resources/Textures/Mask", texturespath + folderDistinguisher + "Brush Textures" + folderDistinguisher + "Mask");

		std::filesystem::create_directories(texturespath + folderDistinguisher + "Brush Textures" + folderDistinguisher + "RGB");
		std::filesystem::copy("./LigidPainter/Resources/Textures/Sticker", texturespath + folderDistinguisher + "Brush Textures" + folderDistinguisher + "RGB");
				
		std::filesystem::create_directories(texturespath + folderDistinguisher + "Brush Textures" + folderDistinguisher + "Normal Map");
		std::filesystem::copy("./LigidPainter/Resources/Textures/NormalMap", texturespath + folderDistinguisher + "Brush Textures" + folderDistinguisher + "Normal Map");
			
		//App Nodes
		std::string nodespath = path;
		nodespath += folderDistinguisher;
		nodespath += "Nodes";
		std::filesystem::create_directories(nodespath);

		std::string modelPath = modelFilePath;

		std::string extension = "";
				
		std::filesystem::copy(modelFilePath, path + folderDistinguisher + "3DModel." + extension);

		std::filesystem::copy("./LigidPainter/Resources/Text", path);

		std::filesystem::copy("./LigidPainter/Resources/Nodes", nodespath);
				
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
    }
    void readFolder(std::string path,std::vector<NodeScene> &materials, std::vector<Node> &appNodes,ContextMenu &addNodeContexMenu,Model &model,std::vector<UIElement> &UIElements,std::vector<aTexture> &albedoTextures){

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
            //Passed
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
			appNodes.push_back(load.createNode(raw));				
		}
        UserInterface ui;
        addNodeContexMenu = ui.createContextMenus(appNodes);

        model.loadModel(path + folderDistinguisher + "3DModel",true);


        //Settings
        std::vector<std::string> elements{
            "Skybox", //0
            "Texture_Resolution", //1
            "Backface_Culling", //2
            "Node_Resolution", //3
            "Image_Background", //4
            "Skybox_Background", //5
            "Realtime_Material_Rendering", //6
            "Sync", //7
            "JPG", //8
            "Selected_Material" //9 
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
            
            int stVal = std::stoi(stValue); 

            //if(stName == elements[0])
            //if(stName == elements[1])
            if(stName == elements[2])
                UIElements[UIbackfaceCullingCheckBox].checkBox.checked = stVal;
            //if(stName == elements[3])
            
            if(stName == elements[4])
                UIElements[UIimageCheckBoxElement].checkBox.checked = stVal;
            if(stName == elements[5])
                UIElements[UIskyboxCheckBox].checkBox.checked = stVal;
            if(stName == elements[6])
                UIElements[UIrealtimeMaterialRenderingCheckBox].checkBox.checked = stVal;
            if(stName == elements[7])
                UIElements[UIstabilizeFpsCheckBox].checkBox.checked = stVal;
            if(stName == elements[8]){
                UIElements[UIjpgCheckBox].checkBox.checked = stVal;
                UIElements[UIpngCheckBox].checkBox.checked = !UIElements[UIjpgCheckBox].checkBox.checked;
            }
            //if(stName == elements[9])
                //UIElements[UIjpgCheckBox].checkBox.checked;

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

private:
};

#endif