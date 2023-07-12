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

/*
*   0 : Don't print anything to the terminal
*   1 : Print node titles
*   2 : Print node titles with the property data
*/
#define LIGID_FBX_IMPORTER_PRINT_TEXT_STATE 0


// Forward declarations for the utilities
Model createModel(std::vector<std::vector<Vertex>> meshVertices, std::vector<std::vector<unsigned int>> meshIndices, std::vector<std::string> matTitles);
void seperateUnitedVertices(std::vector<std::vector<Vertex>>& unitedVertices, std::vector<std::vector<Vertex>>& meshVertices, std::vector<std::vector<unsigned int>>& meshIndices);
std::vector<std::vector<Vertex>> triangulateFaces(const std::vector<Vertex>& faceData);
void generateTangentBitangent(std::vector<Vertex>& faceData);
std::vector<char> DecompressZlibChar(const std::vector<char>& compressedData, size_t uncompressedSize);
std::vector<float> DecompressZlibFloat(const std::vector<char>& compressedData, size_t numFloats);
std::vector<double> DecompressZlibDouble(const std::vector<char>& compressedData, size_t numDoubles);
std::vector<long long> DecompressZlibLongLong(const std::vector<char>& compressedData, size_t numLongLongs);
std::vector<int> DecompressZlibInt(const std::vector<char>& compressedData, size_t numInts);

// Forward declarations for the fbx file processing functions
void ReadNestedNodes(std::ifstream& file, std::vector<FbxNode>& nestedNodes);
void ProcessNodeHierarchy( std::vector<FbxNode>& nodes, std::vector<glm::vec3>& vertPositions, std::vector<glm::vec2>& vertUVs, std::vector<glm::vec3>& vertNormals, std::vector<int>& polygonVertexIndices, std::vector<int>& edges, std::vector<int>& uvIndices, int depth = 0); 

int _FBX_totalBitsRead = 0;

struct Property {
    char typeCode;
    std::vector<char> data;
};

struct FbxNode {
    std::string nodeType;
    std::vector<Property> properties;
    std::vector<FbxNode> nestedNodes;
};


