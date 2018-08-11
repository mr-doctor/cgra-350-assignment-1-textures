#version 330 core

layout(location = 0) in vec3 vertex_position_modelspace;
layout(location = 1) in vec2 vertex_uv;

out vec2 uv;
uniform mat4 mvp_matrix;

void main(){
    gl_Position = mvp_matrix * vec4(vertex_position_modelspace, 1);
    uv = vertex_uv;
}