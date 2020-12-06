#version 460 core

layout (location = 0) in vec2 v_pos;
layout (location = 1) in vec2 v_tex_coords;

out vec2 tex_coords;

void main()
{
	tex_coords = v_tex_coords;
	gl_Position = vec4(v_pos, 0.f, 1.f);
}