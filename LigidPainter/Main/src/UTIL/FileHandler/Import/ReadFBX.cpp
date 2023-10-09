/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    IMPLEMENTED FROM THE BLENDER'S FBX FILE FORMAT DOCUMENTATION

    https://code.blender.org/2013/08/fbx-binary-file-format-specification/#binary-file-structure

    TODO : Process objects

*/

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

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

/*
*   0 : Don't print anything to the terminal
*   1 : Print node titles
*   2 : Print node titles with the property data
*/
#define LIGID_FBX_IMPORTER_PRINT_TEXT_STATE 0
#define LIGID_FBX_IMPORTER_TRIANGULATE true

// Forward declarations for the utilities
Model createModel(std::vector<std::vector<Vertex>> meshVertices, std::vector<std::vector<unsigned int>> meshIndices, std::vector<std::string> matTitles);
void seperateUnitedVertices(std::vector<std::vector<Vertex>>& unitedVertices, std::vector<std::vector<Vertex>>& meshVertices, std::vector<std::vector<unsigned int>>& meshIndices);
void calculateTangentBitangent(Vertex& v0, Vertex& v1, Vertex& v2);
std::vector<char> DecompressZlibChar(const std::vector<char>& compressedData, size_t uncompressedSize);
std::vector<float> DecompressZlibFloat(const std::vector<char>& compressedData, size_t numFloats);
std::vector<double> DecompressZlibDouble(const std::vector<char>& compressedData, size_t numDoubles);
std::vector<long long> DecompressZlibLongLong(const std::vector<char>& compressedData, size_t numLongLongs);
std::vector<int> DecompressZlibInt(const std::vector<char>& compressedData, size_t numInts);

struct FbxProperty {
    char typeCode;
    std::vector<char> data;

    double singleDoubleVal = 0.;
};

struct FbxNode {
    std::string nodeType;
    std::vector<FbxProperty> properties;
    std::vector<FbxNode> nestedNodes;
};

struct FBXTransform{
    glm::vec3 translation = glm::vec3(FLT_MAX); //Lcl Translation  
    glm::vec3 rotation = glm::vec3(FLT_MAX); //Lcl Rotation
    glm::vec3 scale = glm::vec3(FLT_MAX); //Lcl Scaling

    std::string currentReadMode = ""; 
};

// Forward declarations for the fbx file processing functions
bool ReadNestedNodes(std::ifstream& file, const std::string fileName, std::vector<FbxNode>& nestedNodes);
void ProcessNodeHierarchy(std::vector<FbxNode>& nodes, std::vector<std::vector<glm::vec3>>& vertPositions, std::vector<std::vector<glm::vec2>>& vertUVs, std::vector<std::vector<glm::vec3>>& vertNormals, std::vector<std::vector<int>>& polygonVertexIndices, std::vector<std::vector<int>>& edges, std::vector<std::vector<int>>& uvIndices,std::vector<std::string>& matTitles,std::vector<std::vector<int>>& materials, std::vector<FBXTransform> &transforms); 
static void parseFBXMeshData(std::vector<glm::vec3>& positions, const std::vector<glm::vec2>& UVs, const std::vector<glm::vec3>& normals, const std::vector<int>& polygonVertexIndices, const std::vector<int>& uvIndices, const std::vector<int>& edges, const std::vector<std::string>& matTitles, const std::vector<int>& materials, std::vector<std::vector<Vertex>>& meshVertices, std::vector<std::vector<unsigned int>>& meshIndices, FBXTransform transform);

int __node_counter = 0;
int _FBX_totalBitsRead = 0;
int objectI = -1;
int modelI = -1;
bool processingMesh = false;
bool processingShape = false;

/*
    HEADER

    Bytes 0 - 20: Kaydara FBX Binary  \x00 (file-magic, with 2 spaces at the end, then a NULL terminator).
    Bytes 21 - 22: [0x1A, 0x00] (unknown but all observed files show these bytes).
    Bytes 23 - 26: unsigned int, the version number. 7300 for version 7.3 for example.
*/
Model FileHandler::readFBXFile(std::string path) {
    _FBX_totalBitsRead = 0;
    __node_counter = 0;
    objectI = -1;
    modelI = -1;
    processingMesh = false;


    std::ifstream file(path, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "ERROR : Can't read FBX file. Can't open : " << path << LGDLOG::end;
        return Model();
    }

    // Read header
    char header[27];
    file.read(header, sizeof(header));
    _FBX_totalBitsRead += sizeof(header);

    // Extract the version number
    uint32_t version = *reinterpret_cast<uint32_t*>(&header[23]);

    if  ( /*      Supported Versions      */
            //version != 6000 && 
            version != 7100 &&
            version != 7200 &&
            version != 7300 &&
            version != 7400 
            //version != 7500 
        )
    {
        LGDLOG::start<< "ERROR Reading fbx file : unsupported fbx version : " << version << LGDLOG::end;
        return Model();
    }

    // Read top-level object record
    FbxNode topLevelObject;
    for (size_t i = 0; i < 10; i++)
    {
        ReadNestedNodes(file, path, topLevelObject.nestedNodes);
    }
        


    // Read header
    //file.read(header, sizeof(header));
    //_FBX_totalBitsRead += sizeof(header);

    std::vector<std::vector<glm::vec3>> positions;
    std::vector<std::vector<glm::vec2>> UVS;
    std::vector<std::vector<glm::vec3>> normals;
    std::vector<std::string> matTitles;
    std::vector<std::vector<int>> polygonVertexIndices;
    std::vector<std::vector<int>> edges;
    std::vector<std::vector<int>> uvIndices;
    std::vector<std::vector<int>> materials;
    std::vector<FBXTransform> transforms;

    // Process the FBX data
    ProcessNodeHierarchy(topLevelObject.nestedNodes, positions, UVS, normals, polygonVertexIndices , uvIndices, edges, matTitles, materials, transforms);


    if(!positions.size() || !edges.size() || !polygonVertexIndices.size() || !uvIndices.size()){
        LGDLOG::start<< "ERROR : Processing the fbx node hierarchy : Can't detect enough vertex data to create a mesh" << LGDLOG::end;
        return Model();
    }

    std::vector<std::vector<Vertex>> meshVertices;
    std::vector<std::vector<unsigned int>> meshIndices;

    if(!matTitles.size())
        matTitles.push_back("FirstDefMat");

    for (size_t i = 0; i < matTitles.size(); i++)
    {
        meshVertices.push_back({});
        meshIndices.push_back({});
    }
    

    for (size_t i = 0; i < objectI + 1; i++)
    {
        std::vector<std::vector<Vertex>> in_meshVertices;
        std::vector<std::vector<unsigned int>> in_meshIndices;
        parseFBXMeshData(  
                            positions[i],
                            UVS[i],
                            normals[i],
                            polygonVertexIndices[i],
                            uvIndices[i],
                            edges[i],
                            matTitles,
                            materials[i],
                            in_meshVertices,
                            in_meshIndices,
                            transforms[i]
                        );

        for (size_t i = 0; i < in_meshIndices.size(); i++)
        {
            for (size_t ii = 0; ii < in_meshIndices[i].size(); ii++)
            {
                meshIndices[i].push_back(in_meshIndices[i][ii] + meshVertices[i].size());
            }
        }
        for (size_t i = 0; i < in_meshVertices.size(); i++)
        {
            for (size_t ii = 0; ii < in_meshVertices[i].size(); ii++)
            {
                meshVertices[i].push_back(in_meshVertices[i][ii]);
            }
        }
    }
    


    if(meshVertices.size()){
        return createModel(meshVertices, meshIndices, matTitles);
    }
    else{
        LGDLOG::start<< "WARNING! Reading FBX file : No mesh data received" << LGDLOG::end;
        return Model();
    }
}




