#version 410

in vec3 vertex_position;
in vec2 uv;
out vec4 out_color;
uniform float alpha;
uniform float time;
uniform vec4 color;
uniform sampler2D the_texture;

void main()
{
    vec4 true_color = texture(the_texture, uv);
    out_color = true_color;
}