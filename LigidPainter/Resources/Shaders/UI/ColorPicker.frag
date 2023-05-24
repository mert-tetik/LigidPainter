#version 330 core

#define PI 3.1416
#define FEATHER 0.01

uniform vec2 resolution;

//HSV to RGB 色相落在不同的区间
vec3 hsv2rgb_(in vec3 hsvColor){
    hsvColor=max(hsvColor,0.1);
    float H=hsvColor.x;
    H=mod(H,360.0);
    float C=hsvColor.b*hsvColor.g;
    float X=C*(1.0-abs(mod(H/60.0,2.0)-1.0));
    float m=hsvColor.b-C;
    vec3 rgb;
    if(0.0<=H && H<=60.0)
    {
        rgb=vec3(C,X,0.0);
    }else if(60.0<H && H<=120.0)
    {
        rgb=vec3(X,C,0.0);
    }else if(120.0<H && H<=180.0)
    {
        rgb=vec3(0.0,C,X);
    }else if(180.0<H && H<=240.0)
    {
        rgb=vec3(0.0,X,C);
    }else if(240.0<H && H<=300.0)
    {
        rgb=vec3(X,0.0,C);
    }else if(300.0<H && H<=360.0)
    {
        rgb=vec3(C,0.0,X);
    }
    else
    {
        return vec3(1.0);
    }
    return rgb+m;
    
}
//RGB to HSV Reference：http://mattlockyer.github.io/iat455/documents/rgb-hsv.pdf
//与三原色哪个更接近，进行相应的偏移
vec3 rgb2hsv(in vec3 rgbColor)
{
    float H_scale=60.0;//最多能偏+-60度
    float Cmax=max(max(rgbColor.x,rgbColor.y),rgbColor.z);//最亮的通道
    float Cmin=min(min(rgbColor.x,rgbColor.y),rgbColor.z);//最暗的通道
    float delta=Cmax-Cmin;//最大偏向，若值大，颜色饱和
    float H=0.0;
    float S=0.0;
    if(delta!=0.0)//注意分母不能为0
    {
        if(Cmax==rgbColor.r)//最接近红色
       {
           //另外两个分量取差值
           //分母代表了红色的权重，分子代表偏绿色的趋势有多少，这个值可正可负，负偏向蓝色，正偏向绿色
           H=(rgbColor.g-rgbColor.b)/delta;
       }else if (Cmax==rgbColor.g)//最接近绿色
       {
           H=(rgbColor.b-rgbColor.r)/delta+2.0;//偏蓝色的趋势有多少
       }else if(Cmax==rgbColor.b)//最接近蓝色
       {
           H=(rgbColor.r-rgbColor.g)/delta+4.0;//偏红色的趋势有多少
       }
    }
    
    if(Cmax!=0.0)
    {
        S=delta/Cmax;//饱和度
    }
    
    H*=H_scale;
    return vec3(H,S,Cmax);
}
//性能更高 https://zhuanlan.zhihu.com/p/61520731
vec3 hsv2rgb(in vec3 c)
{
  c.x/=360.0;//归化到1
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx,0.0,1.0), c.y);
}
//计算三角形三个顶点的权重Reference https://codeplea.com/triangular-interpolation
vec3 calcTriangleWeight(in vec2 v1,in vec2 v2,in vec2 v3,in vec2 p)
{
    vec3 weight;
    weight.x=((v2.y-v3.y)*(p.x-v3.x)+(v3.x-v2.x)*(p.y-v3.y)) / ((v2.y-v3.y)*(v1.x-v3.x)+(v3.x-v2.x)*(v1.y-v3.y));
    weight.y=((v3.y-v1.y)*(p.x-v3.x)+(v1.x-v3.x)*(p.y-v3.y)) / ((v2.y-v3.y)*(v1.x-v3.x)+(v3.x-v2.x)*(v1.y-v3.y));
    weight.z=1.0 - weight.x- weight.y;
    return weight;
}

