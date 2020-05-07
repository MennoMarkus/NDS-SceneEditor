#version 430 core

in vec3 vNorm;
in vec2 vTex;
in vec4 vCol;

out vec4 FragColor;

uniform sampler2D texDiffuse;
uniform sampler2D texLightmap;
uniform vec4 colMult;
uniform float mult;

void main()
{
    //vec4 lightmap = texture(texLightmap, vTex)+vec4(0.4,0.4,0.4,0.0);
    
    FragColor = mix(vCol, texture(texDiffuse, vTex), 0.9); //* lightmap * vec4(vCol,1.0) * colMult * mult;
}