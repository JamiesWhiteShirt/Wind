#version 330

uniform mat4 modelview;
uniform mat4 projection;
uniform vec4 const_color;
uniform vec4 cam_pos;
uniform float fog_dist;

layout(location = 0)in vec4 vertex_in;
layout(location = 1)in vec2 tex_coord_in;
layout(location = 2)in vec4 color_in;

out vec4 color;
out vec3 pos_out;

void main()
{
	vec4 vert = vertex_in * modelview * projection;
	gl_Position = vert;
	color = color_in * const_color;
	
	pos_out = vec3(vertex_in * modelview - cam_pos) / fog_dist;
}