#version 450

layout(binding = 0) uniform VP {
	mat4 view;
	mat4 proj;
} vp;

layout(location = 0) out vec3 nearPoint;
layout(location = 1) out vec3 farPoint;
layout(location = 2) out mat4 view;
layout(location = 6) out mat4 proj;

vec3 gridPlane[6] = vec3[](
    vec3(1, 1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(-1, -1, 0), vec3(1, 1, 0), vec3(1, -1, 0)
);

vec3 unprojectPoint(vec3 point, mat4 view, mat4 proj) {
	vec4 unprojected = inverse(view) * inverse(proj) * vec4(point, 1.0);
	return unprojected.xyz / unprojected.w;
}

void main() {
	vec3 p = gridPlane[gl_VertexIndex].xyz;
	nearPoint = unprojectPoint(vec3(p.xy, 0.0), vp.view, vp.proj);
	farPoint = unprojectPoint(vec3(p.xy, 1.0), vp.view, vp.proj);
	view = vp.view;
	proj = vp.proj;
	gl_Position = vec4(p.xy, 0.99999, 1.0);
}	