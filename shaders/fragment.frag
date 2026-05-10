#version 460
#extension GL_EXT_nonuniform_qualifier : enable

layout(push_constant) uniform PC {
	mat4 MVP;
	uint textureIndex;
};

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(set = 0, binding = 0) uniform sampler2D textures[];

layout(location = 0) out vec4 outColor;

void main() {
	outColor = texture(textures[textureIndex], fragTexCoord);
}