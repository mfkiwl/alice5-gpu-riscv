// The following is a preamble needed on ShaderToy shaders to be
// compiled into the Vulkan environment of SPIR-V.

#version 450

precision highp float;

layout (binding = 0) uniform params {
	ivec2 iResolution;
	float iTime;
	ivec4 iMouse;
}; // Params;

// layout (binding = 1) uniform sampler2D iChannel0;

layout (location = 0) in vec2 fragCoord;

layout (location = 0) out vec4 color;

