#include <iostream>
#include <ostream>
#include <fstream>
#include <sstream>

#include "../../thirdparty/include/glad/glad.h"

#include "Core/Model/model.h"
#include "Core/UI/UserInterface.h"
#include "Core/Texture/Texture.h"
#include "Core/gl.h"

//LIGID PAINTER PROJECT FILE GENERATOR
//This single header generates *.ligid file witch contains loaded 3d model, textures & materials

//Documentation

//Every LigidPainter project file shall start with 0x94DA0DB9 0xF53B4451 0xD0E355DE   

//3D Model code 0xB2911B6A 0x6825EA68 0x59EF9DF5

//Vertices code 0x1DCEC243 0xD25EF96B 0xF6A94C0C + size of the vertices 
//Example : 0x1DCEC243 0xD25EF96B 0xF6A94C0C 0x003D0900 (same goes for the indices)

//Indices code 0x3600353C 0xB3ACAD02 0x6C23DE2C + size of the indices

//Texture code 0xA7F8F642 0x99CAD11E 0x6F5E55A8 + size of the texture elements + texture resolution
//Example : 0xA7F8F642 0x99CAD11E 0x6F5E55A8 0x0000000a 0x00000400

//Texture element code 0xB7A58C82 0xF7F7B273 0x3C82A6FF + size of the elements

//Node Scene (Material code) 0x0A78DA67 0xF99C3CA7 0x36CB89A5 + size of the node scenes



//TODO : Multiple meshes
//TODO : Get nodes & materials

class LigidFile{
public:
    Model writeTheFile(const char* path,Model model,std::vector<aTexture> textures,std::vector<NodeScene> nodeScenes){

        std::ofstream wf = createTheProjectFile(path);

        writeModelData(wf,model);
        writeTextures(wf,textures);
        drawTextureElements(wf,textures);
        writeNodeScenes(wf,nodeScenes);

        wf.close();

        return model;
    }
    void readTheFile(const char* path,Model &model,std::vector<aTexture> &textures){
        ifstream rf;
        std::cout << "READING";

        if(startReadingTheProjectFile(path,rf)){
            //3D Model
            textures.clear();
            model.meshes.clear();
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            
            uint64_t c;
            int modelKeysCounter = 0;
            int verticeKeysCounter = 0;
            int indiceKeysCounter = 0;
            int textureKeysCounter = 0;
            int textureElementKeysCounter = 0;

            int counter = 0;
            while(rf.read(reinterpret_cast<char*>(&c),sizeof(uint64_t))){
                
                updateCounters(c,modelKeysCounter,verticeKeysCounter,indiceKeysCounter,textureKeysCounter,textureElementKeysCounter);
                
                //3D Model (Is not necessary)
                if(modelKeysCounter == 3){
                    modelKeysCounter = 0;
                }

                //Vertices
                if(verticeKeysCounter == 3){
                    verticeKeysCounter = 0;
                    readVertices(rf,vertices);
                }

                //Indices
                if(indiceKeysCounter == 3){
                    indiceKeysCounter = 0;
                    readIndices(rf,indices);
                }

                //Textures
                if(textureKeysCounter == 3){
                    textureKeysCounter = 0;
                    readTexture(rf,textures);
                }
                
                //Textures
                if(textureElementKeysCounter == 3){
                    textureElementKeysCounter = 0;
                    readTextureElements(rf,textures);
                }

                counter++;
            }
            model.meshes.push_back(Mesh(vertices, indices, {},"",0));
            finishTheModel(model);
        }
        rf.close();
    }
private:
    //--------------------READ THE FILE--------------------
    struct sTexture{
	    unsigned int id;

	    bool nameTextActive = false;

	    bool isTexture = true;
    
	    bool isTrashFolder = false;

	    bool rightClicked = false;

	    int folderIndex = 10000;
    };

