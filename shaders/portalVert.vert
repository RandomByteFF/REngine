#version 450

layout(push_constant) uniform UniformBufferObject {
	mat4 MVP;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec4 clipPos;

void main() {
	gl_Position = ubo.MVP * vec4(inPosition, 1.0);
	clipPos = gl_Position;
}