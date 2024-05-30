/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "3D/ThreeD.hpp"

#include "UTIL/Painting/Painter.hpp"

#include "GUI/Dialogs.hpp"

#include "Toolkit/VectorScene/VectorScene.hpp"

#include <string>
#include <vector>

/* 
    Bezier curve function
    
    B(t) = (1 - t)^2 * start + 2 * (1 - t) * t * offset + t^2 * end
*/
static glm::vec2 bezier(float t, glm::vec2 start, glm::vec2 end, glm::vec2 offset){
    return glm::pow((glm::vec2(1.f) - glm::vec2(t)), glm::vec2(2)) * start + glm::vec2(2) * (glm::vec2(1) - glm::vec2(t)) * glm::vec2(t) * offset + glm::pow(glm::vec2(t), glm::vec2(2)) * end;
}
static std::vector<glm::vec2> calculateBezierCurve(glm::vec2 start, glm::vec2 end, glm::vec2 direction, int frequency){
    
    std::vector<glm::vec2> points;

    for (size_t i = 0; i < frequency; i++)
    {
        points.push_back(bezier((float)i / ((float)frequency - 1.f), start, end, direction));
    }

    return points;
}

void VectorScene::apply_strokes(
                                    bool threeD,
                                    bool twoDWrap,
                                    PaintSettings paint_settings
                                )
{
    if(threeD){
        for (VectorStroke3D stroke3D : this->strokes_3D)
        {
            int stroke_pos_i = 0;
            for (VertexUTIL stroke_pos : stroke3D.lineVertices){
                if(stroke_pos_i % (int)std::ceil((paint_settings.stroke_brush.properties.spacing + 1.f) / 20.f) == 0){
                    paint_settings.point.use_3D = true;
                    paint_settings.point.point_3D = ThreeDPoint(stroke_pos.Position, stroke_pos.Normal);
                    painting_paint_buffers(paint_settings, stroke_pos_i == 0, stroke_pos_i == stroke3D.lineVertices.size() - 1);
                }

                stroke_pos_i++;
            }
        }
    }
    else{
        std::vector<glm::vec2> strokePositions;

        for (size_t vecI = 0; vecI < strokes_2D.size(); vecI++)
        {
            float distance = glm::distance(strokes_2D[vecI].startPoint.pos, strokes_2D[vecI].offsetPoint.pos) + glm::distance(strokes_2D[vecI].endPoint.pos, strokes_2D[vecI].offsetPoint.pos);
            unsigned int quality = (unsigned int)(distance / 2.f);

            std::vector<glm::vec2> points = calculateBezierCurve(
                                                                    strokes_2D[vecI].startPoint.pos / 100.f * *Settings::videoScale(), 
                                                                    strokes_2D[vecI].endPoint.pos / 100.f * *Settings::videoScale(), 
                                                                    strokes_2D[vecI].offsetPoint.pos / 100.f * *Settings::videoScale(), 
                                                                    quality
                                                                );

            // Add the points of a single vector to the total stroke positions
            for (size_t pointI = 0; pointI < points.size(); pointI++)
            {
                strokePositions.push_back(points[pointI]);
            }
        }
        
        const int maxStrokeSize = 50;

        // Calculate how many subvectors you'll need
        const int numSubvectors = (strokePositions.size() + maxStrokeSize - 1) / maxStrokeSize;

        // Loop through and process the subvectors
        for (int i = 0; i < numSubvectors; ++i) {
            
            // Calculate the start and end indices for each subvector
            int startIdx = i * maxStrokeSize;
            int endIdx = std::min((i + 1) * maxStrokeSize, static_cast<int>(strokePositions.size()));

            // Create a subvector from the original vector
            std::vector<glm::vec2> subVector(strokePositions.begin() + startIdx, strokePositions.begin() + endIdx);

            for (size_t i = 0; i < subVector.size(); i++)
            {
                paint_settings.point.use_3D = false;
                paint_settings.point.point_2D = subVector[i];
                painting_paint_buffers(paint_settings, i == 0, i == subVector.size() - 1);
            }
        }

        Settings::defaultFramebuffer()->FBO.bind();
        Settings::defaultFramebuffer()->setViewport();
    }
}