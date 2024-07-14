#version 410

in vec3 vertex_position;
in vec2 uv;
in vec3 normal;
out vec4 out_color;
uniform float alpha;
uniform vec4 color;
uniform sampler2D the_texture;

void main()
{
    vec4 true_color = texture(the_texture, uv);
    float unlit = 1 - max(max(true_color.r,true_color.g),true_color.b);
    vec4 unlit_color = vec4(true_color.r+unlit,true_color.g+unlit,true_color.b+unlit,true_color.a);
    out_color = vec4(normal,1.0f);
}