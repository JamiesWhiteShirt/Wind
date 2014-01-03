#version 330

uniform mat4 modelview;
uniform mat4 projection;
uniform vec4 const_color;

layout(location = 0)in vec4 vertex_in;
layout(location = 1)in vec2 tex_coord_in;
layout(location = 2)in vec4 color_in;

out vec4 color;

void main()
{
	vec4 vert = vertex_in * modelview * projection;
	gl_Position = vert;
	color = color_in * const_color;
}