/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef THREED_THREED_BOX_HPP
#define THREED_THREED_BOX_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <string>
#include <vector>

#include"3D/Mesh/Mesh.hpp"

class ThreeDBox
{
public:
    unsigned int VBO = 0;
    unsigned int VAO = 0;
    unsigned int EBO = 0;

    glm::vec3 pos_topLeft;
    glm::vec3 pos_topRight;
    glm::vec3 pos_bottomLeft;
    glm::vec3 pos_bottomRight;
    glm::vec3 normal;

    std::vector<Vertex> boxVertices;
    std::vector<unsigned int> boxIndices;

    bool flipX = false;
    bool flipY = false;

    ThreeDBox();

    void init(glm::vec3 pos_topLeft, glm::vec3 pos_topRight, glm::vec3 pos_bottomLeft, glm::vec3 pos_bottomRight, glm::vec3 normal);
    void update(glm::vec3 pos_topLeft, glm::vec3 pos_topRight, glm::vec3 pos_bottomLeft, glm::vec3 pos_bottomRight, glm::vec3 normal);
    void projectToModel(std::vector<Vertex>& vertices, glm::vec3 center);

    void updateDetailed(
                            glm::vec3 pos_topLeft, 
                            glm::vec3 pos_topRight, 
                            glm::vec3 pos_bottomLeft, 
                            glm::vec3 pos_bottomRight, 
                            glm::vec3 normal,
                            
                            glm::vec3 detailed_threeDPoint_r1_c2, int detailed_threeDPoint_r1_c2_index, 
                            glm::vec3 detailed_threeDPoint_r1_c3, int detailed_threeDPoint_r1_c3_index, 
                            glm::vec3 detailed_threeDPoint_r1_c4, int detailed_threeDPoint_r1_c4_index, 
                            glm::vec3 detailed_threeDPoint_r2_c1, int detailed_threeDPoint_r2_c1_index, 
                            glm::vec3 detailed_threeDPoint_r2_c2, int detailed_threeDPoint_r2_c2_index, 
                            glm::vec3 detailed_threeDPoint_r2_c3, int detailed_threeDPoint_r2_c3_index, 
                            glm::vec3 detailed_threeDPoint_r2_c4, int detailed_threeDPoint_r2_c4_index, 
                            glm::vec3 detailed_threeDPoint_r2_c5, int detailed_threeDPoint_r2_c5_index, 
                            glm::vec3 detailed_threeDPoint_r3_c1, int detailed_threeDPoint_r3_c1_index, 
                            glm::vec3 detailed_threeDPoint_r3_c2, int detailed_threeDPoint_r3_c2_index, 
                            glm::vec3 detailed_threeDPoint_r3_c3, int detailed_threeDPoint_r3_c3_index, 
                            glm::vec3 detailed_threeDPoint_r3_c4, int detailed_threeDPoint_r3_c4_index, 
                            glm::vec3 detailed_threeDPoint_r3_c5, int detailed_threeDPoint_r3_c5_index, 
                            glm::vec3 detailed_threeDPoint_r4_c1, int detailed_threeDPoint_r4_c1_index, 
                            glm::vec3 detailed_threeDPoint_r4_c2, int detailed_threeDPoint_r4_c2_index, 
                            glm::vec3 detailed_threeDPoint_r4_c3, int detailed_threeDPoint_r4_c3_index, 
                            glm::vec3 detailed_threeDPoint_r4_c4, int detailed_threeDPoint_r4_c4_index, 
                            glm::vec3 detailed_threeDPoint_r4_c5, int detailed_threeDPoint_r4_c5_index, 
                            glm::vec3 detailed_threeDPoint_r5_c2, int detailed_threeDPoint_r5_c2_index, 
                            glm::vec3 detailed_threeDPoint_r5_c3, int detailed_threeDPoint_r5_c3_index, 
                            glm::vec3 detailed_threeDPoint_r5_c4, int detailed_threeDPoint_r5_c4_index
                        );
    
    void getDetailedVertices(
                                glm::vec3* detailed_threeDPoint_r1_c2, int* detailed_threeDPoint_r1_c2_index, 
                                glm::vec3* detailed_threeDPoint_r1_c3, int* detailed_threeDPoint_r1_c3_index, 
                                glm::vec3* detailed_threeDPoint_r1_c4, int* detailed_threeDPoint_r1_c4_index, 
                                glm::vec3* detailed_threeDPoint_r2_c1, int* detailed_threeDPoint_r2_c1_index, 
                                glm::vec3* detailed_threeDPoint_r2_c2, int* detailed_threeDPoint_r2_c2_index, 
                                glm::vec3* detailed_threeDPoint_r2_c3, int* detailed_threeDPoint_r2_c3_index, 
                                glm::vec3* detailed_threeDPoint_r2_c4, int* detailed_threeDPoint_r2_c4_index, 
                                glm::vec3* detailed_threeDPoint_r2_c5, int* detailed_threeDPoint_r2_c5_index, 
                                glm::vec3* detailed_threeDPoint_r3_c1, int* detailed_threeDPoint_r3_c1_index, 
                                glm::vec3* detailed_threeDPoint_r3_c2, int* detailed_threeDPoint_r3_c2_index, 
                                glm::vec3* detailed_threeDPoint_r3_c3, int* detailed_threeDPoint_r3_c3_index, 
                                glm::vec3* detailed_threeDPoint_r3_c4, int* detailed_threeDPoint_r3_c4_index, 
                                glm::vec3* detailed_threeDPoint_r3_c5, int* detailed_threeDPoint_r3_c5_index, 
                                glm::vec3* detailed_threeDPoint_r4_c1, int* detailed_threeDPoint_r4_c1_index, 
                                glm::vec3* detailed_threeDPoint_r4_c2, int* detailed_threeDPoint_r4_c2_index, 
                                glm::vec3* detailed_threeDPoint_r4_c3, int* detailed_threeDPoint_r4_c3_index, 
                                glm::vec3* detailed_threeDPoint_r4_c4, int* detailed_threeDPoint_r4_c4_index, 
                                glm::vec3* detailed_threeDPoint_r4_c5, int* detailed_threeDPoint_r4_c5_index, 
                                glm::vec3* detailed_threeDPoint_r5_c2, int* detailed_threeDPoint_r5_c2_index, 
                                glm::vec3* detailed_threeDPoint_r5_c3, int* detailed_threeDPoint_r5_c3_index, 
                                glm::vec3* detailed_threeDPoint_r5_c4, int* detailed_threeDPoint_r5_c4_index
                            );

    void draw();
};

#endif