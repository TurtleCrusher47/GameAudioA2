#version 330 core

in vec2 TexCoord;
in vec4 Colour;

out vec4 FragColour;

// texture samplers
uniform sampler2D imageTexture;
uniform vec4 runtimeColour;

void main()
{
	FragColour = texture(imageTexture, TexCoord);
	FragColour *= Colour;
	FragColour *= runtimeColour;
}