/*
    HEADER

    Bytes 0 - 20: Kaydara FBX Binary  \x00 (file-magic, with 2 spaces at the end, then a NULL terminator).
    Bytes 21 - 22: [0x1A, 0x00] (unknown but all observed files show these bytes).
    Bytes 23 - 26: unsigned int, the version number. 7300 for version 7.3 for example.
*/
Model FileHandler::readFBXFile(std::string path) {
    _FBX_totalBitsRead = 0;
    __ne_c_cc = 0;

    std::ifstream file(path, std::ios::binary);

    std::cout << path << std::endl;

    if (!file.is_open()) {
        std::cerr << "ERROR : Can't read FBX file. Can't open : " << path << std::endl;
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
        std::cout << "ERROR Reading fbx file : unsupported fbx version : " << version << std::endl;
        return Model();
    }

    // Read top-level object record
    FbxNode topLevelObject;
    ReadNestedNodes(file, topLevelObject.nestedNodes);

    // Read header
    file.read(header, sizeof(header));
    _FBX_totalBitsRead += sizeof(header);
    

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> UVS;
    std::vector<glm::vec3> normals;
    std::vector<std::string> matTitles;
    std::vector<int> polygonVertexIndices;
    std::vector<int> edges;
    std::vector<int> uvIndices;

    // Process the FBX data
    ProcessNodeHierarchy(topLevelObject.nestedNodes, positions, UVS, normals, polygonVertexIndices , uvIndices, edges);


    
    std::vector<std::vector<Vertex>> meshVertices;
    
    std::vector<std::vector<unsigned int>> meshIndices;
    std::vector<std::vector<Vertex>> unitedVertices;
    std::vector<Vertex> _meshVertices;
    std::vector<unsigned int> _meshIndices;


    /* ----------- Stack All The Data ----------- */
    std::vector<Vertex> _unitedVertices;
    std::vector<bool> faceHolder;
    for (size_t i = 0; i < polygonVertexIndices.size(); i++)
    {
        int posIndex = polygonVertexIndices[i];
        int uvIndex = edges[i];
        

        if(posIndex < 0){
            posIndex = abs(posIndex) - 1;
            faceHolder.push_back(1);
        }
        else{
            faceHolder.push_back(0);
        }
        
        if(uvIndex < 0)
            uvIndex = abs(uvIndex) - 1;

        Vertex vert;
        vert.Position = positions[posIndex];  
        vert.TexCoords = UVS[uvIndex];  
        vert.Normal = normals[i];  

        _unitedVertices.push_back(vert);

    }
    
    /* ----------- Triangulate & Generate Tangent For Stacked Data ----------- */
    std::vector<Vertex> triangulatedUnitedVertices;
    std::vector<Vertex> face;
    for (size_t i = 0; i < _unitedVertices.size(); i++)
    {
        face.push_back(_unitedVertices[i]);

        // Face ended
        if(faceHolder[i] == true){
            std::vector<std::vector<Vertex>> triangulatedFaces = triangulateFaces(face);

            for (size_t fI = 0; fI < triangulatedFaces.size(); fI++)
            {
                generateTangentBitangent(triangulatedFaces[fI]);
                for (size_t vI = 0; vI < triangulatedFaces[fI].size(); vI++)
                {
                    triangulatedUnitedVertices.push_back(triangulatedFaces[fI][vI]);
                }
            }

            face.clear();
        }
    }
    

    unitedVertices.push_back(triangulatedUnitedVertices);

    seperateUnitedVertices(unitedVertices, meshVertices, meshIndices);

    // Close the file
    file.close();

    return createModel(meshVertices, meshIndices, {});
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
void ReadProperties(std::ifstream& file, std::vector<Property>& properties, uint32_t numProperties) {
    
// Loop through the properties
    for (int i = 0; i < numProperties; i++) {
        Property prop;


        if(!file.read(&prop.typeCode, sizeof(char)))
            break;

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
                uint8_t boolValue;
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
                prop.data.emplace_back(doubleValue);
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
                
                    // Store the byte vector in the Property struct
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
                
                    // Store the byte vector in the Property struct
                    prop.data = byteArray;
                }
                else {
                    std::cout << "ERROR : Reading FBX unknown encoding value : " << encoding << std::endl;
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
                
                    // Store the byte vector in the Property struct
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
                
                    // Store the byte vector in the Property struct
                    prop.data = byteArray;
               
                }
                else {
                    std::cout << "ERROR : Reading FBX unknown encoding value : " << encoding << std::endl;
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
                
                    // Store the byte vector in the Property struct
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
                
                    // Store the byte vector in the Property struct
                    prop.data = byteArray;            
                }
                else {
                    std::cout << "ERROR : Reading FBX unknown encoding value : " << encoding << std::endl;
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
                
                    // Store the byte vector in the Property struct
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
                
                    // Store the byte vector in the Property struct
                    prop.data = byteArray;         
                }
                else {
                    std::cout << "ERROR : Reading FBX unknown encoding value : " << encoding << std::endl;
                }

                break;
            }
            case 'b':
            {
                /*BOOL ARRAY*/
                
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
                
                    //// Store the byte vector in the Property struct
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
                    std::cout << "ERROR : Reading FBX unknown encoding value : " << encoding << std::endl;
                }
                
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
            std::cout << '-';
            for (size_t i = 0; i < prop.data.size(); i++)
            {
                std::cout << prop.data[i];
            
                if(i > 20){
                    std::cout << "More...";
                    break;
                }
            }
            std::cout << " ( " << prop.data.size() << " ) ";
            std::cout << " ( " << prop.typeCode << " ) ";
            std::cout << std::endl;
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
    ?	        ?	    Property[n], for n in 0:PropertyListLen
    Optional		
    ?	        ?	    NestedList
    13	        uint8[]	NULL-record
*/
int __ne_c_cc = 0;

void ReadNestedNodes(std::ifstream& file, std::vector<FbxNode>& nestedNodes) {
        FbxNode nestedNode; // Move the declaration inside the while loop

        uint32_t endOffset;
        if(!file.read(reinterpret_cast<char*>(&endOffset), sizeof(uint32_t)))
            return;
        _FBX_totalBitsRead += sizeof(uint32_t);

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
            std::cout << nestedNode.nodeType << std::endl;
        
        // Read nested node properties
        ReadProperties(file, nestedNode.properties, numProperties);

        char nullRecord[13];
        
        // Recursively read nested nodes
        while(endOffset != file.tellg().seekpos() + 13){
            __ne_c_cc++;
            ReadNestedNodes(file, nestedNode.nestedNodes);
            if(file.eof())
                break; 
        }

        file.read(nullRecord, sizeof(nullRecord));
        _FBX_totalBitsRead += sizeof(nullRecord);

        nestedNodes.push_back(nestedNode);
        
        if(__ne_c_cc == 0){
            //Nested node list ended            
        }
        
        __ne_c_cc--;
}

void ProcessNodeHierarchy( 
                            std::vector<FbxNode>& nodes, 
                            std::vector<glm::vec3>& vertPositions, 
                            std::vector<glm::vec2>& vertUVs, 
                            std::vector<glm::vec3>& vertNormals, 
                            std::vector<int>& polygonVertexIndices, 
                            std::vector<int>& edges, 
                            std::vector<int>& uvIndices, 
                            int depth = 0
                        ) 
{
    for ( auto& node : nodes) {
        //std::cout << node.nodeType<< std::endl;

        if (true) {
            std::vector<Vertex> vertices;
            
            // Process vertex properties
            for (auto& prop : node.properties) {
                if(node.nodeType == "Vertices"){
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

                            vertPositions.push_back(vertPos);                            
                        }
                    }
                }
                
                if(node.nodeType == "Normals"){
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

                            vertNormals.push_back(vertNormal);                            
                        }
                    }
                }

                if(node.nodeType == "UV"){
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

                            vertUVs.push_back(vertUV);                            
                        }
                    }
                }

                if(node.nodeType == "PolygonVertexIndex"){
                    if (prop.typeCode == 'i') {
                        size_t arrayLength = prop.data.size() / sizeof(int);
                        std::vector<int> intArray(arrayLength);

                        std::memcpy(intArray.data(), prop.data.data(), prop.data.size());

                        for (size_t i = 0; i < intArray.size(); i++)
                        {
                            polygonVertexIndices.push_back(intArray[i]);                            
                        }
                    }
                }

                if(node.nodeType == "Edges"){
                    if (prop.typeCode == 'i') {
                        size_t arrayLength = prop.data.size() / sizeof(int);
                        std::vector<int> intArray(arrayLength);

                        std::memcpy(intArray.data(), prop.data.data(), prop.data.size());

                        for (size_t i = 0; i < intArray.size(); i++)
                        {
                            edges.push_back(intArray[i]);                            
                        }
                    }
                }

                if(node.nodeType == "UVIndex"){
                    if (prop.typeCode == 'i') {
                        size_t arrayLength = prop.data.size() / sizeof(int);
                        std::vector<int> intArray(arrayLength);

                        std::memcpy(intArray.data(), prop.data.data(), prop.data.size());

                        for (size_t i = 0; i < intArray.size(); i++)
                        {
                            uvIndices.push_back(intArray[i]);                            
                        }
                    }
                }

                if(node.nodeType == "MappingInformationType"){
                    if (prop.typeCode == 'S') {
                        std::string infoStr(prop.data.begin(), prop.data.end());
                        if(infoStr != "ByPolygonVertex")
                            std::cout << "WARNING : MappingInformationType is : " << infoStr << "! Results might be unexpected."  << std::endl;
                    }
                }
            }
        }

        // Recursively process nested nodes
        ProcessNodeHierarchy(node.nestedNodes, vertPositions, vertUVs, vertNormals, polygonVertexIndices, edges, uvIndices, depth + 1);
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