/*
    i) Primitive Types

        Y: 2 byte signed Integer
        C: 1 bit boolean (1: true, 0: false) encoded as the LSB of a 1 Byte value.
        I: 4 byte signed Integer
        F: 4 byte single-precision IEEE 754 number
        D: 8 byte double-precision IEEE 754 number
        L: 8 byte signed Integer
    
    For primitive scalar types the Data in the record is exactly the binary representation of the value, in little-endian byte order.

    ii) Array types

        f: Array of 4 byte single-precision IEEE 754 number (float)
        d: Array of 8 byte double-precision IEEE 754 number (double)
        l: Array of 8 byte signed Integer (long long)
        i: Array of 4 byte signed Integer (int)
        b: Array of 1 byte Booleans (bool)
*/

/*

    Array types

    Size (Bytes)	Data Type	Name
    -------------------------------------------
    4	            Uint32	    ArrayLength
    4	            Uint32	    Encoding
    4	            Uint32	    CompressedLength
    ?	            ?	        Contents

    If Encoding is 0, the Contents is just ArrayLength times the array data type
    If Encoding is 1, the Contents is a deflate/zip-compressed buffer of length CompressedLength bytes. The buffer can for example be decoded using zlib.
    Values other than 0,1 for Encoding have not been observed.


*/

