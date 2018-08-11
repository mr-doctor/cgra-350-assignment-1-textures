#version 330 core

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;

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

    gl_Position = proj_mat * view_mat * model_mat * vec4(vertex_pos, 1.0);
    vec4 normal = proj_mat * view_mat * model_mat * vec4(normalize(vertex_pos), 1.0);
    vec4 tangent = proj_mat * view_mat * model_mat * vec4(normalize(vertex_tan), 1.0);
    vec4 bitangent = proj_mat * view_mat * model_mat * vec4(normalize(vertex_bitan), 1.0);
    TBN = transpose(mat3(
        tangent,
        bitangent,
        normal
    ));
    
    vec4 pos = view_mat * model_mat * vec4(vertex_pos, 1.0);
    frag_position = vec3(pos) / pos.w;
    mat3 normal_mat = transpose(inverse(mat3(view_mat * model_mat)));
    frag_normal = normal_mat * vertex_norm;

    mat3 normalMat = transpose(inverse(mat3(view_mat * model_mat)));
    uv = vertex_uv;
}