#version 330

uniform vec4 fog_color;

in vec4 color;
in vec3 pos_out;

out vec4 fragColor;

void main()
{
	float f = dot(pos_out, pos_out);
	if(f > 1.0)
	{
		discard;
	}
	fragColor = mix(color, fog_color, f);
}