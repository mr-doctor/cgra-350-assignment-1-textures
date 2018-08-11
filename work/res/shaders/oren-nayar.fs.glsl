#version 330 core

#define PI 3.1415926535897932384626433832795

out vec4 color;

in vec3 fragPosition;
in vec3 fragNormal;

const vec3 lightDir = vec3(0.25, 0.25, -1);

const float roughness = 2.0;

const vec3 view_position = vec3(0.0, 0.0, -1.0);

const vec3 ambient_colour = vec3(0.0, 0.0, 0.9);
const vec3 diffuse_colour = vec3(0.4, 0.4, 1.0);
const vec3 spec_colour    = vec3(0.2, 0.1, 0.1);

const float shininess = 16.0;

float albedo = 3.0;

float A_component(float albedo) {
    float roughness_sq = roughness * roughness;
    float a = 1.0 - (0.5 * (roughness_sq) / (roughness_sq + 0.33));
    float b = 0.17 * albedo * (roughness_sq) / (roughness_sq + 0.13);

    return (a + b) / PI;
}

float B_component(float albedo) {
    float roughness_sq = roughness * roughness;
    float a = 0.45 * (roughness_sq) / (roughness_sq + 0.09);

    return a / PI;
}

float s_component(vec3 light_dir, vec3 normal, vec3 view_position) {
    return dot(light_dir, view_position) - (dot(normal, light_dir) * dot(normal, view_position));
}

float t_component(vec3 light_dir, vec3 normal, float s, vec3 view_position) {
    if (s <= 0.0) {
        return 1.0;
    }

    return max(dot(normal, light_dir), dot(normal, view_position));
}

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 light_incoming = normalize(-lightDir);

    float specular = 0.0;

    float s = s_component(light_incoming, normal, view_position);

    float lambertian = albedo *
    dot(normal, light_incoming) *
        (A_component(albedo) + B_component(albedo) *
        (s / t_component(light_incoming, normal, s, view_position)));

    if (lambertian > 0.0) {

        vec3 viewDir = normalize(-fragPosition);

        vec3 halfDir = normalize(light_incoming + viewDir);
        float specAngle = max(dot(halfDir, normal), 0.0);
        specular = pow(specAngle, shininess);

    }

    vec3 fragColor = ambient_colour +
                     lambertian * diffuse_colour +
                     specular * spec_colour;

    color = vec4(fragColor, 1.0);
}
