/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>

#include "UTIL/Util.hpp"

glm::vec3 Color::rgbToHsv( glm::vec3 rgb)
{
    float r = rgb.r / 255.0f;
    float g = rgb.g / 255.0f;
    float b = rgb.b / 255.0f;

    float cmax = glm::max(glm::max(r, g), b);
    float cmin = glm::min(glm::min(r, g), b);
    float delta = cmax - cmin;

    float hue = 0.0f;
    if (delta != 0.0f) {
        if (cmax == r)
            hue = (g - b) / delta;
        else if (cmax == g)
            hue = 2.0f + (b - r) / delta;
        else if (cmax == b)
            hue = 4.0f + (r - g) / delta;

        hue *= 60.0f;
        if (hue < 0.0f)
            hue += 360.0f;
    }

    float saturation = (cmax != 0.0f) ? delta / cmax : 0.0f;
    float value = cmax * 100.0f; // Scale the value to the desired range of 0-100

    return glm::vec3(hue, saturation, value);
}

glm::vec3 Color::hsvToRgb( glm::vec3 hsv)
{
    double hh, p, q, t, ff;
    long i;
    glm::vec3 rgb;

    if (hsv.y <= 0.0) {
        rgb.r = hsv.z;
        rgb.g = hsv.z;
        rgb.b = hsv.z;
        return rgb / glm::vec3(100.f) * glm::vec3(255.f);
    }

    hh = hsv.x;
    if (hh >= 360.0)
        hh = 0.0;
    hh /= 60.0;
    i = static_cast<long>(hh);
    ff = hh - i;
    p = hsv.z * (1.0 - hsv.y);
    q = hsv.z * (1.0 - (hsv.y * ff));
    t = hsv.z * (1.0 - (hsv.y * (1.0 - ff)));

    switch (i) {
        case 0:
            rgb.r = hsv.z;
            rgb.g = t;
            rgb.b = p;
            break;
        case 1:
            rgb.r = q;
            rgb.g = hsv.z;
            rgb.b = p;
            break;
        case 2:
            rgb.r = p;
            rgb.g = hsv.z;
            rgb.b = t;
            break;
        case 3:
            rgb.r = p;
            rgb.g = q;
            rgb.b = hsv.z;
            break;
        case 4:
            rgb.r = t;
            rgb.g = p;
            rgb.b = hsv.z;
            break;
        case 5:
        default:
            rgb.r = hsv.z;
            rgb.g = p;
            rgb.b = q;
            break;
    }

    return rgb / glm::vec3(100.f) * glm::vec3(255.f);
}

glm::vec3 Color::hexToRgb( std::string hex)
{
    // Remove leading '#' if present
    std::string hexValue = hex;
    if (hexValue[0] == '#')
        hexValue.erase(0, 1);
    
    // Parse the hex string into RGB components
    unsigned int r, g, b;
    std::istringstream(hexValue.substr(0, 2)) >> std::hex >> r;
    std::istringstream(hexValue.substr(2, 2)) >> std::hex >> g;
    std::istringstream(hexValue.substr(4, 2)) >> std::hex >> b;
    
    // Convert the RGB components to the range [0, 1]
    glm::vec3 rgb(r, g, b);
    return rgb;
}

std::string Color::rgbToHex(glm::vec3 rgb) {

    // Convert RGB to hexadecimal string
    std::stringstream hexStream;
    hexStream << "#" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(rgb.r)
              << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(rgb.g)
              << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(rgb.b);

    return hexStream.str();
}

glm::vec3 Color::hexToHsv( std::string hex)
{
    glm::vec3 rgb = this->hexToRgb(hex);

    return this->rgbToHsv(rgb);
}

std::string Color::hsvToHex( glm::vec3 hsv)
{
    // Convert HSV to RGB
    glm::vec3 rgb = this->hsvToRgb(hsv);

    return this->rgbToHex(rgb);
}