#version 330 core
layout(location = 0) in vec3 position; // Position of the vertex
layout(location = 1) in vec3 normal;   // Normal of the vertex

out vec3 texCoord;

uniform mat4 p;
uniform mat4 v;
uniform mat4 m;

void main(void)
{
    mat4 nv = mat4(mat3(v));
    texCoord = position;
    vec4 pos = p * nv * vec4(position * 2, 1.0);
    gl_Position = pos.xyww;
}
