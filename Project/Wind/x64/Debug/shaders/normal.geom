#version 150

layout(lines) in;
layout(line_strip, max_vertices = 3) out;

void main()
{
	for(int i = 0; i < gl_in.length; ++i)
	{
		//gl_FrontColor = gl_in[i].gl_FrontColor;
		//gl_TexCoord[0] = gl_in[i].texCoord[0];
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}