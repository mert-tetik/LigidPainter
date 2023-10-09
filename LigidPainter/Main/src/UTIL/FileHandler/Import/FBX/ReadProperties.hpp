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