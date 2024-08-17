#version 330 core

layout(location = 0) in vec4 pos;

uniform vec2 screenSize;

void main()
{
	gl_Position = vec4(pos.x / screenSize.x * 2.f - 1.f, -(pos.y / screenSize.y * 2.f - 1.f), 1.f, 1.f);
};