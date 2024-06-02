/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef VECTOR_SCENE_HPP
#define VECTOR_SCENE_HPP

//OpenGL & window 
#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

//OpenGL Math Library GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UTIL/Util.hpp"
#include "GUI/Elements.hpp"
#include "3D/ThreeDPoint/ThreeDPoint.hpp"

class PaintSettings; 

struct VectorPoint2D{
    /*! @brief Position of the point (0 - 100 scale) */
    glm::vec2 pos = glm::vec2(0.f);

    /*! @brief This flag indicates if the point was hovered by the cursor*/
    bool hover = false;
    
    /*! @brief This flag indicates if the point was pressed and ready to move*/
    bool canMove = false;
    
    /*! @brief This flag indicates if the point was pressed and ready to move*/
    bool active = false;
    
    /*! @brief This value gradually becomes 1.f as the hover flag set to true*/
    float hoverMixVal = 0.f;
    
    /*! @brief This value gradually becomes 1.f as the active flag set to true*/
    float activeMixVal = 0.f;

    void render(Timer& timer, bool doMouseTracking, float scaleDivider, glm::vec4 color);
};

struct VectorStroke{
    VectorPoint2D startPoint;
    VectorPoint2D endPoint;
    VectorPoint2D offsetPoint;

    VectorStroke(){}
    VectorStroke(glm::vec2 startPos, glm::vec2 endPos, glm::vec2 offsetPos){
        this->startPoint.pos = startPos;
        this->endPoint.pos = endPos;
        this->offsetPoint.pos = offsetPos;
    }
    
    void renderCurve(float edge, glm::vec2 start, glm::vec2 dest, glm::vec2 offset);
    void draw(Timer& timer, float edge, bool doMouseTracking, std::vector<VectorStroke>& strokes, int curI);
    void draw_single_stroke(Timer& timer, float edge);
};

struct VertexUTIL {
    // position
    glm::vec3 Position = glm::vec3(0.f);
    // texCoords
    glm::vec2 TexCoords = glm::vec2(0.f);
    // normal
    glm::vec3 Normal = glm::vec3(0.f);
    // tangent
    glm::vec3 Tangent = glm::vec3(0);
    // bitangent
    glm::vec3 Bitangent = glm::vec3(0);

    VertexUTIL(){}
    VertexUTIL(glm::vec3 Position, glm::vec2 TexCoords, glm::vec3 Normal, glm::vec3 Tangent, glm::vec3 Bitangent){
        this->Position = Position;
        this->TexCoords = TexCoords;
        this->Normal = Normal;
        this->Tangent = Tangent;
        this->Bitangent = Bitangent;
    }
};

struct VectorStroke3D{
public:
    ThreeDPoint startPoint;
    ThreeDPoint endPoint;

    void updateLinePoints();

    std::vector<VertexUTIL> lineVertices;

    VectorStroke3D(){}
    VectorStroke3D(ThreeDPoint startPoint, ThreeDPoint endPoint);

    bool draw(Timer& timer, float edge, bool doMouseTracking, std::vector<VectorStroke3D>& strokes, int curI);

private:
    void projectToModel(std::vector<VertexUTIL>& vertices, glm::vec3 center);
    void genLineVertices(); 
    void renderLine(); 
};


class VectorScene{
public:
    std::vector<VectorStroke> strokes_2D;
    std::vector<VectorStroke3D> strokes_3D;

    void render_scene(Timer& timer, bool doMouseTracking, bool threeD, bool enable_stroke);

    void apply_strokes(bool threeD, bool twoDWrap, PaintSettings paint_settings);
    void delete_selected_points(bool threeD);
    void clear_points(bool threeD);
    void subdivide_selected_points(bool threeD);

    VectorScene();
    VectorScene(std::vector<VectorStroke> strokes_2D, std::vector<VectorStroke3D> strokes_3D);

    void update3DVectorBuffers();
    
    struct StrokeDialog{
        int paint_mode = 0;
        bool twoD_wrap_mode = 0;

        void show_stroke_dialog(bool threeD, VectorScene* vector_scene);
        void set_properties(int paint_mode, bool twoD_wrap_mode);
    };

    StrokeDialog stroke_dialog;

    void render2DVectors(Timer& timer, bool doMouseTracking);
    void render3DVectors(Timer& timer, bool doMouseTracking);

    void addNew2DVector();
    void addNew3DVector();
    bool isAny2DPointsActive();
    bool isAnyWrappedPointsActive();

    bool write_data(std::ofstream& wf);
    bool read_data(std::ifstream& rf);
};

#endif