#version 430 core

in vec3 vCol;

out vec4 FragColor;

void main(){
    FragColor = vec4(vCol, 1.0);
	gl_FragDepth = gl_FragCoord.z-0.0001;
}