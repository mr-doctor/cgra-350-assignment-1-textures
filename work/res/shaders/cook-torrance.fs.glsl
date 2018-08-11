#version 330 core
#define PI 3.1415926535897932384626433832795

out vec4 color;

in vec3 fragPosition;
in vec3 fragNormal;                         // N

const vec3 light = vec3(0.25, 0.25, -1);
const float roughness = 0.1;                // m

const vec3 ambient_colour = vec3(0.0, 0.0, 1.0);
const vec3 diffuse_colour = vec3(0.4, 0.4, 1.0);
const vec3 spec_colour    = vec3(1.0, 1.0, 1.0);

float beckmann(vec3 normal_unit, vec3 half_angle) {
    float alpha = acos(dot(normal_unit, half_angle));

    float numerator = exp((-pow(tan(alpha), 2) / pow(roughness, 2)));
    float denominator = PI * pow(roughness, 2) * pow(cos(alpha), 4);

    return numerator / denominator;
}

float schlick(float angle, float n1, float n2, vec3 normal_unit, vec3 view) {
    float r0 = pow((n1 - n2) / (n1 + n2), 2);
    return r0 + (1 - r0) * (1 - dot(view, normal_unit));
}

float microfacet(vec3 vector, vec3 normal_unit, vec3 view, vec3 half_angle) {
    float numerator = 2 * dot(half_angle, normal_unit) * dot(vector, normal_unit);
    float denominator = dot(view, normal_unit);

    return numerator / denominator;
}

float geo_attenuation(vec3 light_direction, vec3 view, vec3 normal_unit, vec3 half_angle) {
    return min(1, min(microfacet(view, normal_unit, view, half_angle), microfacet(light_direction, normal_unit, view, half_angle)));
}

void main() {

    vec3 normal_unit = normalize(fragNormal);
    vec3 light_direction = normalize(-light);

    vec3 view_direction = normalize(-fragPosition);
    vec3 half_angle = normalize(light_direction + view_direction);

    float specular = 0.0;
    float lambertian = max(dot(light_direction, normal_unit), 0.0);

    if (lambertian > 0.0) {
        specular = (beckmann(normal_unit, half_angle) *
            schlick(dot(normal_unit, view_direction), 1.0, 1.5, normal_unit, view_direction) *
            geo_attenuation(light_direction, view_direction, normal_unit, half_angle))
            / 4 * dot(view_direction, normal_unit) * dot(normal_unit, light_direction);
    }

    vec3 frag_colour = ambient_colour +
        lambertian * diffuse_colour +
        specular * spec_colour;

    color = vec4(frag_colour, 1.0);
}