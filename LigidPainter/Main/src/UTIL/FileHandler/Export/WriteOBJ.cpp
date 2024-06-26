/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Basic *.obj file exporter

    v - vertex pos
    vt - vertex texture coordinates
    vn - vertex normal vector
    f - face
    usemtl - indicate the material
    # - comment lines

*/

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"

// ---- Forward declerations for the utility functions ----

void writeVec3(std::ofstream &wf, glm::vec3 val, int precision);
void writeVec2(std::ofstream &wf, glm::vec2 val, int precision);
void calculateUniqueValues(std::vector<glm::vec3>& uniquePos, std::vector<glm::vec2>& uniqueUV, std::vector<glm::vec3>& uniqueNormal, Model& model);
void writeUniqueValues(std::ofstream& wf, std::vector<glm::vec3>& uniquePos, std::vector<glm::vec2>& uniqueUV, std::vector<glm::vec3>& uniqueNormal);
void writeIndices(std::ofstream& wf, std::vector<glm::vec3>& uniquePos, std::vector<glm::vec2>& uniqueUV, std::vector<glm::vec3>& uniqueNormal, Model& model);


bool FileHandler::writeOBJFile(std::string path, Model model){
    std::ofstream wf(path, std::ios::out);

    if(!wf) {
        LGDLOG::start<< "ERROR WHILE WRITING *.OBJ FILE! Cannot open file : " << path << LGDLOG::end;
        return false;
    }
    
    //Comment header (indicate this is a obj file) (not necessary tho)
    wf << "# Wavefront OBJ File \n";
    
    //Comment header
    wf << "# LigidPainter Exporter \n";

    //Insert space
    wf << '#' << '\n';
    wf << '#' << '\n';
    wf << '#' << '\n';

    //All the unique position values in the model (every mesh)
    std::vector<glm::vec3> uniquePos;

    //All the unique texture coordinates values in the model (every mesh)
    std::vector<glm::vec2> uniqueUV;

    //All the unique normal vector values in the model (every mesh)
    std::vector<glm::vec3> uniqueNormal;

    //Calculate unique values (takes all the vectors as reference and inserts the vertex data into them)
    calculateUniqueValues(uniquePos, uniqueUV, uniqueNormal, model);

    //Write all the unqiue vectors to the obj file
    writeUniqueValues(wf, uniquePos, uniqueUV, uniqueNormal);

    //Write the index values
    writeIndices(wf, uniquePos, uniqueUV, uniqueNormal, model);

    //Close the obj file stream
    wf.close();

    return true;
}

















// ----------- UTILITY FUNCTIONS ------------





void writeIndices(std::ofstream& wf, std::vector<glm::vec3>& uniquePos, std::vector<glm::vec2>& uniqueUV, std::vector<glm::vec3>& uniqueNormal, Model& model){
    
    //All the meshes
    for (size_t meshI = 0; meshI < model.meshes.size(); meshI++)
    {
        for (size_t objI = 0; objI < model.meshes[meshI].objects.size(); objI++)
        {
            //Indicate the object 
            wf << "o " + model.meshes[meshI].objects[objI].title + '\n';
            
            //Smooth normals off (change that)
            if(meshI == 0)
                wf << "s off \n";

            //Write all the indices
            for (size_t indI = 0; indI < model.meshes[meshI].indices.size();)
            {
                try
                {
                    //Write a face
                
                    wf << "f ";

                    for (size_t i = 0; i < 3; i++)
                    {
                        Vertex currentVertex = model.meshes[meshI].vertices[model.meshes[meshI].indices[indI]]; 
                        
                        long additionalIndex = 0;
                        for (size_t i = 0; i < meshI; i++)
                        {
                            additionalIndex += model.meshes[i].vertices.size();
                        }
                        

                        //Find the index of the vertex pos
                        wf << model.meshes[meshI].indices[indI] + 1 + additionalIndex;

                        wf << '/';

                        //Find the index of the vertex texture coordinate
                        wf << model.meshes[meshI].indices[indI] + 1 + additionalIndex;
                        
                        wf << '/';

                        //Find the index of the vertex normal vector
                        wf << model.meshes[meshI].indices[indI] + 1 + additionalIndex;

                        if(i == 2)
                            wf << '\n';
                        else
                            wf << ' ';

                        indI++;
                    }
                    
                    
                }
                
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                }
            }

            
            //Indicate the material 
            wf << "usemtl " + model.meshes[meshI].materialName + '\n';
        }

    }
}

void writeUniqueValues(std::ofstream& wf, std::vector<glm::vec3>& uniquePos, std::vector<glm::vec2>& uniqueUV, std::vector<glm::vec3>& uniqueNormal){
    for (size_t vertI = 0; vertI < uniquePos.size(); vertI++)
    {
        //Indicate Pos
        wf << "v ";

        //Write the vector
        writeVec3(wf, uniquePos[vertI],6);

        //New line
        wf << '\n';
    }
    
    //Write all the vertex texture coordinates
    for (size_t vertI = 0; vertI < uniqueUV.size(); vertI++)
    {
        //Indicate Pos
        wf << "vt ";

        //Write the vector
        writeVec2(wf, uniqueUV[vertI], 6);

        //New line
        wf << '\n';
    }

    //Write all the vertex normal vectors
    for (size_t vertI = 0; vertI < uniqueNormal.size(); vertI++)
    {
        //Indicate Normal
        wf << "vn ";

        //Write the vector
        writeVec3(wf, uniqueNormal[vertI],4);

        //New line
        wf << '\n';
    }
}

void calculateUniqueValues(std::vector<glm::vec3>& uniquePos, std::vector<glm::vec2>& uniqueUV, std::vector<glm::vec3>& uniqueNormal, Model& model){
    for (size_t meshI = 0; meshI < model.meshes.size(); meshI++)
    {
        //Calculate unique vertex positions
        for (size_t vertI = 0; vertI < model.meshes[meshI].vertices.size(); vertI++)
        {
            uniquePos.push_back(model.meshes[meshI].vertices[vertI].Position);
        }
        
        //Calculate unique vertex texture coordinates
        for (size_t vertI = 0; vertI < model.meshes[meshI].vertices.size(); vertI++)
        {
            uniqueUV.push_back(model.meshes[meshI].vertices[vertI].TexCoords);
        }

        //Calculate unique vertex normal vectors
        for (size_t vertI = 0; vertI < model.meshes[meshI].vertices.size(); vertI++)
        {
            uniqueNormal.push_back(model.meshes[meshI].vertices[vertI].Normal);
        }
    }
}

void writeVec3(std::ofstream &wf, glm::vec3 val, int precision){

    //Write the vector
    wf << std::setprecision(precision) << val.x;
    wf << " ";
    wf << std::setprecision(precision) << val.y;
    wf << " ";
    wf << std::setprecision(precision) << val.z;
}

void writeVec2(std::ofstream &wf, glm::vec2 val, int precision){

    //Write the vector
    wf << std::fixed << std::setprecision(precision) << val.x;
    wf << std::fixed << std::setprecision(precision) << " ";
    wf << std::fixed << std::setprecision(precision) << val.y;
}