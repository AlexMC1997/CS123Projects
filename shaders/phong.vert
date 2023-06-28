#version 330 core

layout(location = 0) in vec3 position; // Position of the vertex
layout(location = 1) in vec3 normal;   // Normal of the vertex
layout(location = 5) in vec2 texCoord; // UV texture coordinates
layout(location = 10) in float arrowOffset; // Sideways offset for billboarded normal arrows


out vec4 cam_pos;
out vec4 cam_norm;
out vec4 world_pos;
out vec2 texc;

// Transformation matrices
uniform mat4 p;
uniform mat4 v;
uniform mat4 m;

uniform vec2 repeatUV;
uniform bool useArrowOffsets; // True if rendering the arrowhead of a normal for Shapes

void main() {
    texc = texCoord * repeatUV;

    vec4 position_cameraSpace = v * m * vec4(position, 1.0);
    vec4 normal_cameraSpace = vec4(normalize(mat3(transpose(inverse(v * m))) * normal), 0);
    cam_pos = position_cameraSpace;
    cam_norm = normal_cameraSpace;

    vec4 position_worldSpace = m * vec4(position, 1.0);
    world_pos = position_worldSpace;
    vec4 normal_worldSpace = vec4(normalize(mat3(transpose(inverse(m))) * normal), 0);

    if (useArrowOffsets) {
        // Figure out the axis to use in order for the triangle to be billboarded correctly
        vec3 offsetAxis = normalize(cross(vec3(position_cameraSpace), vec3(normal_cameraSpace)));
        position_cameraSpace += arrowOffset * vec4(offsetAxis, 0);
    }

    gl_Position = p * position_cameraSpace;
}
