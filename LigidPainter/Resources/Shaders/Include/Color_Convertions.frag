/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    *This file is included to other shader code using color convertions

    *Include that file using : #pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Color_Convertions.frag)

    And don't forget to disable the squiggles for the GLSL 
    (squiggles becomes annoying since the vscode compiler doesn't recognise including other shader files) 
*/

vec3 rgbToHsv( vec3 rgb)
{
    float r = rgb.r / 255.;
    float g = rgb.g / 255.;
    float b = rgb.b / 255.;

    float cmax = max(max(r, g), b);
    float cmin = min(min(r, g), b);
    float delta = cmax - cmin;

    float hue = 0.;

    if (delta != 0.) {
        if (cmax == r)
            hue = (g - b) / delta;
        else if (cmax == g)
            hue = 2. + (b - r) / delta;
        else if (cmax == b)
            hue = 4. + (r - g) / delta;

        hue *= 60.;
        if (hue < 0.)
            hue += 360.;
    }

    float saturation = (cmax != 0.) ? delta / cmax : 0.;
    float value = cmax * 100.; // Scale the value to the desired range of 0-100

    return vec3(hue, saturation, value);
}

vec3 hsvToRgb( vec3 hsv)
{
    double hh, p, q, t, ff;
    long i;
    vec3 rgb;

    if (hsv.y <= 0.0) {
        rgb.r = hsv.z;
        rgb.g = hsv.z;
        rgb.b = hsv.z;
        return rgb / vec3(100.) * vec3(255.);
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

    return rgb / vec3(100.) * vec3(255.);
}