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

#include "GUI/Dialogs.hpp"

#include "Toolkit/VectorScene/VectorScene.hpp"

#include <string>
#include <vector>

/* ----------------------------------------- 3D ------------------------------------------ */

struct SinglePoint3D{
    glm::vec3 point_pos = glm::vec3(0.f);
    glm::vec3 point_normal = glm::vec3(0.f);
    bool is_active = false;
    SinglePoint3D(){}
    SinglePoint3D(glm::vec3 point_pos, glm::vec3 point_normal, bool is_active){
        this->point_pos = point_pos; 
        this->point_normal = point_normal; 
        this->is_active = is_active;
    }
};

static bool does_single_point_included_3D(std::vector<SinglePoint3D> vec, glm::vec3 pos){
    for(SinglePoint3D& point : vec){
        if(point.point_pos == pos)
            return true;
    }

    return false;
}

static std::vector<SinglePoint3D> calculate_single_points_3D(std::vector<VectorStroke3D> strokes_3D){
    std::vector<SinglePoint3D> result;

    for(VectorStroke3D& stroke : strokes_3D){
        if(!does_single_point_included_3D(result, stroke.startPoint.pos)){
            result.push_back(SinglePoint3D(stroke.startPoint.pos, stroke.startPoint.normal, stroke.startPoint.active));
        }
        if(!does_single_point_included_3D(result, stroke.endPoint.pos)){
            result.push_back(SinglePoint3D(stroke.endPoint.pos, stroke.endPoint.normal, stroke.endPoint.active));
        }
    }

    return result;
}

static void delete_selected_3D_strokes(VectorScene* vector_scene){
    if(!vector_scene->isAnyWrappedPointsActive())
        return;
        
    dialog_log.registerVectorAction("Selected wrapped point deleted", vector_scene);

    while(vector_scene->isAnyWrappedPointsActive()){
        std::vector<SinglePoint3D> points = calculate_single_points_3D(vector_scene->strokes_3D);
        
        for (size_t i = 0; i < points.size(); i++)
        {
            if(points[i].is_active){
                points.erase(points.begin() + i);
                i--;
            }
        }
        
        // Re-generate the strokes array
        vector_scene->strokes_3D.clear();
        for (size_t i = 0; i < points.size(); i++){
            VectorStroke3D stroke;
            stroke.startPoint = ThreeDPoint(points[i].point_pos, points[i].point_normal);

            bool add_to_array = false;
            
            if(i + 1 < points.size()){
                stroke.endPoint = ThreeDPoint(points[i + 1].point_pos, points[i + 1].point_normal);
                add_to_array = true;
            }
            else{
                if(points.size() == 1){
                    // Single stroke point
                    stroke.endPoint = stroke.startPoint;
                    add_to_array = true;
                }
                else{
                    // No stroke for the last point
                    add_to_array = false;
                }
            }
            
            if(add_to_array){
                vector_scene->strokes_3D.push_back(stroke);
            }
        }
        
    }

    vector_scene->update3DVectorBuffers();
}














/* ----------------------------------------- 2D ------------------------------------------ */

struct SinglePoint2D{
    glm::vec2 point_pos = glm::vec2(0.f);
    bool is_active = false;
    SinglePoint2D(){}
    SinglePoint2D(glm::vec2 point_pos, bool is_active){
        this->point_pos = point_pos; 
        this->is_active = is_active;
    }
};

static bool does_single_point_included_2D(std::vector<SinglePoint2D> vec, glm::vec2 pos){
    for(SinglePoint2D& point : vec){
        if(point.point_pos == pos)
            return true;
    }

    return false;
}

static std::vector<SinglePoint2D> calculate_single_points_2D(std::vector<VectorStroke> strokes_2D){
    std::vector<SinglePoint2D> result;

    for(VectorStroke& stroke : strokes_2D){
        if(!does_single_point_included_2D(result, stroke.startPoint.pos)){
            result.push_back(SinglePoint2D(stroke.startPoint.pos, stroke.startPoint.active));
        }
        if(!does_single_point_included_2D(result, stroke.endPoint.pos)){
            result.push_back(SinglePoint2D(stroke.endPoint.pos, stroke.endPoint.active));
        }
    }

    return result;
}

static void delete_selected_2D_strokes(VectorScene* vector_scene){
    if(!vector_scene->isAny2DPointsActive())
        return;

    dialog_log.registerVectorAction("Selected point deleted", vector_scene);
    while (vector_scene->isAny2DPointsActive()){
        for (size_t i = 0; i < vector_scene->strokes_2D.size(); i++)
        {
            if(i == 0){
                if(vector_scene->strokes_2D[i].startPoint.active && vector_scene->strokes_2D[i].endPoint.active && vector_scene->strokes_2D.size() == 1){
                    vector_scene->strokes_2D.clear(); 
                    break;
                }
                
                if(vector_scene->strokes_2D[i].startPoint.active && vector_scene->strokes_2D.size() == 1){
                    vector_scene->strokes_2D[i].startPoint.pos = vector_scene->strokes_2D[i].endPoint.pos; 
                    vector_scene->strokes_2D[i].offsetPoint.pos = vector_scene->strokes_2D[i].endPoint.pos; 
                    vector_scene->strokes_2D[i].startPoint.active = false; 
                    break;
                }
                
                if(vector_scene->strokes_2D[i].endPoint.active && vector_scene->strokes_2D.size() == 1){
                    vector_scene->strokes_2D[i].endPoint.pos = vector_scene->strokes_2D[i].startPoint.pos; 
                    vector_scene->strokes_2D[i].offsetPoint.pos = vector_scene->strokes_2D[i].startPoint.pos; 
                    vector_scene->strokes_2D[i].endPoint.active = false; 
                    break;
                }

                if(vector_scene->strokes_2D[i].startPoint.active){
                    vector_scene->strokes_2D.erase(vector_scene->strokes_2D.begin() + i);
                    break;
                }
                
                if(vector_scene->strokes_2D[i].endPoint.active && vector_scene->strokes_2D.size() > 1){
                    vector_scene->strokes_2D[i].endPoint.pos = vector_scene->strokes_2D[i + 1].endPoint.pos; 
                    vector_scene->strokes_2D[i].endPoint.active = vector_scene->strokes_2D[i + 1].endPoint.active; 
                    vector_scene->strokes_2D.erase(vector_scene->strokes_2D.begin() + i + 1);
                    break;
                }
            }
            else{
                if(vector_scene->strokes_2D[i].endPoint.active){
                    vector_scene->strokes_2D.erase(vector_scene->strokes_2D.begin() + i);
                    if(i - 1 < vector_scene->strokes_2D.size() && i < vector_scene->strokes_2D.size()){
                        vector_scene->strokes_2D[i].startPoint.pos = vector_scene->strokes_2D[i - 1].endPoint.pos; 
                        vector_scene->strokes_2D[i].startPoint.active = false; 
                    }
                    break;
                }
            }
        }
    }
}







void VectorScene::delete_selected_points(bool threeD){
    if(!threeD){
        delete_selected_2D_strokes(this);
    }
    else{   
        delete_selected_3D_strokes(this);
    }
}