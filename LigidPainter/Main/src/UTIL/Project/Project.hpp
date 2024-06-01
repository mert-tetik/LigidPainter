/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef UTIL_PROJECT_HPP
#define UTIL_PROJECT_HPP

#include <string>
#include <mutex>

extern std::mutex project_mutex;
std::string project_path();
std::string project_title();
std::string project_recover_path(int slot);

extern bool project_discard_update_flag;    

/// @brief Create a project from scratch
/// @param destinationPath where the project folder be created
/// @param name title of the project
/// @return true if success
bool project_create(std::string destinationPath, std::string name, std::vector<std::string> TDModelPaths);

/// @brief load an existing project using ligid file path
/// @param ligidFilePath path to the ligid file
/// @return 
bool project_load(std::string ligidFilePath);

/// @brief update the existing project (in the destination of the public member variable folderPath) (write files in the library)
void project_update(bool updateTextures, bool recover_update_mode, std::atomic<bool>* is_started);

void project_save_as(std::string dstPath, std::atomic<bool>* is_active);

/// @brief Loads each library elements (textures, materials, brushes, models etc.) into the Library
bool project_load_library_elements(std::string folderPath, std::string ligidFilePath, bool lock);

/// @brief 
void project_add_model(std::string filePath);

/// @brief Locates the ligid file in the folderPath param. Returns "" if there's no ligid file. 
std::string project_locate_ligid_file(std::string folderPath);
/// @brief Locates the ligid file in this->folderPath. Returns "" if there's no ligid file. 
std::string project_locate_ligid_file();

#endif