    bool startReadingTheProjectFile(const char* path,std::ifstream &rf){
        rf = std::ifstream(path, std::ios::out | std::ios::binary);
        if(!rf.is_open()){
            std::cout << "ERROR WHILE READING THE LIGID PROJECT FILE! Cannot open file : " << path << std::endl;
            return false;
        }
        uint64_t keys[3] = {0x94DA0DB9 ,0xF53B4451 ,0xD0E355DE};
        for (size_t i = 0; i < 3; i++)
        {
            uint64_t c;
            rf.read(reinterpret_cast<char*>(&c),sizeof(uint64_t));
            if(c != keys[i]){
                std::cout << "ERROR WHILE READING THE LIGID PROJECT FILE! This file is not associated with LigidPainter : " << path << std::endl;
                return false;
            }
        }
        return true;
    }
    
    void updateCounters(uint64_t c,int &modelKeysCounter,int &verticeKeysCounter,int &indiceKeysCounter,int &textureKeysCounter, int &textureElementKeysCounter){
        uint64_t modelKeys[3] =  {0xB2911B6A, 0x6825EA68, 0x59EF9DF5};
        uint64_t verticeKeys[3] =  {0x1DCEC243, 0xD25EF96B, 0xF6A94C0C};
        uint64_t indiceKeys[3] =  {0x3600353C, 0xB3ACAD02, 0x6C23DE2C};
        uint64_t textureKeys[3] =  {0xA7F8F642, 0x99CAD11E, 0x6F5E55A8};
        uint64_t textureElementKeys[3] =  {0xB7A58C82 ,0xF7F7B273 ,0x3C82A6FF};
        uint64_t nodeSceneKeys[3] =  {0x0A78DA67, 0xF99C3CA7, 0x36CB89A5};

        if(c == modelKeys[modelKeysCounter])
            modelKeysCounter++;
        else{
            modelKeysCounter = 0;
        }
        if(c == verticeKeys[verticeKeysCounter])
            verticeKeysCounter++;
        else{
            verticeKeysCounter = 0;
        }
        if(c == indiceKeys[indiceKeysCounter])
            indiceKeysCounter++;
        else{
            indiceKeysCounter = 0;
        }
        if(c == textureKeys[textureKeysCounter])
            textureKeysCounter++;
        else{
            textureKeysCounter = 0;
        }
        if(c == textureElementKeys[textureElementKeysCounter])
            textureElementKeysCounter++;
        else{
            textureElementKeysCounter = 0;
        }
    }

    void readVertices(std::ifstream &rf,std::vector<Vertex> &vertices){
        uint64_t verticeCount = 0;
        rf.read(reinterpret_cast<char*>(&verticeCount),sizeof(uint64_t));
        
        for (size_t i = 0; i < verticeCount; i++)
        {
            Vertex vertex;
            rf.read(reinterpret_cast<char*>(&vertex),sizeof(Vertex));
            vertices.push_back(vertex);
        }
    }
    void readIndices(std::ifstream &rf,std::vector<unsigned int> &indices){
        uint64_t indiceCount = 0;
        rf.read(reinterpret_cast<char*>(&indiceCount),sizeof(uint64_t));
        
        
        for (size_t i = 0; i < indiceCount; i++)
        {
            unsigned int index;
            rf.read(reinterpret_cast<char*>(&index),sizeof(unsigned int));
            indices.push_back(index);
        }
    }
    void readTexture(std::ifstream &rf,std::vector<aTexture>&textures){
        uint64_t textureCount = 0;
        rf.read(reinterpret_cast<char*>(&textureCount),sizeof(uint64_t));
        uint64_t resolution = 0;
        rf.read(reinterpret_cast<char*>(&resolution),sizeof(uint64_t));//1024
        
        uint64_t pixelCount = resolution*resolution;

        for (size_t txtrI = 0; txtrI < textureCount; txtrI++)
        {
            GLubyte* textureData = new GLubyte[pixelCount*4];
            aTexture txtr;
            for (size_t pxI = 0; pxI < pixelCount*4; pxI++)
            {
                GLubyte data;
                rf.read(reinterpret_cast<char*>(&data),sizeof(GLubyte));
                textureData[pxI] = data;
            }
            glActiveTexture(GL_TEXTURE28);
            unsigned int textureId;
            glGenTextures(1,&textureId);
            glBindTexture(GL_TEXTURE_2D,textureId);
            GlSet gls;
            gls.texImage(textureData,resolution,resolution,GL_RGBA);
            gls.generateMipmap();
            delete[] textureData;

            txtr.id = textureId;
            txtr.folderIndex = 10000;
            txtr.isTexture = true;
            txtr.name = "importedTexture";
            textures.push_back(txtr);
        }        
    }
    void readTextureElements(std::ifstream &rf,std::vector<aTexture>&textures){
        std::vector<aTexture> generatedTextureArray;
        
        uint64_t textureElementCount = 0;
        rf.read(reinterpret_cast<char*>(&textureElementCount),sizeof(uint64_t));

        int orgTxtrI = 0;
        for (size_t i = 0; i < textureElementCount; i++)
        {
            sTexture element;
            rf.read(reinterpret_cast<char*>(&element),sizeof(sTexture));

            if(element.isTexture){
                element.id = textures[orgTxtrI].id; 
                orgTxtrI++;
            }
            aTexture atxtr;
            atxtr.id = element.id;
            atxtr.folderIndex = element.folderIndex;
            atxtr.isTexture = element.isTexture;
            atxtr.isTrashFolder = element.isTrashFolder;
            atxtr.nameTextActive = false;

            uint64_t txtrNameSize = 0;
            rf.read(reinterpret_cast<char*>(&txtrNameSize),sizeof(uint64_t));
            for (size_t cI = 0; cI < txtrNameSize; cI++)
            {
                char txtrNameC;
                rf.read(reinterpret_cast<char*>(&txtrNameC),sizeof(char));
                atxtr.name.push_back(txtrNameC);
            }

            generatedTextureArray.push_back(atxtr);
        }
        textures = generatedTextureArray;
    }


