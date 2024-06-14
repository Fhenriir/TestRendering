#version 410

in vec3 vertex_position;
in vec2 uv;
out vec4 out_color;
uniform float alpha;
uniform vec4 color;
uniform sampler2D the_texture;

void main()
{
    out_color = texture(the_texture, uv);
}