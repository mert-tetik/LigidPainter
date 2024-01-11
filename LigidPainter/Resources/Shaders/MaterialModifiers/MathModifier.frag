/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

YOU SHOULD KNOW THAT A SOUL CAN'T BE CUT
NO MATTER WHAT BLADE YOU WIELD
IF YOU FOLLOW YOUR OWN PATH TO JUSTICE
THE BRUTAL TRUTH WILL BE REVEALED
THE SOUND'S GETTING LOUDER
AND LOUDER AND LOUDER
AND LOUDER AND LOUDER
YOU SHOULD KNOW

*/

#version 400 core

out vec4 fragColor;

in vec2 TexCoords;

/*
    0 : Add 
    1 : Subtract 
    2 : Multiply 
    3 : Divide 
    4 : Power 
    5 : Logarithm 
    6 : Min 
    7 : Max 
    8 : Sqrt 
    9 : Exponent 
    10 : Sin 
    11 : Cos 
    12 : Tan 
    13 : Cross 
    14 : Dot 
    15 : Distance 

*/
uniform int operationIndex;
uniform int useRightSideTxtr;
uniform float rightSideVal;
uniform sampler2D rightSideTxtr;

uniform sampler2D mask;
uniform sampler2D previousTxtr;
uniform float opacity;

void main(){
    
    fragColor.a = 1.;

    vec3 lval = texture(previousTxtr, TexCoords).rgb;
    vec3 rval = vec3(rightSideVal);
    if(useRightSideTxtr == 1)
        rval = texture(rightSideTxtr, TexCoords).rgb;
        
    if(operationIndex == 0)
        fragColor.rgb = lval + rval;
    if(operationIndex == 1)
        fragColor.rgb = lval - rval;
    if(operationIndex == 2)
        fragColor.rgb = lval * rval;
    if(operationIndex == 3)
        fragColor.rgb = lval / rval;
    if(operationIndex == 4)
        fragColor.rgb = pow(lval, rval);
    if(operationIndex == 5)
        fragColor.rgb = log(lval);
    if(operationIndex == 6)
        fragColor.rgb = min(lval, rval);
    if(operationIndex == 7)
        fragColor.rgb = max(lval, rval);
    if(operationIndex == 8)
        fragColor.rgb = sqrt(lval);
    if(operationIndex == 9)
        fragColor.rgb = exp(lval);
    if(operationIndex == 10)
        fragColor.rgb = sin(lval);
    if(operationIndex == 11)
        fragColor.rgb = cos(lval);
    if(operationIndex == 12)
        fragColor.rgb = tan(lval);
    if(operationIndex == 13)
        fragColor.rgb = cross(lval, rval);
    if(operationIndex == 14)
        fragColor.rgb = vec3(dot(lval, rval));
    if(operationIndex == 15)
        fragColor.rgb = vec3(distance(lval, rval));
    
    float alpha = opacity;
    alpha *= texture(mask, TexCoords).r; 

    fragColor.rgb = mix(lval, fragColor.rgb, alpha);
}