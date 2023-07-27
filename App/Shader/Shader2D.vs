#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColour;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec4 Colour;

uniform mat4 Projection;
uniform mat4 Model;

void main()
{
	gl_Position = Projection * Model * vec4(aPos, 1.0);
	Colour = aColour;
	TexCoord = aTexCoord;
}