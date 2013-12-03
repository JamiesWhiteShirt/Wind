#version 330

uniform mat4 modelview;
uniform mat4 projection;

layout(location = 0)in vec4 vertex_in;
layout(location = 1)in vec2 tex_coord_in;
layout(location = 2)in vec4 color_in;

out vec2 tex_coord;
out vec4 color;

void main()
{
	vec4 vert = vertex_in * modelview * projection;
	gl_Position = vert;
	tex_coord = tex_coord_in;
	color = color_in * (1.0 - vert.w * 0.01);
}