//QQ3977270
//使用权重插值颜色并可进行三角形排除，
vec4 lerpWithWeight(in vec3 weight,in vec3 sourceColor1,in vec3 sourceColor2,in vec3 sourceColor3)
    
{
    float alpha=min(min(weight.x,weight.y),weight.z);//如果权重小于0顶点不在三角形内
    alpha=smoothstep(-FEATHER*0.5,FEATHER*0.5,alpha);
    return vec4(weight.x*sourceColor1+weight.y*sourceColor2+weight.z*sourceColor3,alpha);
}
//绘制一标记，一个小圆点
vec4 drawPoint(in vec2 uv,in vec2 p)
{
    float alpha=smoothstep(0.008,0.002,abs(length(uv-p)-0.02));
    return vec4(vec3(1.0),alpha);
}

//通过三角形内的点计算出对应的sv值
vec2 position2sv(in vec2 p,in vec2 v1,in vec2 v2,in vec2 v3)
{
    //v1是色相所指的点，逆时针依次为v2,v3
    vec2 baseS=v1-v3;
    vec2 baseV=baseS*0.5-(v2-v3);
    vec2 baseO=v3-baseV;
         p-=baseO;
    float s=dot(p,baseS)/pow(length(baseS),2.0);
    float v=dot(p,baseV)/pow(length(baseV),2.0);
    s-=0.5;
    s/=v;
    s+=0.5;
    return vec2(s,v);
}
//三角形转换函数通过sv值计算出在三角形内的位置
vec2 hsv2Position(in vec3 hsvColor,in vec2 v1,in vec2 v2,in vec2 v3)
{
    //
    vec2 baseS=v1-v3;
    vec2 baseV=baseS*0.5-(v2-v3);
    vec2 baseO=v3-baseV;
    float S=hsvColor.g;
    float V=hsvColor.b;
    S-=0.5;
    S*=V;
    S+=0.5;
    vec2 ret=baseO+baseS*S+baseV*V;
    return ret;
    
}
//通过外截圆半径和旋转角度（角度即色相）算三角形的顶点
void calcTriangle(in float Raidus,in float degree,inout vec2 v1,inout vec2 v2,inout vec2 v3)
{
    float r=PI/180.0;
    //每间隔120度一个顶点
    float radian=degree*r;
    v1.x=cos(radian)*Raidus;
    v1.y=sin(radian)*Raidus;
    radian+=120.0*r;
    v2.x=cos(radian)*Raidus;
    v2.y=sin(radian)*Raidus;
    radian+=120.0*r;
    v3.x=cos(radian)*Raidus;
    v3.y=sin(radian)*Raidus;
}
//绘制线段
vec4 drawLine(in vec2 uv,in vec2 p1,in vec2 p2,in vec4 color,in float thickness)
{
    thickness*=0.5;
    vec2 lineDirection=p2-p1;
    float lineLength=length(lineDirection);
    lineDirection=normalize(lineDirection);
    vec2 toUv=uv-p1;
    float projectLength=dot(lineDirection,toUv);
    float p2LineDistance=length(toUv-lineDirection*projectLength);
    p2LineDistance=smoothstep(thickness+FEATHER,thickness,p2LineDistance);
    float p2EndDistance=projectLength<=0.0? abs(projectLength):projectLength-lineLength;
    //float rr=projectLength<=0.0? 0.0:1.0;
    //float rr=floor(projectLength/abs(floor(projectLength)))+1.0;
    //float p2EndDistance=abs(projectLength)-lineLength*rr;
    //修改颜色加黑边
    p2EndDistance=smoothstep(thickness,thickness-FEATHER*0.5,p2EndDistance);
    color.xyz*=mix(p2LineDistance,0.0,00.01);
    //应用透明度
    color.a*=min(p2LineDistance,p2EndDistance);
    return color;
}
//绘制色环
vec4 drawColorRing(in vec2 uv,in float exteriorRaidus,in float interiorRaidus){
  //绘制色环
    //float radian=uv.y>0.0? atan(uv.y,uv.x): 2.0*PI+atan(uv.y,uv.x);//弧度
    float xiangxian=floor(1.0-uv.y );
    float radian=xiangxian*2.0*PI+atan(uv.y,uv.x);
    
    float degree=radian/(2.0*PI)*360.0;//角度
    vec4 ringColor=vec4(hsv2rgb(vec3(degree,1.0,1.0)),0.0);//色环颜色
    float psitionRaidus=length(uv);//当前点到圆心的距离
    //float s=psitionRaidus/exteriorRaidus;//饱和度
    //色环的透明度
    float interiorMask=smoothstep(interiorRaidus,interiorRaidus+FEATHER,psitionRaidus);//内
    float outeriorMask=smoothstep(exteriorRaidus,exteriorRaidus-FEATHER,psitionRaidus);//外
    ringColor.a=min(interiorMask,outeriorMask);  
    return ringColor;
}

