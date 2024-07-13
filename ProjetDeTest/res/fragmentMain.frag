#version 410

in vec3 vertex_position;
in vec2 uv;
out vec4 out_color;
uniform float alpha;
uniform vec4 color;
uniform sampler2D the_texture;

void main()
{
    vec4 true_color = texture(the_texture, uv);
    float luminosity = true_color.r * 0.299 + true_color.g * 0.587 + true_color.b * 0.114;
    vec4 final_color = vec4(luminosity, 0f, 0.f, alpha);
    out_color = final_color;
}