#version 330 core
layout (location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;

uniform mat4 lTransform;
uniform mat4 m;
uniform mat4 iw;

void main(void)
{
    gl_Position = lTransform * m * vec4(pos, 1.0);
}