//绘制三角形,提供三个点位以及色相
vec4 drawTriangle(in vec2 uv,in vec2 v1,in vec2 v2,in vec2 v3,in float H)
{
   vec4 triangleColor;
   triangleColor.xyz=hsv2rgb(vec3(H,position2sv(uv,v1,v2,v3)));
   vec3 weight=calcTriangleWeight(v1,v2,v3,uv);
   triangleColor.a=min(min(weight.x,weight.y),weight.z);//如果权重小于0顶点不在三角形内
   triangleColor.a=smoothstep(-FEATHER*0.5,FEATHER*0.5,triangleColor.a);
   return triangleColor;
    
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // 规格化坐标系为0-1,原点在中心
	fragCoord-=resolution.xy*vec2(0.5);
    vec2 ndc=fragCoord/(resolution.y*0.5);
    //鼠标位置
    vec2 mousePosition = iMouse.xy-resolution.xy*vec2(0.5);
    mousePosition/=(resolution.y*0.5);
    
    //公共参数
    float exteriorRaidus=1.0;//环外半径
    float thickness=0.2;//环厚度
    float interiorRaidus=exteriorRaidus-thickness;//环内半径
 
    
    //vec3 sampleColor=vec3(H,sin(iTime*0.5)*0.5+0.5,1.0);
    vec3 sampleColor=rgb2hsv(vec3(0.0,.6,0.6));//当前颜色
    //色相由鼠标确定,也是角度
    sampleColor.x=mod(atan(mousePosition.y,mousePosition.x)/PI*180.0,360.0);//改色相

    
    ///////////////////////////////////////////////////////////////////////
    //开始绘制三角
    //等边三角形顶点
    vec2 p1;
    vec2 p3;
    vec2 p2;
    calcTriangle(interiorRaidus,sampleColor.x,p1,p2,p3);//填充顶点数据
   
    ////绘制三解形
    vec4 triangleColor=drawTriangle(ndc,p1,p2,p3,sampleColor.x);
 
    //绘制色环
    vec4 ringColor=drawColorRing(ndc,exteriorRaidus,interiorRaidus);
    
    //绘制色环刻度 
    vec4 lineColor=drawLine(ndc,p1+normalize(p1)*FEATHER*0.5,p1+normalize(p1)*(thickness-FEATHER*0.5),vec4(1.0,1.0,1.0,0.5),0.02);
    
    //绘制颜色位置标记
    vec4 marker=drawPoint(ndc,hsv2Position(sampleColor,p1,p2,p3));
    ///////////////////////////////////////////////////////////////////////////////
    //混合最终输出
    fragColor=vec4(0.15,0.15,0.15,1.0);//背景
    fragColor=fragColor*(1.0-ringColor.a)+ringColor*ringColor.a;//色环
    fragColor=fragColor*(1.0-triangleColor.a)+triangleColor*triangleColor.a;//三角形
    fragColor=fragColor*(1.0-lineColor.a)+lineColor*lineColor.a;//色环刻度 
    fragColor=fragColor*(1.0-marker.a)+marker*marker.a;//颜色位置标记
   
}