/*
    iii) Special types

        S: String
        R: raw binary data

    Both of these have the following interpretation:
    
        Size (Bytes)	Data Type	    Name
        --------------------------------------
        4	            Uint32	        Length
        Length	        byte/char	    Data
*/
void ReadProperties(std::ifstream& file, std::vector<FbxProperty>& properties, uint32_t numProperties, uint32_t propertyListLen) {
    
// Loop through the properties
    for (int i = 0; i < numProperties; i++) {
        FbxProperty prop;


        if(!file.read(&prop.typeCode, sizeof(char))){}

        _FBX_totalBitsRead += sizeof(char);


        // Handle each property type accordingly
        switch (prop.typeCode) {

            //  ---------   PRIMITIVE TYPES   --------- 
            case 'Y':
            {
                // Read a 2-byte signed integer
                int16_t intValue;
                file.read(reinterpret_cast<char*>(&intValue), sizeof(intValue));
                _FBX_totalBitsRead += sizeof(intValue);
                prop.data.emplace_back(intValue);

                // Handle the integer value
                break;
            }
            case 'C':
            {
                // Read a 1-bit boolean
                bool boolValue;
                file.read(reinterpret_cast<char*>(&boolValue), sizeof(boolValue));
                _FBX_totalBitsRead += sizeof(boolValue);
                prop.data.emplace_back(boolValue);
                // Handle the boolean value
                break;
            }
            case 'I':
            {

                // Read a 4-byte signed integer
                int32_t intValue;
                file.read(reinterpret_cast<char*>(&intValue), sizeof(intValue));
                _FBX_totalBitsRead += sizeof(intValue);
                prop.data.emplace_back(intValue);
                // Handle the integer value
                break;
            }
            case 'F':
            {

                // Read a 4-byte floating-point value
                float floatValue;
                file.read(reinterpret_cast<char*>(&floatValue), sizeof(floatValue));
                _FBX_totalBitsRead += sizeof(floatValue);
                prop.data.emplace_back(floatValue);
                // Handle the floating-point value
                break;
            }
            case 'D':
            {
                // Read an 8-byte floating-point value
                double doubleValue;
                file.read(reinterpret_cast<char*>(&doubleValue), sizeof(doubleValue));
                _FBX_totalBitsRead += sizeof(doubleValue);
                prop.singleDoubleVal = doubleValue;
                // Handle the double value
                break;
            }
            case 'L':
            {
                // Read an 8-byte signed integer
                int64_t longValue;
                file.read(reinterpret_cast<char*>(&longValue), sizeof(longValue));
                _FBX_totalBitsRead += sizeof(longValue);
                prop.data.emplace_back(longValue);
                // Handle the long value
                break;
            }

            // ---------  ARRAY TYPES  ---------
            case 'f':
            {
                /* FLOAT ARRAY */

                uint32_t arrayLength;
                file.read(reinterpret_cast<char*>(&arrayLength), sizeof(uint32_t));
                _FBX_totalBitsRead += sizeof(uint32_t);
    	        
                uint32_t encoding;
                file.read(reinterpret_cast<char*>(&encoding), sizeof(uint32_t));
                _FBX_totalBitsRead += sizeof(uint32_t);
    	        
                uint32_t compressedLength;
                file.read(reinterpret_cast<char*>(&compressedLength), sizeof(uint32_t));
                _FBX_totalBitsRead += sizeof(uint32_t);

                if (encoding == 0) {

                    // Contents is ArrayLength times the array data type
                    std::vector<float> floatArray(arrayLength);
                    file.read(reinterpret_cast<char*>(floatArray.data()), sizeof(float) * arrayLength);
                    _FBX_totalBitsRead += sizeof(float) * arrayLength;
                    // Handle the float array
                
                    // Convert the float array to a byte vector
                    std::vector<char> byteArray(sizeof(float) * arrayLength);
                    std::memcpy(byteArray.data(), floatArray.data(), sizeof(float) * arrayLength);
                
                    // Store the byte vector in the FbxProperty struct
                    prop.data = byteArray;
                }

                else if (encoding == 1) {
                    // Contents is a deflate/zip-compressed buffer of length CompressedLength bytes
                    std::vector<char> compressedData(compressedLength);
                    file.read(compressedData.data(), compressedLength);
                    _FBX_totalBitsRead += compressedLength;

                    // Handle the compressed data using zlib
                    std::vector<float> decompressedData = DecompressZlibFloat(compressedData, sizeof(float) * arrayLength);
              
                    // Convert the float array to a byte vector
                    std::vector<char> byteArray(sizeof(float) * arrayLength);
                    std::memcpy(byteArray.data(), decompressedData.data(), sizeof(float) * arrayLength);
                
                    // Store the byte vector in the FbxProperty struct
                    prop.data = byteArray;
                }
                else {
                    LGDLOG::start<< "ERROR : Reading FBX unknown encoding value : " << encoding << LGDLOG::end;
                }

                break;
            }
            case 'd':
            {
                /*DOUBLE ARRAY*/
                
                uint32_t arrayLength;
                file.read(reinterpret_cast<char*>(&arrayLength), sizeof(uint32_t));
                _FBX_totalBitsRead += sizeof(uint32_t);
    	        
                uint32_t encoding;
                file.read(reinterpret_cast<char*>(&encoding), sizeof(uint32_t));
                _FBX_totalBitsRead += sizeof(uint32_t);
    	        
                uint32_t compressedLength;
                file.read(reinterpret_cast<char*>(&compressedLength), sizeof(uint32_t));
                _FBX_totalBitsRead += sizeof(uint32_t);

                if (encoding == 0) {

                    // Contents is ArrayLength times the array data type
                    std::vector<double> doubleArray(arrayLength);
                    file.read(reinterpret_cast<char*>(doubleArray.data()), sizeof(double) * arrayLength);
                    _FBX_totalBitsRead += sizeof(double) * arrayLength;
                
                    // Convert the double array to a byte vector
                    std::vector<char> byteArray(sizeof(double) * arrayLength);
                    std::memcpy(byteArray.data(), doubleArray.data(), sizeof(double) * arrayLength);
                
                    // Store the byte vector in the FbxProperty struct
                    prop.data = byteArray;
                }

                else if (encoding == 1) {

                    // Contents is a deflate/zip-compressed buffer of length CompressedLength bytes
                    std::vector<char> compressedData(compressedLength);
                    file.read(compressedData.data(), compressedLength);
                    _FBX_totalBitsRead += compressedLength;

                    std::vector<double> decompressedData = DecompressZlibDouble(compressedData, sizeof(double) * arrayLength);

                    // Convert the double array to a byte vector
                    std::vector<char> byteArray(sizeof(double) * arrayLength);
                    std::memcpy(byteArray.data(), decompressedData.data(), sizeof(double) * arrayLength);
                
                    // Store the byte vector in the FbxProperty struct
                    prop.data = byteArray;
               
                }
                else {
                    LGDLOG::start<< "ERROR : Reading FBX unknown encoding value : " << encoding << LGDLOG::end;
                }

                break;
            }
            case 'l':
            {
                /*LONG LONG ARRAY */

                uint32_t arrayLength;
                file.read(reinterpret_cast<char*>(&arrayLength), sizeof(uint32_t));
                _FBX_totalBitsRead += sizeof(uint32_t);
    	        
                uint32_t encoding;
                file.read(reinterpret_cast<char*>(&encoding), sizeof(uint32_t));
                _FBX_totalBitsRead += sizeof(uint32_t);
    	        
                uint32_t compressedLength;
                file.read(reinterpret_cast<char*>(&compressedLength), sizeof(uint32_t));
                _FBX_totalBitsRead += sizeof(uint32_t);

                if (encoding == 0) {

                    // Contents is ArrayLength times the array data type
                    std::vector<long long> longlongArray(arrayLength);
                    file.read(reinterpret_cast<char*>(longlongArray.data()), sizeof(long long) * arrayLength);
                    _FBX_totalBitsRead += sizeof(long long) * arrayLength;
                
                    // Convert the long long array to a byte vector
                    std::vector<char> byteArray(sizeof(long long) * arrayLength);
                    std::memcpy(byteArray.data(), longlongArray.data(), sizeof(long long) * arrayLength);
                
                    // Store the byte vector in the FbxProperty struct
                    prop.data = byteArray;
                }

                else if (encoding == 1) {
                    // Contents is a deflate/zip-compressed buffer of length CompressedLength bytes
                    std::vector<char> compressedData(compressedLength);
                    file.read(compressedData.data(), compressedLength);
                    _FBX_totalBitsRead += compressedLength;

                    std::vector<long long> decompressedData = DecompressZlibLongLong(compressedData, sizeof(long long) * arrayLength); 

                    // Convert the long long array to a byte vector
                    std::vector<char> byteArray(sizeof(long long) * arrayLength);
                    std::memcpy(byteArray.data(), decompressedData.data(), sizeof(long long) * arrayLength);
                
                    // Store the byte vector in the FbxProperty struct
                    prop.data = byteArray;            
                }
                else {
                    LGDLOG::start<< "ERROR : Reading FBX unknown encoding value : " << encoding << LGDLOG::end;
                }

                break;
            }
            case 'i':
            {
                /*INT ARRAY*/

                uint32_t arrayLength;
                file.read(reinterpret_cast<char*>(&arrayLength), sizeof(uint32_t));
                _FBX_totalBitsRead += sizeof(uint32_t);
    	        
                uint32_t encoding;
                file.read(reinterpret_cast<char*>(&encoding), sizeof(uint32_t));
                _FBX_totalBitsRead += sizeof(uint32_t);
    	        
                uint32_t compressedLength;
                file.read(reinterpret_cast<char*>(&compressedLength), sizeof(uint32_t));
                _FBX_totalBitsRead += sizeof(uint32_t);

                if (encoding == 0) {

                    // Contents is ArrayLength times the array data type
                    std::vector<int> intArray(arrayLength);
                    file.read(reinterpret_cast<char*>(intArray.data()), sizeof(int) * arrayLength);
                    _FBX_totalBitsRead += sizeof(int) * arrayLength;
                
                    // Convert the int array to a byte vector
                    std::vector<char> byteArray(sizeof(int) * arrayLength);
                    std::memcpy(byteArray.data(), intArray.data(), sizeof(int) * arrayLength);
                
                    // Store the byte vector in the FbxProperty struct
                    prop.data = byteArray;
                }

                else if (encoding == 1) {
                    // Contents is a deflate/zip-compressed buffer of length CompressedLength bytes
                    std::vector<char> compressedData(compressedLength);
                    file.read(compressedData.data(), compressedLength);
                    _FBX_totalBitsRead += compressedLength;

                    std::vector<int> decompressedData = DecompressZlibInt(compressedData, sizeof(int) * arrayLength);       

                    // Convert the int array to a byte vector
                    std::vector<char> byteArray(sizeof(int) * arrayLength);
                    std::memcpy(byteArray.data(), decompressedData.data(), sizeof(int) * arrayLength);
                
                    // Store the byte vector in the FbxProperty struct
                    prop.data = byteArray;         
                }
                else {
                    LGDLOG::start<< "ERROR : Reading FBX unknown encoding value : " << encoding << LGDLOG::end;
                }

                break;
            }
            case 'b':
            {
                /*BOOL ARRAY*/
                /*
                uint32_t arrayLength;
                file.read(reinterpret_cast<char*>(&arrayLength), sizeof(uint32_t));
                _FBX_totalBitsRead += sizeof(uint32_t);
    	        
                uint32_t encoding;
                file.read(reinterpret_cast<char*>(&encoding), sizeof(uint32_t));
                _FBX_totalBitsRead += sizeof(uint32_t);
    	        
                uint32_t compressedLength;
                file.read(reinterpret_cast<char*>(&compressedLength), sizeof(uint32_t));
                _FBX_totalBitsRead += sizeof(uint32_t);

                if (encoding == 0) {

                    // Contents is ArrayLength times the array data type
                    std::vector<bool> boolArray(arrayLength);
                    boolArray.push_back(0);
                    file.read(reinterpret_cast<char*>(&boolArray[0]), sizeof(bool) * arrayLength);
                    _FBX_totalBitsRead += sizeof(bool) * arrayLength;
                
                    //// Convert the bool array to a byte vector
                    std::vector<char> byteArray(sizeof(bool) * arrayLength);
                    std::memcpy(byteArray.data(), &boolArray[0], sizeof(bool) * arrayLength);
                
                    //// Store the byte vector in the FbxProperty struct
                    prop.data = byteArray;
                }//

                else if (encoding == 1) {

                    // Contents is a deflate/zip-compressed buffer of length CompressedLength bytes
                    std::vector<char> compressedData(compressedLength);
                    file.read(compressedData.data(), compressedLength);
                    _FBX_totalBitsRead += compressedLength;

                    prop.data = DecompressZlibChar(compressedData, sizeof(bool) * arrayLength);                
                }
                else {
                    LGDLOG::start<< "ERROR : Reading FBX unknown encoding value : " << encoding << LGDLOG::end;
                }
                */

                LGDLOG::start<< "WARNING! : Reading fbx file : Boolean array type detected. LigidPainter can't handle boolean array types." << LGDLOG::end;
                
                break;
            }


            case 'S':
            case 'R' :
            {
                uint32_t length;
                file.read(reinterpret_cast<char*>(&length), sizeof(uint32_t));
                _FBX_totalBitsRead += sizeof(uint32_t);
                
                
                std::vector<char> charArray(length);
                file.read(reinterpret_cast<char*>(charArray.data()), sizeof(char) * length);
                _FBX_totalBitsRead += sizeof(char) * length;

                prop.data = charArray;

                break;
            }
            
            default:
            {
                //unknown property type
                break;
            }
        }

        if(LIGID_FBX_IMPORTER_PRINT_TEXT_STATE == 2){
            LGDLOG::start<< '-';
            for (size_t i = 0; i < prop.data.size(); i++)
            {
                LGDLOG::start<< prop.data[i];
            
                if(i > 20){
                    LGDLOG::start<< "More...";
                    break;
                }
            }
            LGDLOG::start<< " ( " << prop.data.size() << " ) ";
            LGDLOG::start<< " ( " << prop.typeCode << " ) ";
            LGDLOG::start<< LGDLOG::end;
        }
        
        properties.push_back(prop);
    }
}

