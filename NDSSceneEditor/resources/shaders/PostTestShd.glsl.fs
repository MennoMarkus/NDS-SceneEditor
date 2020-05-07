#version 430 core

in vec2 vTex;

out vec4 FragColor;

uniform sampler2D texProcess;

void main(){
    vec3 col = texture(texProcess, vTex).rgb;

    FragColor = vec4(col * vec3(0.9,0.8,0.7),1.0);
}