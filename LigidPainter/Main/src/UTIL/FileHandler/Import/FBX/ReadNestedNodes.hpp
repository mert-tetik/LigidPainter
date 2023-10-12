/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

/* Defines the ReadProperties function */
#include "UTIL/FileHandler/Import/FBX/ReadProperties.hpp"

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
        ReadProperties(file, nestedNode.properties, numProperties, propertyListLen, nestedNode.nodeType);

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