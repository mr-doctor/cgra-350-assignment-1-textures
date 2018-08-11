#version 330 core

in vec2 uv;

out vec3 colour;

uniform sampler2D texture_sampler;

void main() {
    colour = texture(texture_sampler, uv).rgb;
}