/*!
    A Node Structure

    4	        Uint32	EndOffset
    4	        Uint32	NumProperties
    4	        Uint32	PropertyListLen
    1	        Uint8t	NameLen
    NameLen	    char	Name
    ?	        ?	    FbxProperty[n], for n in 0:PropertyListLen
    Optional		
    ?	        ?	    NestedList
    13	        uint8[]	NULL-record
*/

uint32_t __lastEndOffset = 0;

bool ReadNestedNodes(std::ifstream& file, const std::string fileName, std::vector<FbxNode>& nestedNodes) {
        
        FbxNode nestedNode; // Move the declaration inside the while loop

        uint32_t endOffset;
        if(!file.read(reinterpret_cast<char*>(&endOffset), sizeof(uint32_t))){}
        _FBX_totalBitsRead += sizeof(uint32_t);

        if(endOffset)
            __lastEndOffset = endOffset; 
        else{
            std::uintmax_t fileSize = std::filesystem::file_size(fileName);
            //endOffset = 105559;
        }



        uint32_t numProperties;
        file.read(reinterpret_cast<char*>(&numProperties), sizeof(uint32_t));
        _FBX_totalBitsRead += sizeof(uint32_t);
        
        uint32_t propertyListLen;
        file.read(reinterpret_cast<char*>(&propertyListLen), sizeof(uint32_t));
        _FBX_totalBitsRead += sizeof(uint32_t);

        uint8_t nameLen;
        file.read(reinterpret_cast<char*>(&nameLen), sizeof(uint8_t));
        _FBX_totalBitsRead += sizeof(uint8_t);

        // Read nested node name
        nestedNode.nodeType.resize(nameLen);
        file.read(nestedNode.nodeType.data(), nameLen);
        _FBX_totalBitsRead += nameLen;

        if(LIGID_FBX_IMPORTER_PRINT_TEXT_STATE)
            LGDLOG::start<< nestedNode.nodeType << LGDLOG::end;
        // Read nested node properties
        ReadProperties(file, nestedNode.properties, numProperties, propertyListLen);

        // Recursively read nested nodes
        while(endOffset != _FBX_totalBitsRead && endOffset != 0){
        //while(true){
            
            __node_counter++;
            //if(file.eof()){
            //    break; 
            //}
            //else if(__node_counter > 5000)
            //    break;
            
            if(!ReadNestedNodes(file, fileName, nestedNode.nestedNodes)){
                
            }
            
            //if(endOffset == 0)
            //    break; 
            //if(file.eof())
            //    break; 

        }

        //if(endOffset){
        //    file.read(nullRecord, 13 * sizeof(char));
        //    _FBX_totalBitsRead += 13 * sizeof(char);
        //}

        nestedNodes.push_back(nestedNode);
        
        if(__node_counter == 0){
            //Nested node list ended            
        }
        
        __node_counter--;
    
    return true;
}

