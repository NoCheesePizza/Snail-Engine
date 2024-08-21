#version 330 core

layout(location = 0) out vec4 color;

uniform vec4 fillColor;
uniform int shldUseFillColor;

void main()
{
	if (shldUseFillColor != 0)
		color = fillColor;
};