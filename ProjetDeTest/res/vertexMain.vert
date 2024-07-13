#version 410

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;
uniform vec2 testVariableUniform;
uniform float aspectRatio;
uniform float time;
uniform mat4 viewMatrix;
out vec3 vertex_position;
out vec2 uv;

void main()
{
    vertex_position = in_position;
    vec3 new_pos = in_position;
    uv = in_uv;
    new_pos[1] = new_pos[1] * aspectRatio;
    gl_Position = viewMatrix * vec4(in_position, 1.);
}