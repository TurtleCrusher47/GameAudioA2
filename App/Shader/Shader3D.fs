#version 330 core
out vec4 FragColour;

in vec2 TexCoord;
in vec4 Colour;

// texture samplers
uniform sampler2D imageTexture;

void main()
{
	FragColour = texture(imageTexture, TexCoord);
	FragColour *= Colour;
}