void ProcessNodeHierarchy( 
                            std::vector<FbxNode>& nodes, 
                            std::vector<std::vector<glm::vec3>>& vertPositions, 
                            std::vector<std::vector<glm::vec2>>& vertUVs, 
                            std::vector<std::vector<glm::vec3>>& vertNormals, 
                            std::vector<std::vector<int>>& polygonVertexIndices, 
                            std::vector<std::vector<int>>& edges, 
                            std::vector<std::vector<int>>& uvIndices,
                            std::vector<std::string>& matTitles,
                            std::vector<std::vector<int>>& materials,
                            std::vector<FBXTransform>& transforms
                        ) 
{
    for ( auto& node : nodes) {
        //LGDLOG::start<< node.nodeType<< LGDLOG::end;

        if (true) {
            
            // Process vertex properties
            for (auto& prop : node.properties) {
                
                if(node.nodeType == "Vertices" && processingMesh){
                    if (prop.typeCode == 'd') {
                        size_t arrayLength = prop.data.size() / sizeof(double);
                        std::vector<double> doubleArray(arrayLength);

                        std::memcpy(doubleArray.data(), prop.data.data(), prop.data.size());

                        for (size_t i = 0; i < doubleArray.size()/3; i++)
                        {
                            glm::vec3 vertPos;
                            if(i * 3 >= doubleArray.size())
                                break;
                            vertPos.x = doubleArray[i * 3];
                            if(i * 3 + 1 >= doubleArray.size())
                                break;
                            vertPos.y = doubleArray[i * 3 + 1];
                            if(i * 3 + 2 >= doubleArray.size())
                                break;
                            vertPos.z = doubleArray[i * 3 + 2];

                            vertPositions[objectI].push_back(vertPos);                            
                        }
                    }
                    else if(prop.typeCode == 'f'){
                        size_t arrayLength = prop.data.size() / sizeof(float);
                        std::vector<float> floatArray(arrayLength);

                        std::memcpy(floatArray.data(), prop.data.data(), prop.data.size());

                        for (size_t i = 0; i < floatArray.size()/3; i++)
                        {
                            glm::vec3 vertPos;
                            if(i * 3 >= floatArray.size())
                                break;
                            vertPos.x = floatArray[i * 3];
                            if(i * 3 + 1 >= floatArray.size())
                                break;
                            vertPos.y = floatArray[i * 3 + 1];
                            if(i * 3 + 2 >= floatArray.size())
                                break;
                            vertPos.z = floatArray[i * 3 + 2];

                            vertPositions[objectI].push_back(vertPos);                            
                        }
                    }
                    else{
                        LGDLOG::start<< "WARNING! Variable type for the vertices array is not valid : " << prop.typeCode << LGDLOG::end;
                    }
                }
                
                if(node.nodeType == "Normals" && processingMesh){
                    if (prop.typeCode == 'd') {
                        size_t arrayLength = prop.data.size() / sizeof(double);
                        std::vector<double> doubleArray(arrayLength);

                        std::memcpy(doubleArray.data(), prop.data.data(), prop.data.size());

                        for (size_t i = 0; i < doubleArray.size()/3; i++)
                        {
                            glm::vec3 vertNormal;
                            if(i * 3 >= doubleArray.size())
                                break;
                            vertNormal.x = doubleArray[i * 3];
                            if(i * 3 + 1 >= doubleArray.size())
                                break;
                            vertNormal.y = doubleArray[i * 3 + 1];
                            if(i * 3 + 2 >= doubleArray.size())
                                break;
                            vertNormal.z = doubleArray[i * 3 + 2];

                            vertNormals[objectI].push_back(vertNormal);                            
                        }
                    }
                    else if(prop.typeCode == 'f'){
                        size_t arrayLength = prop.data.size() / sizeof(float);
                        std::vector<float> floatArray(arrayLength);

                        std::memcpy(floatArray.data(), prop.data.data(), prop.data.size());

                        for (size_t i = 0; i < floatArray.size()/3; i++)
                        {
                            glm::vec3 vertNormal;
                            if(i * 3 >= floatArray.size())
                                break;
                            vertNormal.x = floatArray[i * 3];
                            if(i * 3 + 1 >= floatArray.size())
                                break;
                            vertNormal.y = floatArray[i * 3 + 1];
                            if(i * 3 + 2 >= floatArray.size())
                                break;
                            vertNormal.z = floatArray[i * 3 + 2];

                            vertNormals[objectI].push_back(vertNormal);                            
                        }
                    }
                    else{
                        LGDLOG::start<< "WARNING! Variable type for the normals array is not valid : " << prop.typeCode << LGDLOG::end;
                    }
                }

                if(node.nodeType == "UV" && processingMesh){
                    if (prop.typeCode == 'd') {
                        size_t arrayLength = prop.data.size() / sizeof(double);
                        std::vector<double> doubleArray(arrayLength);

                        std::memcpy(doubleArray.data(), prop.data.data(), prop.data.size());

                        for (size_t i = 0; i < doubleArray.size()/2; i++)
                        {
                            glm::vec3 vertUV;
                            
                            if(i * 2 >= doubleArray.size())
                                break;
                            vertUV.x = doubleArray[i * 2];
                            
                            if(i * 2 + 1 >= doubleArray.size())
                                break;
                            vertUV.y = doubleArray[i * 2 + 1];

                            vertUVs[objectI].push_back(vertUV);                            
                        }
                    }
                    else if (prop.typeCode == 'f') {
                        size_t arrayLength = prop.data.size() / sizeof(float);
                        std::vector<float> floatArray(arrayLength);

                        std::memcpy(floatArray.data(), prop.data.data(), prop.data.size());

                        for (size_t i = 0; i < floatArray.size()/2; i++)
                        {
                            glm::vec3 vertUV;
                            
                            if(i * 2 >= floatArray.size())
                                break;
                            vertUV.x = floatArray[i * 2];
                            
                            if(i * 2 + 1 >= floatArray.size())
                                break;
                            vertUV.y = floatArray[i * 2 + 1];

                            vertUVs[objectI].push_back(vertUV);                            
                        }
                    }
                    else{
                        LGDLOG::start<< "WARNING! Variable type for the texture coordinate array is not valid : " << prop.typeCode << LGDLOG::end;
                    }
                }

                if(node.nodeType == "PolygonVertexIndex" && processingMesh){
                    if (prop.typeCode == 'i') {
                        size_t arrayLength = prop.data.size() / sizeof(int);
                        std::vector<int> intArray(arrayLength);

                        std::memcpy(intArray.data(), prop.data.data(), prop.data.size());

                        for (size_t i = 0; i < intArray.size(); i++)
                        {
                            polygonVertexIndices[objectI].push_back(intArray[i]);                            
                        }
                    }
                }

                if(node.nodeType == "Edges" && processingMesh){
                    if (prop.typeCode == 'i') {
                        size_t arrayLength = prop.data.size() / sizeof(int);
                        std::vector<int> intArray(arrayLength);

                        std::memcpy(intArray.data(), prop.data.data(), prop.data.size());

                        for (size_t i = 0; i < intArray.size(); i++)
                        {
                            edges[objectI].push_back(intArray[i]);                            
                        }
                    }
                }

                if(node.nodeType == "UVIndex" && processingMesh){
                    if (prop.typeCode == 'i') {
                        size_t arrayLength = prop.data.size() / sizeof(int);
                        std::vector<int> intArray(arrayLength);

                        std::memcpy(intArray.data(), prop.data.data(), prop.data.size());

                        for (size_t i = 0; i < intArray.size(); i++)
                        {
                            uvIndices[objectI].push_back(intArray[i]);                            
                        }
                    }
                }

                if(node.nodeType == "Material"){
                    if (prop.typeCode == 'S') {
                        std::string infoStr(prop.data.begin(), prop.data.end());

                        matTitles.push_back(infoStr);
                    }
                }
                
                if(node.nodeType == "Materials" && processingMesh){
                    if (prop.typeCode == 'i') {
                        size_t arrayLength = prop.data.size() / sizeof(int);
                        std::vector<int> intArray(arrayLength);

                        std::memcpy(intArray.data(), prop.data.data(), prop.data.size());
                        materials[objectI] = intArray;                            
                    }
                }

                if(node.nodeType == "P" && processingMesh){
                    if (prop.typeCode == 'S') {
                        std::string infoStr(prop.data.begin(), prop.data.end());

                        if(infoStr.size() > 3){
                            if(modelI < transforms.size())
                                if(infoStr[0] == 'L' && infoStr[1] == 'c' && infoStr[2] == 'l')
                                    transforms[modelI].currentReadMode = UTIL::getLastWordBySeparatingWithChar(infoStr, ' ');
                        }
                    }
                    else if (prop.typeCode == 'D') {
                        double doubleValue = prop.singleDoubleVal;
                        
                        //doubleValue /= 100.;

                        if(modelI < transforms.size()){
                            if(transforms[modelI].currentReadMode == "Translation"){
                                if(transforms[modelI].translation.x == FLT_MAX){
                                    transforms[modelI].translation.x = doubleValue/100.;
                                }
                                else if(transforms[modelI].translation.y == FLT_MAX){
                                    transforms[modelI].translation.y = doubleValue/100.;
                                }
                                else if(transforms[modelI].translation.z == FLT_MAX){
                                    transforms[modelI].translation.z = doubleValue/100.;
                                }
                            }
                            else if(transforms[modelI].currentReadMode == "Rotation"){
                                if(transforms[modelI].rotation.x == FLT_MAX){
                                    transforms[modelI].rotation.x = doubleValue;
                                }
                                else if(transforms[modelI].rotation.y == FLT_MAX){
                                    transforms[modelI].rotation.y = doubleValue;
                                }
                                else if(transforms[modelI].rotation.z == FLT_MAX){
                                    transforms[modelI].rotation.z = doubleValue;
                                }
                            }
                            else if(transforms[modelI].currentReadMode == "Scaling"){
                                if(transforms[modelI].scale.x == FLT_MAX){
                                    transforms[modelI].scale.x = doubleValue / 100.;
                                }
                                else if(transforms[modelI].scale.y == FLT_MAX){
                                    transforms[modelI].scale.y = doubleValue / 100.;
                                }
                                else if(transforms[modelI].scale.z == FLT_MAX){
                                    transforms[modelI].scale.z = doubleValue / 100.;
                                }
                            }
                        }
                        else{
                            LGDLOG::start << "WARNING : Reading FBX file : Transform properties : Invalid model index" << LGDLOG::end;
                        }
                    }
                }

                if(node.nodeType == "MappingInformationType" && processingMesh){
                    if (prop.typeCode == 'S') {
                        std::string infoStr(prop.data.begin(), prop.data.end());
                        if(infoStr != "ByPolygonVertex" && infoStr != "ByPolygon" && infoStr != "AllSame"){
                            LGDLOG::start<< "WARNING : Mapping information type is : " << infoStr << "! Results might be unexpected."  << LGDLOG::end;
                        }
                    }
                }

                if(node.nodeType == "ObjectType" && processingMesh){
                    if (prop.typeCode == 'S') {
                        std::string infoStr(prop.data.begin(), prop.data.end());
                        if(infoStr != "Geometry"){
                            LGDLOG::start<< "WARNING : Object type is  : " << infoStr << "! Results might be unexpected."  << LGDLOG::end;
                        }
                    }
                }

                if(node.nodeType == "Model"){
                    if (prop.typeCode == 'S') {
                        std::string infoStr(prop.data.begin(), prop.data.end());
                        if(infoStr == "Mesh"){
                            transforms.push_back({});
                            modelI++;
                        }
                    }
                }
                
                if(node.nodeType == "Geometry"){
                    if (prop.typeCode == 'S') {
                        std::string infoStr(prop.data.begin(), prop.data.end());
                        if(infoStr == "Mesh"){
                            objectI++;   
                            vertPositions.push_back({}); 
                            vertUVs.push_back({}); 
                            vertNormals.push_back({}); 
                            polygonVertexIndices.push_back({}); 
                            edges.push_back({}); 
                            uvIndices.push_back({});
                            materials.push_back({});
                            processingMesh = true;
                            processingShape = false;
                        }
                        else if(infoStr == "Shape"){
                            processingMesh = false;
                            processingShape = true;
                        }
                    }
                }
            }
        }

        // Recursively process nested nodes
        ProcessNodeHierarchy(node.nestedNodes, vertPositions, vertUVs, vertNormals, polygonVertexIndices, edges, uvIndices, matTitles, materials, transforms);
    }
}

