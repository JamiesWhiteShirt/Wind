#version 330

uniform mat4 modelview;
uniform mat4 projection;
uniform vec4 const_color;

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
	
	float f = 1.0 - (vert.w / 128.0) * (vert.w / 128.0);
	color = vec4(color_in.r * const_color.r * f, color_in.g * const_color.g * f, color_in.b * const_color.b * f, color_in.a * const_color.a);
}