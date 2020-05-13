#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec4 aCol;

out vec3 vCol;
out vec2 vTex;

const int LIGHT_LIMIT = 20;
const float LIGHT_STRENGTH = 0.8;
const vec3 LIGHT_AMBIENT = vec3(0.2,0.2,0.2);

struct Light{
    vec4 pos; //range is found in the .w component
    vec4 col;//.w is used for padding 
};
layout (std140, binding = 0) uniform Lights{
    Light lights[LIGHT_LIMIT];
};

uniform mat4 mvp;

float light_point(vec3 vPos, vec3 vNorm, vec3 lPos, float lRange){
    float prod = max(dot(vNorm, normalize(lPos-vPos)), 0.0);
    float dist = max((lRange-distance(vPos,lPos))/lRange,0.0);
    return dist*prod;
}

void main(){
    gl_Position = mvp*vec4(aPos.x, aPos.y, aPos.z, 1.0);

	//float light = max(dot(aNorm,normalize(vec3(-1.0,-1.0,-1.0))), 0.0)*0.9 + 0.1;
    vec3 lighting = LIGHT_AMBIENT;
    for (int i=0; i<LIGHT_LIMIT; i++){
        Light curLight = lights[i];
        if (curLight.pos.w > 0.0){
            float strength = light_point(aPos,aNorm, curLight.pos.xyz,curLight.pos.w);//TO-DO: use world matrix pos and normal matrix norm
            lighting += strength * curLight.col.rgb * LIGHT_STRENGTH;
        }
    }

	vCol = aCol * lighting;
    vTex = aTex;

    //vCol = aCol;
}