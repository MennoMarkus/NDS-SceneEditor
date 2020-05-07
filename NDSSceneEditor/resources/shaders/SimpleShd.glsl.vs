#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec4 aCol;

out vec3 vNorm;
out vec2 vTex;
out vec4 vCol;

uniform mat4 mvp;
uniform vec4 meshCol;

void main()
{
    gl_Position = mvp * vec4(aPos.x, aPos.y, aPos.z, 1.0);

	// Lighting information
	vNorm = aNorm;
    vTex = aTex;
    vCol = meshCol;
}