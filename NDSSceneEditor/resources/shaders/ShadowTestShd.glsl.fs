#version 430 core

out vec4 FragColor;

float random(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main(){
    float rand = random(gl_FragCoord.xy+gl_FragCoord.z);
    if (rand < 0.6){
        discard;
    }

    FragColor = vec4(1.0,0.0,0.0, 1.0);
}