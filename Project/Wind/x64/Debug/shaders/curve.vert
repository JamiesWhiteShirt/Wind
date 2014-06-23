#version 330

#define M_PI 3.1415926535897932384626433832795

uniform mat4 modelview;
uniform mat4 projection;
uniform vec4 const_color;
uniform vec4 cam_pos;
uniform float fog_dist;

layout(location = 0)in vec4 vertex_in;
layout(location = 1)in vec2 tex_coord_in;
layout(location = 2)in vec4 color_in;

out vec2 tex_coord;
out vec4 color;
out vec3 pos_out;

void main()
{
	const float curveCenter = -20000.0;
	
	vec3 vertToCurve = (vertex_in * modelview).xyz;
	vec3 distance = vec3(cam_pos.x - vertToCurve.x, curveCenter - vertToCurve.y, cam_pos.z - vertToCurve.z);

	float radian = length(distance.xz) / length(distance) * M_PI;

	vec4 curvedVert = vec4(vertToCurve.x * cos(radian), vertToCurve.y - curveCenter * (cos(radian) - 1.0), vertToCurve.z * cos(radian), 1.0);
	
	gl_Position = curvedVert * projection;
	tex_coord = tex_coord_in;
	
	color = color_in * const_color;
	
	pos_out = vec3(curvedVert - cam_pos) / fog_dist;
}