static void parseFBXMeshData(
                        std::vector<glm::vec3>& positions,
                        const std::vector<glm::vec2>& UVs,
                        const std::vector<glm::vec3>& normals,
                        const std::vector<int>& polygonVertexIndices,
                        const std::vector<int>& uvIndices,
                        const std::vector<int>& edges,
                        const std::vector<std::string>& matTitles,
                        const std::vector<int>& materials,
                        std::vector<std::vector<Vertex>>& meshVertices,
                        std::vector<std::vector<unsigned int>>& meshIndices,
                        FBXTransform transform
                    )
{


    // TODO Seperate unique vertices for each mesh

    //Contains the unique vertex values generated
    std::vector<Vertex> uniqueVertices;

    // polygonVertexIndices index, uniqueVertices array index 
    std::vector<std::map<std::pair<int,int>, int>> posData;

    // Give an error if the polygonVertexIndices array & edges array are the same
    /**/
    
    if (polygonVertexIndices.size() != edges.size())
    {
        LGDLOG::start<< "ERROR: Reading FBX file. Can't parse mesh data. Sizes of the polygonVertexIndices & the edges are not the same." << LGDLOG::end;
        return;
    }

    std::cout << "TRANSFORM : " << glm::to_string(transform.rotation) << " " << glm::to_string(transform.scale) << " " << glm::to_string(transform.translation) << std::endl;

    if(transform.translation == glm::vec3(FLT_MAX))
        transform.translation = glm::vec3(0.f);
    if(transform.rotation == glm::vec3(FLT_MAX))
        transform.rotation = glm::vec3(0.f);
    if(transform.scale == glm::vec3(FLT_MAX))
        transform.scale = glm::vec3(1.f);

    glm::vec3 copyTrans = transform.translation; //TODO X
    transform.translation.z = copyTrans.y;
    transform.translation.y = copyTrans.z;

    for (size_t i = 0; i < positions.size(); i++)
    {
        // Create a transformation matrix for translation and rotation
        glm::mat4 modelMatrix = glm::mat4(1.0f); // Identity matrix
        modelMatrix = glm::translate(modelMatrix, transform.translation);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.rotation.z), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(0.f), glm::vec3(1.0f, 0.0f, 0.0f)); //TODO X
        modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.rotation.y), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrix = glm::scale(modelMatrix, transform.scale);

        // Apply the transformation matrix to the vertex
        glm::vec4 transformedVertex = modelMatrix * glm::vec4(positions[i], 1.0f);

        // Store the transformed vertex
        positions[i] = glm::vec3(transformedVertex);
    }

    for (size_t i = 0; i < matTitles.size(); i++)
    {
        posData.push_back({});
        meshVertices.push_back({});
        meshIndices.push_back({});
    }
    // Mapping the indices
    int faceCounter = 0;
    for (size_t i = 0; i < polygonVertexIndices.size(); i++)
    {
        /*
            uvIndices array contains the necessary indices
            for matching the texture coordinates & the vertex
            position data without repeating

            goes like : 0, 1, 2, 3, 5, 8, 9
        */
        // int uvIndex = uvIndices[i];

        // Get the index of the position
        int posIndex = polygonVertexIndices[i];
        if (posIndex < 0){
            posIndex = abs(posIndex) - 1;
        }

        // Get the index of the texture coordinate
        int edgeIndex = edges[i];
        if (edgeIndex  < 0)
            edgeIndex  = abs(edgeIndex) - 1;

        Vertex uniqueVert;
        
        if(posIndex >= positions.size()){
            LGDLOG::start<< "ERROR : Reading fbx file. Position index is not valid." << LGDLOG::end;
            return;
        }
        uniqueVert.Position = positions[posIndex];

        if(edgeIndex >= UVs.size()){
            LGDLOG::start<< "ERROR : Reading fbx file. Texture coordinate index is not valid." << LGDLOG::end;
            return;
        }
        uniqueVert.TexCoords = UVs[edgeIndex];
        
        if(i >= normals.size()){
            LGDLOG::start<< "ERROR : Reading fbx file. Normal vector index is not valid." << LGDLOG::end;
            return;
        }
        uniqueVert.Normal = normals[i];

        int materialI;
        if(materials.size() && materials.size() != 1 && faceCounter < materials.size())
            materialI = materials[faceCounter];
        else if (materials.size() == 1)
            materialI = 0;
        else
            materialI = 0;
        
        posData[materialI][std::make_pair(polygonVertexIndices[i], edges[i])] = meshVertices[materialI].size();
        meshVertices[materialI].push_back(uniqueVert);

        if(polygonVertexIndices[i] < 0)
            faceCounter++;
    }
    

    int faceI = 0;
    faceCounter = 0;
    for (size_t i = 0; i < polygonVertexIndices.size(); i++)
    {
        
        faceI++;
        
        /* Triangulation */
        if (polygonVertexIndices[i] < 0){
            int faceCount = faceI - 2;

            if(!LIGID_FBX_IMPORTER_TRIANGULATE)
                faceCount = 1;

            if(faceCount < 0)
                faceCount = 0;

            int vStartI = i - faceI + 1;
            for (size_t fI = 0; fI < faceCount; fI++)
            {
                glm::ivec3 facePos;
                glm::ivec3 faceEdge;
                
                if(vStartI + 2 + fI >= edges.size()){
                    break;
                }

                facePos.x = polygonVertexIndices[vStartI];
                facePos.y = polygonVertexIndices[vStartI + 1 + fI];
                facePos.z = polygonVertexIndices[vStartI + 2 + fI];
                
                faceEdge.x = edges[vStartI];
                faceEdge.y = edges[vStartI + 1 + fI];
                faceEdge.z = edges[vStartI + 2 + fI];

                int materialI;
                if(materials.size() && materials.size() != 1)
                    materialI = materials[faceCounter];
                else if (materials.size() == 1)
                    materialI = 0;
                else
                    materialI = 0;

                meshIndices[materialI].push_back(posData[materialI][std::make_pair(facePos.x, faceEdge.x)]);
                meshIndices[materialI].push_back(posData[materialI][std::make_pair(facePos.y, faceEdge.y)]);
                meshIndices[materialI].push_back(posData[materialI][std::make_pair(facePos.z, faceEdge.z)]);

                /* Tangent calculations */
                //meshVertices[materialI][posData[materialI][std::make_pair(facePos.x, faceEdge.x)]].Tangent = glm::vec3(0); 
                //meshVertices[materialI][posData[materialI][std::make_pair(facePos.x, faceEdge.x)]].Bitangent = glm::vec3(0); 
                //meshVertices[materialI][posData[materialI][std::make_pair(facePos.y, faceEdge.y)]].Tangent = glm::vec3(0); 
                //meshVertices[materialI][posData[materialI][std::make_pair(facePos.y, faceEdge.y)]].Bitangent = glm::vec3(0); 
                //meshVertices[materialI][posData[materialI][std::make_pair(facePos.z, faceEdge.z)]].Tangent = glm::vec3(0);
                //meshVertices[materialI][posData[materialI][std::make_pair(facePos.z, faceEdge.z)]].Bitangent = glm::vec3(0);
                calculateTangentBitangent(meshVertices[materialI][posData[materialI][std::make_pair(facePos.y, faceEdge.y)]], meshVertices[materialI][posData[materialI][std::make_pair(facePos.z, faceEdge.z)]], meshVertices[materialI][posData[materialI][std::make_pair(facePos.x, faceEdge.x)]]);
                
                /* Normalize tangent values */
                meshVertices[materialI][posData[materialI][std::make_pair(facePos.x, faceEdge.x)]].Tangent = glm::normalize(meshVertices[materialI][posData[materialI][std::make_pair(facePos.x, faceEdge.x)]].Tangent); 
                meshVertices[materialI][posData[materialI][std::make_pair(facePos.x, faceEdge.x)]].Bitangent = glm::normalize(meshVertices[materialI][posData[materialI][std::make_pair(facePos.x, faceEdge.x)]].Bitangent); 
                meshVertices[materialI][posData[materialI][std::make_pair(facePos.y, faceEdge.y)]].Tangent = glm::normalize(meshVertices[materialI][posData[materialI][std::make_pair(facePos.y, faceEdge.y)]].Tangent); 
                meshVertices[materialI][posData[materialI][std::make_pair(facePos.y, faceEdge.y)]].Bitangent = glm::normalize(meshVertices[materialI][posData[materialI][std::make_pair(facePos.y, faceEdge.y)]].Bitangent); 
                meshVertices[materialI][posData[materialI][std::make_pair(facePos.z, faceEdge.z)]].Tangent = glm::normalize(meshVertices[materialI][posData[materialI][std::make_pair(facePos.z, faceEdge.z)]].Tangent);
                meshVertices[materialI][posData[materialI][std::make_pair(facePos.z, faceEdge.z)]].Bitangent = glm::normalize(meshVertices[materialI][posData[materialI][std::make_pair(facePos.z, faceEdge.z)]].Bitangent);
            }
            
            faceI = 0;
            faceCounter++;
        }
    }
}



