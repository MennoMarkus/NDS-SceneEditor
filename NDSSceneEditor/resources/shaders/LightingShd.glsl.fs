#version 430 core

in vec3 vCol;
in vec2 vTex;

out vec4 FragColor;

uniform sampler2D texDiffuse;

void main(){
    FragColor = vec4(texture(texDiffuse, vTex).rgb * vCol,1.0);
}