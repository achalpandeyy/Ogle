#version 460 core

layout (location = 0) out vec4 FragColor;

in vec2 tex_coords;

uniform sampler2D u_Framebuffer;

void main()
{
	FragColor = texture(u_Framebuffer, tex_coords);
}