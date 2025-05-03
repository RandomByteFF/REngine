#version 450

layout(location = 0) in vec3 nearPoint;
layout(location = 1) in vec3 farPoint;
layout(location = 2) in mat4 view;
layout(location = 6) in mat4 proj;

layout(location = 0) out vec4 outColor;

vec4 grid(vec3 fragPos, float scale) {
	vec2 coord = fragPos.xz * scale;
	vec2 derivative = fwidth(coord);
	vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
	float line = min(grid.x, grid.y);
	float minimumx = min(derivative.x, 1) * 0.1;
	float minimumy = min(derivative.y, 1) * 0.1;
	vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));
	if (fragPos.x > -minimumx && fragPos.x < minimumx) color.z = 1.0;
	if (fragPos.z > -minimumy && fragPos.z < minimumy) color.x = 1.0;
	return vec4(color);
}

float computeDepth(vec3 pos) {
	vec4 clipSpacePos = proj * view * vec4(pos, 1.0);
	return clipSpacePos.z / clipSpacePos.w;
}

float computeLinearDepth(vec3 pos) {
	vec4 clipSpacePos = proj * view * vec4(pos, 1.0);
	float clipSpaceDepth = (clipSpacePos.z / clipSpacePos.w) * 2.0 - 1.0;
	float near = 0.1;
	float far = 100.0;
	float linearDepth = (2.0 * near * far) / (far + near - clipSpaceDepth * (far - near));
	return linearDepth / far;
}

void main() {
	float t = -nearPoint.y / (farPoint.y - nearPoint.y);
	vec3 fragPos =  nearPoint + t * (farPoint - nearPoint);
	
	float linearDepth = computeLinearDepth(fragPos);
	float fading = max(0, (0.5 - linearDepth));
	gl_FragDepth = computeDepth(fragPos);
	outColor = grid(fragPos, 2) * float(t > 0);
	outColor.a *= fading;
}