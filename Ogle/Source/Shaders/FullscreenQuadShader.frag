#version 460 core

layout (location = 0) out vec4 FragColor;

in vec2 tex_coords;

uniform sampler2D u_Framebuffer;

void main()
{
	// Todo: Get viewport dims here
	vec2 texture_coords = vec2(tex_coords.x * (1280.f / 720.f), tex_coords.y);
	FragColor = texture(u_Framebuffer, texture_coords);
}