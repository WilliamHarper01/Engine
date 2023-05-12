#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec2 pos;
	vec2 size;
	float rot;
    float fov;
	vec2 cameraPos;
	vec4 color;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragPos;
layout(location = 3) out vec4 fragColor;

void main() {
    vec4 toClip = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    vec3 toDevice = vec3(toClip.x/toClip.w, toClip.y/toClip.w, toClip.z/toClip.w);

    float hFov = ubo.fov/2.0f;

    vec2 scaledObj = toDevice.xy * ubo.size;
	    scaledObj = vec2(
	    scaledObj.x / hFov,
	    scaledObj.y / hFov
	);

	vec2 movedObj = vec2(
	    (ubo.pos.x - ubo.cameraPos.x) / hFov,
	    (((ubo.pos.y - ubo.cameraPos.y) / hFov) * 16.0f) / 9.0f
	);

	toDevice.xy = movedObj + scaledObj;

    gl_Position = vec4(toDevice, 1.0);
    //gl_Position = vec4(inPosition, 1.0);
    fragTexCoord = inTexCoord;
    fragNormal = mat3(transpose(inverse(ubo.model))) * inNormal;
    fragPos = vec3(ubo.model * vec4(inPosition, 1.0));
    fragColor = ubo.color;
}