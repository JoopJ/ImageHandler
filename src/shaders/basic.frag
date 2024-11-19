#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform bool flipped;

void main()
{
// OpenGL expects the 0.0 coordinate on the y-axis to be on the bottom 
// side of the image, but images usually have 0.0 at the top of the y-axis.

// So when we want to flip an image:
	if (flipped)
		FragColor = texture(texture1, TexCoord);
// If we don't want to flip the image:
	else
		FragColor = texture(texture1, vec2(TexCoord.x, 1.0 - TexCoord.y));
}