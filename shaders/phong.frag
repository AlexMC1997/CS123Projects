#version 330 core

in vec2 texc;
in vec4 cam_pos;
in vec4 cam_norm;
in vec4 world_pos;

out vec4 fragColor;

uniform sampler2D tex;
uniform int useTexture = 0;
uniform vec2 repeatUV;

uniform samplerCube envMap;

// Transformation matrices
uniform mat4 p;
uniform mat4 v;
uniform mat4 m;
uniform mat4 iw;
uniform mat4 iv;
uniform mat4 rot;

// Light data
const int MAX_LIGHTS = 10;
uniform int lightTypes[MAX_LIGHTS];         // 0 for point, 1 for directional
uniform vec3 lightPositions[MAX_LIGHTS];    // For point lights
uniform vec3 lightDirections[MAX_LIGHTS];   // For directional lights
//uniform vec3 lightAttenuations[MAX_LIGHTS]; // Constant, linear, and quadratic term
uniform vec3 lightColors[MAX_LIGHTS];
uniform mat4 lightTransforms[MAX_LIGHTS];

//uniform sampler2DArrayShadow shadowmaps;

// Material data
uniform vec3 ambient_color;
uniform vec3 diffuse_color;
uniform vec3 specular_color;
uniform float shininess;
uniform float roughness;
uniform float metalness;
uniform float transparency;
uniform vec3 ior;
uniform vec3 k2;
uniform vec3 f0;

uniform int shadingType;
uniform int lightingType;

float D_Beckmann(float m, float HN) {
    float r = clamp(m, 0.01, 0.99);
    return max(0.0, 1.0 / (exp(pow(tan(acos(HN)) / r, 2)) * pow(r * pow(HN, 2), 2)));
}

float D_BlinnPhong(float m, float HN) {
    float a2 = pow(clamp(m, 0.01, 0.99), 4);
    return (1 / (3.14159 * a2)) * pow(HN, (2 / a2) - 2);
}

float D_GGX(float m, float LN) {
    return 0.0f;
}

float G_CT(float HN, float VH, float NV, float LN) {
    return min(1, (2 * HN / VH) * min(NV, LN));
}

vec3 F_Schlick(float normView, float metalness, vec3 f0, vec3 ior, vec3 k2) {
    float weight = pow(1 - normView, 5);

    vec3 Fa = mix(vec3(weight), vec3(1), f0);
    vec3 Fb = vec3(
               (pow(ior.x - 1, 2) + 4 * ior.x * weight + k2.x) / (pow(ior.x + 1, 2) + k2.x),
               (pow(ior.y - 1, 2) + 4 * ior.y * weight + k2.y) / (pow(ior.y + 1, 2) + k2.y),
               (pow(ior.z - 1, 2) + 4 * ior.z * weight + k2.z) / (pow(ior.z + 1, 2) + k2.z)
                );
    return mix(Fa, Fb, metalness);
}

void main(){
    vec4 norm = normalize(cam_norm);
    vec4 pos = cam_pos;
    vec4 eyeDirection = normalize(vec4(0,0,0,1) - pos);
    float normView = max(0.0, dot(norm, eyeDirection));
    vec3 color;
    vec3 sampled, sampled2;
    vec3 Ft;
    if (shadingType == 2) {
        color = ambient_color.xyz;
        if (lightingType == 3) {
            vec4 R = iw * reflect(eyeDirection, norm);
            sampled = vec3(texture(envMap, vec3(-R)));
            R = iw * refract(-eyeDirection, norm, 1 / ior.x);
            sampled2.x = vec3(texture(envMap, vec3(R))).x;
            R = iw * refract(-eyeDirection, norm, 1 / ior.y);
            sampled2.y = vec3(texture(envMap, vec3(R))).y;
            R = iw * refract(-eyeDirection, norm, 1 / ior.z);
            sampled2.z = vec3(texture(envMap, vec3(R))).z;

            Ft = F_Schlick(normView, metalness, f0, ior, k2);

            color = mix(ambient_color.xyz, sampled * Ft, shininess);
            color += sampled2 * (1 - Ft) * transparency;
        }

        if (lightingType > 0) {
            for (int i = 0; i < MAX_LIGHTS; i++) {
                vec4 vertexToLight = vec4(0);

                // Point Light
                if (lightTypes[i] == 0) {
                    vertexToLight = normalize(v * vec4(lightPositions[i], 1) - pos);
                } else if (lightTypes[i] == 1) {
                    // Dir Light
                    vertexToLight = normalize(v * vec4(-lightDirections[i], 0));
                }

                if (lightingType < 3) {
                    // Add diffuse component
                    float diffuseIntensity = max(0.0, dot(vertexToLight, norm));
                    color += max(vec3(0), lightColors[i] * diffuse_color * diffuseIntensity);
                }

                vec4 lightReflection = normalize(-reflect(vertexToLight, norm));

                if (lightingType == 2) {
                    //Phong Distribution
                    float specIntensity = pow(max(0.0, dot(eyeDirection, lightReflection)), (1 - roughness) * 64);
                    color += max (vec3(0), lightColors[i] * specIntensity * specular_color);
                }

                if (lightingType == 3) {
//                    if (lightTypes[i] == 1) {
//                        vec4 lpos = lightTransforms[i] * world_pos;
//                        lpos = lpos / lpos.w;
//                        vec3 lpix = (lpos.xyz * 0.5) + 0.5;
//                        color = vec3(texture(shadowmaps, vec4(lpix.xy, i, lpix.z)));
//                        break;
//                    }
                    //Cook-Torrance BRDF
                    vec4 halfVector = normalize(eyeDirection + vertexToLight);
                    float lightNorm = max(0.0, dot(vertexToLight, norm));
                    float halfNorm = max(0.0, dot(halfVector, norm));
                    float viewHalf = max(0.0, dot(eyeDirection, halfVector));

                    float D = D_Beckmann(roughness, halfNorm);

                    float G = G_CT(halfNorm, viewHalf, normView, lightNorm);
                    vec3 specIntensity = (Ft * D * G) / (4 * normView); //C-T BRDF

                    float diffuseIntensity = clamp(lightNorm / 3.14, 0, 1);

                    color += lightColors[i] * max(vec3(0),
                                 mix(
                                     diffuse_color * diffuseIntensity * (1 - metalness),
                                     vec3(1),
                                     specIntensity
                                 )
                             ) * (1 - transparency);
                }
            }
        }
    }

    color = clamp(color, 0.0, 1.0);

    vec3 texColor = texture(tex, texc*repeatUV).rgb;
    texColor = clamp(texColor + vec3(1-useTexture), vec3(0), vec3(1));
    fragColor = vec4(color * texColor, 1);
}
