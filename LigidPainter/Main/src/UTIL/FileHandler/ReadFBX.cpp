/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    IMPLEMENTED FROM THE BLENDER'S FBX FILE FORMAT DOCUMENTATION

    https://code.blender.org/2013/08/fbx-binary-file-format-specification/#binary-file-structure

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

#include <UTIL/FileHandler/OpenFBX/miniz.h>
#include <UTIL/FileHandler/OpenFBX/ofbx.h>

Model readFBXFile(std::string path){
    // Ignoring certain nodes will only stop them from being processed not tokenised (i.e. they will still be in the tree)
	ofbx::LoadFlags flags =
		ofbx::LoadFlags::TRIANGULATE |
//		ofbx::LoadFlags::IGNORE_MODELS |
		ofbx::LoadFlags::IGNORE_BLEND_SHAPES |
		ofbx::LoadFlags::IGNORE_CAMERAS |
		ofbx::LoadFlags::IGNORE_LIGHTS |
//		ofbx::LoadFlags::IGNORE_TEXTURES |
		ofbx::LoadFlags::IGNORE_SKIN |
		ofbx::LoadFlags::IGNORE_BONES |
		ofbx::LoadFlags::IGNORE_PIVOTS |
//		ofbx::LoadFlags::IGNORE_MATERIALS |
		ofbx::LoadFlags::IGNORE_POSES |
		ofbx::LoadFlags::IGNORE_VIDEOS |
		ofbx::LoadFlags::IGNORE_LIMBS |
//		ofbx::LoadFlags::IGNORE_MESHES |
		ofbx::LoadFlags::IGNORE_ANIMATIONS;

    FILE* fp = fopen(path.c_str(), "rb");

	if (!fp){
        std::cout << "Failed to load Fbx file can't open path : " << path << std::endl;
        return Model();  
    } 

	fseek(fp, 0, SEEK_END);
	long file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	auto* content = new ofbx::u8[file_size];
	fread(content, 1, file_size, fp);

	ofbx::IScene* g_scene = ofbx::load((ofbx::u8*)content, file_size, (ofbx::u16)flags);

    int obj_idx = 0;
	int indices_offset = 0;
	int normals_offset = 0;
	int mesh_count = g_scene->getMeshCount();

    Model model;

    std::vector<glm::vec3> _vert_pos;
    std::vector<glm::vec2> _vert_UV;
    std::vector<glm::vec3> _vert_normal;
    std::vector<int> _indices;

	for (int i = 0; i < mesh_count; ++i)
	{
        Mesh msh;
        std::vector<Vertex> _vertices;

		fprintf(fp, "o obj%d\ng grp%d\n", i, obj_idx);

		const ofbx::Mesh* mesh = g_scene->getMesh(i);
		const ofbx::Geometry* geom = mesh->getGeometry();
		
        int vertex_count = geom->getVertexCount();
		const ofbx::Vec3* vertices = geom->getVertices();
		for (int i = 0; i < vertex_count; ++i)
		{
			ofbx::Vec3 v = vertices[i];
            glm::vec3 _v;
            _v.x = v.x;
            _v.y = v.y;
            _v.z = v.z;
            _vert_pos.push_back(_v);
		}

		bool has_normals = geom->getNormals() != nullptr;
		if (has_normals)
		{
			const ofbx::Vec3* normals = geom->getNormals();
			int count = geom->getIndexCount();

			for (int i = 0; i < count; ++i)
			{
				ofbx::Vec3 n = normals[i];
                glm::vec3 _n;
                _n.x = n.x;
                _n.y = n.y;
                _n.z = n.z;
                _vert_normal.push_back(_n);
			}
		}

		bool has_uvs = geom->getUVs() != nullptr;
		if (has_uvs)
		{
			const ofbx::Vec2* uvs = geom->getUVs();
			int count = geom->getIndexCount();

			for (int i = 0; i < count; ++i)
			{
				ofbx::Vec2 uv = uvs[i];
                glm::vec2 _uv;
                _uv.x = uv.x;
                _uv.y = uv.y;
                _vert_UV.push_back(_uv);
			}
		}
        
		const int* faceIndices = geom->getFaceIndices();
		int index_count = geom->getIndexCount();
		bool new_face = true;
		for (int i = 0; i < index_count; ++i)
		{
			_indices.push_back(faceIndices[i]);
		}

		indices_offset += vertex_count;
		normals_offset += index_count;
		++obj_idx;
	}
    
    if(_vert_pos.size() > 100)
        return Model();

    std::cout << "Positions : " << std::endl;
    for (size_t i = 0; i < _vert_pos.size(); i++)
    {
        std::cout <<  _vert_pos[i].x << ' ' << _vert_pos[i].y << ' ' << _vert_pos[i].z << std::endl;
    }
    
    std::cout << "UV : " << std::endl;
    for (size_t i = 0; i < _vert_UV.size(); i++)
    {
        std::cout <<  _vert_UV[i].x << ' ' << _vert_UV[i].y  << std::endl;
    }
    
    std::cout << "Normal : " << std::endl;
    for (size_t i = 0; i < _vert_normal.size(); i++)
    {
        std::cout <<  _vert_normal[i].x << ' ' << _vert_normal[i].y << ' ' << _vert_normal[i].z << std::endl;
    }
    
    std::cout << "Indices : " << std::endl;
    for (size_t i = 0; i < _indices.size(); i++)
    {
        std::cout << _indices[i] << std::endl;
    }
    
    return Model();
}