#version 450

layout(binding = 0) uniform VP {
	mat4 view;
	mat4 proj;
} vp;

vec3 gridPlane[6] = vec3[](
    vec3(1, 1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(-1, -1, 0), vec3(1, 1, 0), vec3(1, -1, 0)
);

void main() {
	vec3 p = gridPlane[gl_VertexIndex].xyz;
	gl_Position = vec4(p.xy, 0.0, 1.0);
}