#version 330

uniform sampler2D texture_1;

in vec2 tex_coord;
in vec4 color;

out vec4 fragColor;

uniform mat4 color_manipulation;

void main()
{
	fragColor = color * texture2D(texture_1, tex_coord) * color_manipulation;
}