#version 430 core

out vec4 FragColor;

uniform vec3 col;

void main(){
    FragColor = vec4(col, 1.0);
}