#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 clipPos;

layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main() {
	vec2 uv = clipPos.xy / clipPos.w * 0.5 + 0.5;
	outColor = texture(texSampler, uv);
	// outColor = vec4(uv, 0., 1.);
}