/*
            SEPERATED

    5 5 9 9 9 7
    Pos :
    vec3(-1.000000, -1.000000, 0.000000) vec3(1.000000, -1.000000, 0.000000) vec3(-1.000000, 1.000000, 0.000000) vec3(-0.208482, -0.552366, 1.007192) vec3(-0.211123, -0.559847, 1.005384)    
    
    UV :
    vec2(0.220108, 0.284449) vec2(0.862011, 0.284449) vec2(0.453018, 0.027691) vec2(0.778371, 0.871778) vec2(0.220108, 0.926352)
    
    Normal :
    vec3(0.000000, 0.000000, 1.000000) vec3(0.000000, 0.000000, 1.000000) vec3(0.000000, 0.000000, 1.000000) vec3(-0.623704, -0.623704, -0.471155) vec3(-0.623704, -0.623704, -0.471155) vec3(-0.623704, -0.623704, -0.471155) vec3(0.000000, 0.916058, -0.401046) vec3(0.000000, 0.916058, -0.401046) vec3(0.000000, 0.916058, -0.401046)
    
    Indices :
    1 2 -1   2 1 -4   1 0 -5
    
    UVIndices :
    0 1 2 4 5 7 8
    
    Edges :
    1 4 0  4 1 3  1 0 2


        UNIT

    4 5 9 9 9 6
    Pos :
    vec3(-1.000000, -1.000000, 0.000000) vec3(1.000000, -1.000000, 0.000000) vec3(-1.000000, 1.000000, 0.000000) vec3(-0.209803, -0.556107, 1.006288)

    UV :
    vec2(0.220108, 0.284449) vec2(0.862011, 0.284449) vec2(0.778371, 0.871778) vec2(0.453018, 0.027691) vec2(0.220108, 0.926352)

    Normal :
    vec3(0.000000, 0.000000, 1.000000) 
    vec3(0.000000, 0.000000, 1.000000) 
    vec3(0.000000, 0.000000, 1.000000) 
    vec3(-0.622656, -0.622656, -0.473918) 
    vec3(-0.622656, -0.622656, -0.473918) 
    vec3(-0.622656, -0.622656, -0.473918) 
    vec3(0.000000, 0.914937, -0.403597) 
    vec3(0.000000, 0.914937, -0.403597) 
    vec3(0.000000, 0.914937, -0.403597)

    Indices :
    1 2 -1   2 1 -4   1 0 -4

    UVIndices :
    0 1 2 4 5 7

    Edges :
    1 4 0   4 1 2   1 0 3

*/







