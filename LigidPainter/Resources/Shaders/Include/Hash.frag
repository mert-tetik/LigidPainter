/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    *This file is included to other shader code using hash functions

    *Include that file using : #pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Hash.frag)

    And don't forget to disable the squiggles for the GLSL 
    (squiggles becomes annoying since the vscode compiler doesn't recognise including other shader files) 
*/


//----------------------- FLOAT ----------------------- 

/*
*   Returns a random float value in the range of 0 - 1 
*/
float hash_normalized_float(float x)
{
    return fract(
                    sin(x) * 43758.5453
                );
}



//----------------------- VEC2 -----------------------

/*
*   Returns a random vec2 value in the range of 0 - 1 using a vec2 value 
*/
vec2 hash_normalized_vec2(vec2 p)
{
    return vec2(
                    hash_normalized_float(p.x),
                    hash_normalized_float(p.y)
                );
}

/*
*   Returns a random vec2 value in the range of 0 - 1 using a float value 
*/
vec2 hash_normalized_vec2(float p)
{
    return vec2(
                    hash_normalized_float(p),
                    hash_normalized_float(p + 0.1)
                );
}



//----------------------- VEC3 -----------------------

/*
*   Returns a random vec3 value in the range of 0 - 1 using a vec3 value 
*/
vec3 hash_normalized_vec3(vec3 p)
{
    return vec3(
                    hash_normalized_float(p.x),
                    hash_normalized_float(p.y),
                    hash_normalized_float(p.z)
                );
}

/*
*   Returns a random vec3 value in the range of 0 - 1 using a vec2 value 
*/
vec3 hash_normalized_vec3(vec2 p)
{
    return vec3(
                    hash_normalized_float(p.x),
                    hash_normalized_float(p.y),
                    hash_normalized_float(p.y + 0.1)
                );
}

/*
*   Returns a random vec3 value in the range of 0 - 1 using a float value 
*/
vec3 hash_normalized_vec3(float p)
{
    return vec3(
                    hash_normalized_float(p),
                    hash_normalized_float(p + 0.1),
                    hash_normalized_float(p + 0.2)
                );
}



//----------------------- VEC3 -----------------------

/*
*   Returns a random vec4 value in the range of 0 - 1 using a vec4 value 
*/
vec4 hash_normalized_vec4(vec4 p)
{
    return vec4(
                    hash_normalized_float(p.x),
                    hash_normalized_float(p.y),
                    hash_normalized_float(p.z),
                    hash_normalized_float(p.w)
                );
}

/*
*   Returns a random vec4 value in the range of 0 - 1 using a vec3 value 
*/
vec4 hash_normalized_vec4(vec3 p)
{
    return vec4(
                    hash_normalized_float(p.x),
                    hash_normalized_float(p.y),
                    hash_normalized_float(p.z + 0.1),
                    hash_normalized_float(p.z)
                );
}

/*
*   Returns a random vec4 value in the range of 0 - 1 using a vec2 value 
*/
vec4 hash_normalized_vec4(vec2 p)
{
    return vec4(
                    hash_normalized_float(p.x),
                    hash_normalized_float(p.y),
                    hash_normalized_float(p.x + 0.1),
                    hash_normalized_float(p.y + 0.1)
                );
}

/*
*   Returns a random vec4 value in the range of 0 - 1 using a float value 
*/
vec4 hash_normalized_vec4(float p)
{
    return vec4(
                    hash_normalized_float(p),
                    hash_normalized_float(p + 0.1),
                    hash_normalized_float(p + 0.2),
                    hash_normalized_float(p + 0.3)
                );
}