#version 330 core

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projectionMat;

in vec3 vertex_pos;
in vec3 vertex_norm;
in vec2 vertex_uv;
in vec3 vertex_tan;
in vec3 vertex_bitan;

out vec2 uv;
out vec3 frag_position;
out vec3 frag_normal;
out mat3 TBN;

void main(){

    gl_Position = projectionMat * viewMat * modelMat * vec4(vertex_pos, 1.0);
    vec4 normal = projectionMat * viewMat * modelMat * vec4(normalize(vertex_pos), 1.0);
    vec4 tangent = projectionMat * viewMat * modelMat * vec4(normalize(vertex_tan), 1.0);
    vec4 bitangent = projectionMat * viewMat * modelMat * vec4(normalize(vertex_bitan), 1.0);
    TBN = transpose(mat3(
        tangent,
        bitangent,
        normal
    ));

    vec4 pos = viewMat * modelMat * vec4(vertex_pos, 1.0);
    frag_position = vec3(pos) / pos.w;

    mat3 normal_mat = transpose(inverse(mat3(viewMat * modelMat)));
    frag_normal = normal_mat * vertex_norm;

    uv = vertex_uv;
}