/*
        PLANE FBX
    
    4 4 6 6 6 5
    
    Pos :
    vec3(-1.000000, 0.000000, -1.000000)    vec3(1.000000, 0.000000, -1.000000) vec3(-1.000000, 0.000000, 1.000000) vec3(1.000000, 0.000000, 1.000000)

    UV :
    vec2(1.000000, 0.000000)                vec2(0.000000, 0.000000)            vec2(0.000000, 1.000000)            vec2(1.000000, 1.000000)

    Normal :
    vec3(0.000000, 0.000000, 1.000000)      vec3(0.000000, 0.000000, 1.000000)  vec3(0.000000, 0.000000, 1.000000)  vec3(0.000000, 0.000000, 1.000000)      vec3(0.000000, 0.000000, 1.000000)  vec3(0.000000, 0.000000, 1.000000)

    Indices :
    1 2 0   1 3 2

    UVIndices :
    0 1 2 3 4

    Edges :
    0 2 1 0 3 2

        PLANE OBJ



    Pos :
    vec3(1.000000, 0.000000, 1.000000)  vec3(-1.000000, 0.000000, -1.000000)    vec3(-1.000000, 0.000000, 1.000000) vec3(1.000000, 0.000000, -1.000000)
    
    UV :
    vec2(1.000000, 0.000000)            vec2(0.000000, 1.000000)                vec2(0.000000, 0.000000)            vec2(1.000000, 1.000000)
    
    Normal :
    vec3(0.000000, 1.000000, 0.000000)  vec3(0.000000, 1.000000, 0.000000)      vec3(0.000000, 1.000000, 0.000000)  vec3(0.000000, 1.000000, 0.000000)
    
    Indices :
    0 1 2   0 3 1

*/

/*
    PLANE :
    
    Pos :
    vec3(-1.000000, -1.000000, 0.000000) vec3(1.000000, -1.000000, 0.000000) vec3(-1.000000, 1.000000, 0.000000) vec3(1.000000, 1.000000, 0.000000)

    UV :
    vec2(0.000000, 1.000000) vec2(1.000000, 0.000000) vec2(0.000000, 0.000000) vec2(1.000000, 1.000000)

    Normal :
    vec3(0.000000, 0.000000, 1.000000) vec3(0.000000, 0.000000, 1.000000) vec3(0.000000, 0.000000, 1.000000) vec3(0.000000, 0.000000, 1.000000)

    Indices :
    0 1 3 -3

    UVIndices :
    0 1 2 3

    Edges :
    2 1 3 0


    TRIANGULATED PLANE :

    Pos :
    vec3(-1.000000, -1.000000, 0.000000) vec3(1.000000, -1.000000, 0.000000) vec3(-1.000000, 1.000000, 0.000000) vec3(1.000000, 1.000000, 0.000000)

    UV :
    vec2(1.000000, 0.000000) vec2(0.000000, 0.000000) vec2(0.000000, 1.000000) vec2(1.000000, 1.000000)

    Normal :
    vec3(0.000000, 0.000000, 1.000000) vec3(0.000000, 0.000000, 1.000000) vec3(0.000000, 0.000000, 1.000000) vec3(0.000000, 0.000000, 1.000000) vec3(0.000000, 0.000000, 1.000000) vec3(0.000000, 0.000000, 1.000000)

    Indices :
    1 2 -1 1 3 -3

    UVIndices :
    0 1 2 3 4

    Edges :
    0 2 1 0 3 2
*/