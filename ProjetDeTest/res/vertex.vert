#version 410

layout(location = 0) in vec3 in_position;
uniform vec2 testVariableUniform;
uniform float aspectRatio;
uniform float time;
uniform mat4 viewMatrix;

void main()
{
    vec3 new_pos = in_position;
    new_pos[1] = new_pos[1] * aspectRatio;
    gl_Position = viewMatrix * vec4(in_position, 1.);
}