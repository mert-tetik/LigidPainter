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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <zlib/zlib.h>

#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"

std::vector<char> ZIPPER::decompressZlibChar(const std::vector<char>& compressedData, size_t uncompressedSize) {
    
    std::vector<char> uncompressedData(uncompressedSize, 0);
    
    z_stream stream;
    stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressedData.data()));
    stream.avail_in = static_cast<uInt>(compressedData.size());
    stream.next_out = reinterpret_cast<Bytef*>(uncompressedData.data());
    stream.avail_out = static_cast<uInt>(uncompressedData.size());

    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    if (inflateInit(&stream) != Z_OK) {
        std::cerr << "Failed to initialize zlib for decompression." << LGDLOG::end;
        return {};
    }

    // Decompress the data
    int result = inflate(&stream, Z_FINISH);
    inflateEnd(&stream);

    if (result != Z_STREAM_END) {
        std::cerr << "Failed to decompress zlib data. Error code: " << result << LGDLOG::end;
        return {};
    }

    return uncompressedData;
}

std::vector<float> ZIPPER::decompressZlibFloat(const std::vector<char>& compressedData, size_t numFloats) {
    std::vector<float> uncompressedFloats(numFloats);

    z_stream stream;
    stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressedData.data()));
    stream.avail_in = static_cast<uInt>(compressedData.size());
    stream.next_out = reinterpret_cast<Bytef*>(uncompressedFloats.data());
    stream.avail_out = static_cast<uInt>(uncompressedFloats.size() * sizeof(float));

    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    if (inflateInit(&stream) != Z_OK) {
        std::cerr << "Failed to initialize zlib for decompression." << LGDLOG::end;
        return {};
    }

    // Decompress the data
    int result = inflate(&stream, Z_FINISH);
    inflateEnd(&stream);

    if (result != Z_STREAM_END) {
        std::cerr << "Failed to decompress zlib data. Error code: " << result << LGDLOG::end;
        return {};
    }

    // Calculate the actual number of floats decompressed
    size_t numDecompressedFloats = stream.total_out / sizeof(float);
    uncompressedFloats.resize(numDecompressedFloats);

    return uncompressedFloats;
}

std::vector<double> ZIPPER::decompressZlibDouble(const std::vector<char>& compressedData, size_t numDoubles) {
    std::vector<double> uncompressedDoubles(numDoubles);

    z_stream stream;
    stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressedData.data()));
    stream.avail_in = static_cast<uInt>(compressedData.size());
    stream.next_out = reinterpret_cast<Bytef*>(uncompressedDoubles.data());
    stream.avail_out = static_cast<uInt>(uncompressedDoubles.size() * sizeof(double));

    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    if (inflateInit(&stream) != Z_OK) {
        std::cerr << "Failed to initialize zlib for decompression." << LGDLOG::end;
        return {};
    }

    // Decompress the data
    int result = inflate(&stream, Z_FINISH);
    inflateEnd(&stream);

    if (result != Z_STREAM_END) {
        std::cerr << "Failed to decompress zlib data. Error code: " << result << LGDLOG::end;
        return {};
    }

    // Calculate the actual number of doubles decompressed
    size_t numDecompressedDoubles = stream.total_out / sizeof(double);
    uncompressedDoubles.resize(numDecompressedDoubles);

    return uncompressedDoubles;
}

std::vector<long long> ZIPPER::decompressZlibLongLong(const std::vector<char>& compressedData, size_t numLongLongs) {
    std::vector<long long> uncompressedLongLongs(numLongLongs);

    z_stream stream;
    stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressedData.data()));
    stream.avail_in = static_cast<uInt>(compressedData.size());
    stream.next_out = reinterpret_cast<Bytef*>(uncompressedLongLongs.data());
    stream.avail_out = static_cast<uInt>(uncompressedLongLongs.size() * sizeof(long long));

    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    if (inflateInit(&stream) != Z_OK) {
        std::cerr << "Failed to initialize zlib for decompression." << LGDLOG::end;
        return {};
    }

    // Decompress the data
    int result = inflate(&stream, Z_FINISH);
    inflateEnd(&stream);

    if (result != Z_STREAM_END) {
        std::cerr << "Failed to decompress zlib data. Error code: " << result << LGDLOG::end;
        return {};
    }

    // Calculate the actual number of long longs decompressed
    size_t numDecompressedLongLongs = stream.total_out / sizeof(long long);
    uncompressedLongLongs.resize(numDecompressedLongLongs);

    return uncompressedLongLongs;
}

std::vector<int> ZIPPER::decompressZlibInt(const std::vector<char>& compressedData, size_t numInts) {
    std::vector<int> uncompressedInts(numInts);

    z_stream stream;
    stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressedData.data()));
    stream.avail_in = static_cast<uInt>(compressedData.size());
    stream.next_out = reinterpret_cast<Bytef*>(uncompressedInts.data());
    stream.avail_out = static_cast<uInt>(uncompressedInts.size() * sizeof(int));

    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    if (inflateInit(&stream) != Z_OK) {
        std::cerr << "Failed to initialize zlib for decompression." << LGDLOG::end;
        return {};
    }

    // Decompress the data
    int result = inflate(&stream, Z_FINISH);
    inflateEnd(&stream);

    if (result != Z_STREAM_END) {
        std::cerr << "Failed to decompress zlib data. Error code: " << result << LGDLOG::end;
        return {};
    }

    // Calculate the actual number of ints decompressed
    size_t numDecompressedInts = stream.total_out / sizeof(int);
    uncompressedInts.resize(numDecompressedInts);

    return uncompressedInts;
}