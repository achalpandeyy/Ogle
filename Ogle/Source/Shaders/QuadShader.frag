#version 460 core

layout (location = 0) out vec4 FragColor;

in vec2 tex_coords;

uniform sampler2D u_Framebuffer;

void main()
{
	// FragColor = vec4(0.8f, 0.3f, 0.2f, 1.f);
	FragColor = texture(u_Framebuffer, tex_coords);
}