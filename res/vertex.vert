#version 410

layout(location = 0) in vec2 in_position;

void main()
{
    vec2 new_pos = in_position + 0.4;
    gl_Position = vec4(new_pos, 0., 1.);
}