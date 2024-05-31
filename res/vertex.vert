#version 410

layout(location = 0) in vec2 in_position;
uniform vec2 testVariableUniform;
uniform float aspectRatio;
uniform float time;

void main()
{
    vec2 new_pos = in_position;
    new_pos[1] = new_pos[1] * aspectRatio;
    new_pos[1] += sin(time/2)/2;
    new_pos[0] += cos(time/2)/2;
    gl_Position = vec4(new_pos, 0., 1.);
}