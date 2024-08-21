#version 330 core

layout(location = 0) in vec4 pos;

uniform vec2 screenSize;
uniform vec2 origin;

void main()
{
	gl_Position = vec4((pos.x + origin.x) / screenSize.x * 2.f - 1.f, 
	-((pos.y + origin.y) / screenSize.y * 2.f - 1.f), 1.f, 1.f);
};