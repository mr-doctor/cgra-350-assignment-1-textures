#version 330 core

// Interpolated values from the vertex shaders
in vec2 uv;
in vec3 frag_position;
in vec3 frag_normal;
in mat3 TBN;

// Ouput data
out vec3 colour;

// Values that stay constant for the whole mesh.
uniform sampler2D BaseMap;
uniform sampler2D NormalMap;

const vec3 light_direction = vec3(0.25, 0.25, -1);

const vec3 ambient_colour = vec3(0.05, 0.05, 0.1);
const vec3 diffuse_colour = vec3(0.4, 0.4, 1.0);
const vec3 spec_colour    = vec3(0.2, 0.1, 0.1);

const float shininess = 16.0;

void main() {

    vec3 normal = normalize(frag_normal);
    vec3 light_norm = normalize(-light_direction);

    float lambertian = max(dot(light_norm, normal), 0.0);
    float specular = 0.0;

    if (lambertian > 0.0) {
        vec3 view_dir = normalize(-frag_position);

        vec3 half_dir = normalize(light_norm + view_dir);
        float spec_angle = max(dot(half_dir, normal), 0.0);

        specular = pow(spec_angle, shininess);
    }

    colour = texture(BaseMap, uv).rgb +
        lambertian * diffuse_colour +
        specular * spec_colour;

}