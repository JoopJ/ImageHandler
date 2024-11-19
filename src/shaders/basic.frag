#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
	FragColor = texture(texture1, vec2(TexCoord.x, 1.0 - TexCoord.y));	// Flip the texture vertically
}