    void finishTheModel(Model &model){
        model.meshes[0].materialIndex = 0;
        model.meshes[0].materialName = "loadedMaterial";

        model.uploadModel(model);
    }



    //--------------------CREATE THE FILE--------------------

    std::ofstream createTheProjectFile(const char* path){
    //Create the ofstream & write the ligidpainter header code
        uint64_t h1 = 0x94DA0DB9; 
        uint64_t h2 = 0xF53B4451; 
        uint64_t h3 = 0xD0E355DE; 

        std::ofstream wf(path, std::ios::out | std::ios::binary);
        if(!wf) {
            std::cout << "ERROR WHILE CREATING LIGID PROJECT FILE! Cannot open file : " << path << std::endl;
        }

        wf.write(reinterpret_cast<char*>(&h1),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(&h2),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(&h3),sizeof(uint64_t));

        return wf;
    }
    void writeModelData(std::ofstream &wf,Model model){
        //Model
        uint64_t model1 = 0xB2911B6A;  
        uint64_t model2 = 0x6825EA68;  
        uint64_t model3 = 0x59EF9DF5; 

        uint64_t vert1 = 0x1DCEC243;    
        uint64_t vert2 = 0xD25EF96B;   
        uint64_t vert3 = 0xF6A94C0C;  
        
        uint64_t indice1 = 0x3600353C; 
        uint64_t indice2 = 0xB3ACAD02; 
        uint64_t indice3 = 0x6C23DE2C;
        
        wf.write(reinterpret_cast<char*>(&model1),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(&model2),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(&model3),sizeof(uint64_t));

        
        for (size_t meshI = 0; meshI < model.meshes.size(); meshI++)
        {
            //--------Vertices
            wf.write(reinterpret_cast<char*>(&vert1),sizeof(uint64_t));
            wf.write(reinterpret_cast<char*>(&vert2),sizeof(uint64_t));
            wf.write(reinterpret_cast<char*>(&vert3),sizeof(uint64_t));
            //Write the size
            uint64_t vertSize = model.meshes[meshI].vertices.size();
            wf.write(reinterpret_cast<char*>(&vertSize),sizeof(uint64_t));

            for (size_t verticeI = 0; verticeI < model.meshes[meshI].vertices.size(); verticeI++)
            {
                wf.write(reinterpret_cast<char*>(&model.meshes[meshI].vertices[verticeI]),sizeof(Vertex));
            }
            
            //--------Indices
             wf.write(reinterpret_cast<char*>(&indice1),sizeof(uint64_t));
             wf.write(reinterpret_cast<char*>(&indice2),sizeof(uint64_t));
             wf.write(reinterpret_cast<char*>(&indice3),sizeof(uint64_t));
            //Write the size
            uint64_t indiceSize = model.meshes[meshI].indices.size();
            wf.write(reinterpret_cast<char*>(&indiceSize),sizeof(uint64_t));
            
            for (size_t indiceI = 0; indiceI < model.meshes[meshI].indices.size(); indiceI++)
            {
                wf.write(reinterpret_cast<char*>(&model.meshes[meshI].indices[indiceI]),sizeof(unsigned int));
            }
        }
    }

