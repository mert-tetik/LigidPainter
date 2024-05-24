/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Used for rendering 3D lines

*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "UTIL/Util.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/GL/GL.hpp"

#include "3D/Line/Line.hpp"
    

static std::vector<std::vector<glm::vec3>> partitionVector(const std::vector<glm::vec3>& input, std::size_t maxChunkSize) {
    std::vector<std::vector<glm::vec3>> result;
    auto it = input.begin();

    while (it != input.end()) {
        // Create a new chunk with a maximum of maxChunkSize elements
        std::vector<glm::vec3> chunk;
        std::size_t remainingSize = std::distance(it, input.end());
        std::size_t currentChunkSize = std::min(maxChunkSize, remainingSize);
        chunk.insert(chunk.end(), it, it + currentChunkSize);

        // Move the iterator
        it += currentChunkSize;

        // Add the chunk to the result
        result.push_back(std::move(chunk));
    }

    return result;
}

static unsigned int line_VBO = 0;
static unsigned int line_VAO = 0;

void render_line(Line3D line, std::vector<glm::vec3> pos_offsets, Shader shader) {
    // Vertex data for a single line segment
    GLfloat vertices[] = {
        line.startPoint.x, line.startPoint.y, line.startPoint.z,   // Start point
        line.endPoint.x, line.endPoint.y, line.endPoint.z          // End point
    };

    LigidGL::cleanGLErrors();

    if (line_VAO == 0) {
        // Generate and bind the VAO
        glGenVertexArrays(1, &line_VAO);
        LigidGL::testGLError("glGenVertexArrays(1, &line_VAO);");
        glBindVertexArray(line_VAO);
        LigidGL::testGLError("glBindVertexArray(line_VAO);");

        // Generate and bind the VBO
        glGenBuffers(1, &line_VBO);
        LigidGL::testGLError("glGenBuffers(1, &line_VBO);");
        glBindBuffer(GL_ARRAY_BUFFER, line_VBO);
        LigidGL::testGLError("glBindBuffer(GL_ARRAY_BUFFER, line_VBO);");
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        LigidGL::testGLError("glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);");

        // Specify the layout of the vertex data
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
        LigidGL::testGLError("glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);");
        glEnableVertexAttribArray(0);
        LigidGL::testGLError("glEnableVertexAttribArray(0);");

        // Unbind the VAO to prevent accidental modifications
        glBindVertexArray(0);
        LigidGL::testGLError("glBindVertexArray(0);");
    }

    // Update vertex data
    glBindBuffer(GL_ARRAY_BUFFER, line_VBO);
    LigidGL::testGLError("glBindBuffer(GL_ARRAY_BUFFER, line_VBO);");
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    LigidGL::testGLError("glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);");

    // Bind the VAO and draw the line
    glBindVertexArray(line_VAO);
    LigidGL::testGLError("glBindVertexArray(line_VAO);");
    
    std::vector<std::vector<glm::vec3>> offsets = partitionVector(pos_offsets, 100);

    for (size_t vecI = 0; vecI < offsets.size(); vecI++)
    {
        for (size_t posI = 0; posI < offsets[vecI].size(); posI++)
        {
            shader.setVec3("offset_pos[" + std::to_string(posI) + "]", offsets[vecI][posI]);
        }
        
        glDrawArraysInstanced(GL_LINES, 0, 2, pos_offsets.size());
        LigidGL::testGLError("glDrawArraysInstanced(GL_LINES, 0, 2, pos_offsets.size());");
    }

    // Unbind the VAO
    glBindVertexArray(0);
    LigidGL::testGLError("glBindVertexArray(0);");
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    LigidGL::testGLError("glBindBuffer(GL_ARRAY_BUFFER, 0);");
}