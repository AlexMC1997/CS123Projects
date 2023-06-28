#version 330
#extension GL_EXT_geometry_shader4 : enable

layout(triangles) in;

layout(triangle_strip, max_vertices = 3) out;

in vec4 cam_norm[];
in vec4 cam_pos[];

out vec3 tri_color;

// Transformation matrices
uniform mat4 p;
uniform mat4 v;
uniform mat4 m;

// Light data
const int MAX_LIGHTS = 10;
uniform int lightTypes[MAX_LIGHTS];         // 0 for point, 1 for directional
uniform vec3 lightPositions[MAX_LIGHTS];    // For point lights
uniform vec3 lightDirections[MAX_LIGHTS];   // For directional lights
//uniform vec3 lightAttenuations[MAX_LIGHTS]; // Constant, linear, and quadratic term
uniform vec3 lightColors[MAX_LIGHTS];

// Material data
uniform vec3 ambient_color;
uniform vec3 diffuse_color;
uniform vec3 specular_color;
uniform vec3 ior;
uniform vec3 k2;
uniform vec3 f0;
uniform float shininess;
uniform float roughness;
uniform float metalness;
uniform float transparency;
uniform vec2 repeatUV;

uniform int shadingType;
uniform int lightingType;
uniform bool useArrowOffsets; // True if rendering the arrowhead of a normal for Shapes

void main() {
    vec4 tri_norm = normalize(cam_norm[0] + cam_norm[1] + cam_norm[2]);
    vec4 tri_pos = (cam_pos[0] + cam_pos[1] + cam_pos[2]) / 3;

    if (shadingType == 0) {
        tri_color = ambient_color.xyz; // Add ambient component

        if (lightingType > 0) {
            for (int i = 0; i < MAX_LIGHTS; i++) {
                vec4 vertexToLight = vec4(0);
                // Point Light
                if (lightTypes[i] == 0) {
                    vertexToLight = normalize(v * vec4(lightPositions[i], 1) - tri_pos);
                } else if (lightTypes[i] == 1) {
                    // Dir Light
                    vertexToLight = normalize(v * vec4(-lightDirections[i], 0));
                }

                // Add diffuse component
                float diffuseIntensity = max(0.0, dot(vertexToLight, tri_norm));
                tri_color += max(vec3(0), lightColors[i] * diffuse_color * diffuseIntensity);

                if (lightingType == 2) {
                    // Add specular component
                    vec4 lightReflection = normalize(-reflect(vertexToLight, tri_norm));
                    vec4 eyeDirection = normalize(vec4(0,0,0,1) - tri_pos);
                    float specIntensity = pow(max(0.0, dot(eyeDirection, lightReflection)), (1 - roughness) * 64);
                    tri_color += max (vec3(0), lightColors[i] * specular_color * specIntensity);
                }
            }
        }
    }

    tri_color = clamp(tri_color, 0.0, 1.0);
    gl_Position = gl_PositionIn[0];
    EmitVertex();
    gl_Position = gl_PositionIn[1];
    EmitVertex();
    gl_Position = gl_PositionIn[2];
    EmitVertex();
    EndPrimitive();
}