    void writeTextures(std::ofstream &wf,std::vector<aTexture> textures){
        uint64_t txtr1 = 0xA7F8F642; 
        uint64_t txtr2 = 0x99CAD11E; 
        uint64_t txtr3 = 0x6F5E55A8;
        wf.write(reinterpret_cast<char*>(&txtr1),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(&txtr2),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(&txtr3),sizeof(uint64_t));
        
        int texturesSize = 0;
        for (size_t i = 0; i < textures.size(); i++)
        {
            if(textures[i].isTexture)
                texturesSize++;
        }
        

        uint64_t textureElementSize = texturesSize;
        wf.write(reinterpret_cast<char*>(&textureElementSize),sizeof(uint64_t));
        uint64_t textureResolutionSize = 1024;
        wf.write(reinterpret_cast<char*>(&textureResolutionSize),sizeof(uint64_t));
        
        //----Get the texture data
        //TODO : Get the width & height
        Texture txtr;
        for (size_t i = 0; i < textures.size(); i++)
        {
            if(textures[i].isTexture){
                glActiveTexture(GL_TEXTURE28);
                glBindTexture(GL_TEXTURE_2D,textures[i].id);
                GLubyte* data = txtr.getTextureFromProgram(GL_TEXTURE28,1024,1024,4);
                for (size_t wh = 0; wh < 1024*1024*4; wh++)
                {
                    wf.write(reinterpret_cast<char*>(&data[wh]),sizeof(GLubyte));
                }
            }
        }
        //----Write the texture data
    }

    void drawTextureElements(std::ofstream &wf,std::vector<aTexture> textures){
        uint64_t txtrE1 = 0xB7A58C82;
        uint64_t txtrE2 = 0xF7F7B273;
        uint64_t txtrE3 = 0x3C82A6FF;
        wf.write(reinterpret_cast<char*>(&txtrE1),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(&txtrE2),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(&txtrE3),sizeof(uint64_t));

        uint64_t textureElementSize = textures.size();
        wf.write(reinterpret_cast<char*>(&textureElementSize),sizeof(uint64_t));

        for (size_t i = 0; i < textures.size(); i++)
        {
            sTexture stexture;
            stexture.id = textures[i].id;
            stexture.isTexture = textures[i].isTexture;
            stexture.isTrashFolder = textures[i].isTrashFolder;
            stexture.folderIndex = textures[i].folderIndex;

            wf.write(reinterpret_cast<char*>(&stexture),sizeof(sTexture));
            
            uint64_t txtrNameSize = textures[i].name.size();
            wf.write(reinterpret_cast<char*>(&txtrNameSize),sizeof(uint64_t));

            for (size_t cI = 0; cI < textures[i].name.size(); cI++)
            {
                wf.write(reinterpret_cast<char*>(&textures[i].name[cI]),sizeof(char));
            }
        }
    }

    void writeNodeScenes(std::ofstream &wf,std::vector<NodeScene> nodeScenes){
        uint64_t nodeSC1 = 0x0A78DA67; 
        uint64_t nodeSC2 = 0xF99C3CA7; 
        uint64_t nodeSC3 = 0x36CB89A5;
        wf.write(reinterpret_cast<char*>(&nodeSC1),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(&nodeSC2),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(&nodeSC3),sizeof(uint64_t));

        uint64_t nodeSceneSize = nodeScenes.size();
        wf.write(reinterpret_cast<char*>(&nodeSceneSize),sizeof(uint64_t));

        for (size_t i = 0; i < nodeScenes.size(); i++)
        {
            wf.write(reinterpret_cast<char*>(&nodeScenes[i]),sizeof(NodeScene));
        }   
    }
};