#include <iostream>
#include <ostream>
#include <fstream>
#include <sstream>

#include "../../thirdparty/include/glad/glad.h"

#include "Model/model.h"
#include "UI/UserInterface.h"
#include "Texture/Texture.h"

//LIGID PAINTER PROJECT FILE GENERATOR
//This single header generates *.ligid file witch contains loaded 3d model & textures

//Documentation

//Every LigidPainter project file shall start with 0x94DA0DB9 0xF53B4451 0xD0E355DE   

//3D Model code 0xB2911B6A 0x6825EA68 0x59EF9DF5

//Vertices code 0x1DCEC243 0xD25EF96B 0xF6A94C0C + size of the vertices 
//Example : 0x1DCEC243 0xD25EF96B 0xF6A94C0C 0x003D0900 (same goes for the indices)

//Indices code 0x3600353C 0xB3ACAD02 0x6C23DE2C + size of the indices

//Texture code 0xA7F8F642 0x99CAD11E 0x6F5E55A8 + size of the texture elements + texture resolution
//Example : 0xA7F8F642 0x99CAD11E 0x6F5E55A8 0x0000000a 0x00000400


//TODO : Multiple meshes

class LigidFile{
public:
    Model writeTheFile(const char* path,Model model,std::vector<aTexture> textures){

        std::ofstream wf = createTheProjectFile(path);

        writeModelData(wf,model);
        writeTextures(wf,textures);

        wf.close();

        return model;
    }
private:
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
             wf.write(reinterpret_cast<char*>(&vert1),sizeof(uint64_t));
             wf.write(reinterpret_cast<char*>(&vert2),sizeof(uint64_t));
             wf.write(reinterpret_cast<char*>(&vert3),sizeof(uint64_t));
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
        
        uint64_t textureElementSize = textures.size();
        wf.write(reinterpret_cast<char*>(&textureElementSize),sizeof(uint64_t));
        uint64_t textureResolutionSize = 1024;
        wf.write(reinterpret_cast<char*>(&textureResolutionSize),sizeof(uint64_t));
        
        std::vector<GLubyte*> textureDatas;
        
        //----Get the texture data
        //TODO : Get the width & height
        Texture txtr;
        for (size_t i = 0; i < textures.size(); i++)
        {
            if(textures[i].isTexture){
                glActiveTexture(GL_TEXTURE28);
                glBindTexture(GL_TEXTURE_2D,textures[i].id);
                GLubyte* data = txtr.getTextureFromProgram(GL_TEXTURE28,1024,1024,4);
                textureDatas.push_back(data);
            }
        }
        //----Write the texture data
        for (size_t i = 0; i < textureDatas.size(); i++)
        {
            for (size_t wh = 0; wh < 1024*1024; wh++)
            {
                wf.write(reinterpret_cast<char*>(&textureDatas[i][wh]),sizeof(GLubyte));
            }
        }
    }
};