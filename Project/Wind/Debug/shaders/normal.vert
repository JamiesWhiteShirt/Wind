#version 330

uniform mat4 modelview;
uniform mat4 projection;
uniform mat4 color_manipulation;

layout(location = 0)in vec4 vertex_in;
layout(location = 1)in vec2 tex_coord_in;
layout(location = 2)in vec4 color_in;

out vec2 tex_coord;
out vec4 color;

void main()
{
	gl_Position = (vertex_in * modelview * projection) / vertex_in.w;
	tex_coord = tex_coord_in;
	color = color_manipulation * color_in;
}