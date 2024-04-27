/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef VIDEO_HPP
#define VIDEO_HPP

//OpenGL & window 
#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

//OpenGL Math Library GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector> //std::vector
#include <string> //std::string
#include <map> //std::map
#include <atomic>

#include "UTIL/Util.hpp"

class Video{
private:
    std::vector<unsigned char*> frames;

    /*! @brief Frames will be loaded to this color buffer*/
    Texture color_buffer;
    
    /*! @brief Generates the color_buffer using resolution & channels value */
    void generate_color_buffer();

    /*! @brief Updates the color_buffer using resolution & channels value. Texture comes from frames[frame] */
    void update_color_buffer();
    
    glm::ivec2 resolution = glm::vec2(0);
    short channels = 0;

    /*! @brief Indicates which frame currently will be rendered. In the frames vector*/
    int frame = 0;
    float frame_mixVal = 0.;

    int fps = 0;
    
public:
    /*! @brief Loads the video in image formats using multiple images (frame by frame) in a folder.*/
    void load_folder(const std::string folder_path, int fps);

    /*! @brief Renders the video. render_mode(0 = play and stop, 1 = constantly play). Set the update_frame_value flag false if rendering multiple times at the same time (true for only once at the beginning)*/
    void render(Timer& timer, glm::vec3 position, glm::vec2 scale, float opacity, int render_mode, bool update_frame_value);
};

#endif