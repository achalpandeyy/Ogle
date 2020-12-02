#version 460 core

layout (location = 0) in vec2 v_pos;

void main()
{
	gl_FragCoord = vec4(v_pos, 0.f, 1.f);
}