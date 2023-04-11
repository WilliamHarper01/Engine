#version 450


layout(binding = 0) uniform UniformBufferObject {
    vec2 pos;
	vec2 size;
	float rot;
    float fov;
	vec2 cameraPos;
	vec4 color;

} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec4 fragColor;

void main() {
    
    float hFov = ubo.fov/2.0f;

    vec2 scaledObj = inPosition.xy * ubo.size;
	    scaledObj = vec2(
	    scaledObj.x / hFov,
	    ((scaledObj.y / hFov) * 16.0f) / 9.0f
	);

	vec2 movedObj = vec2(
	    (ubo.pos.x - ubo.cameraPos.x) / hFov,
	    (((ubo.pos.y - ubo.cameraPos.y) / hFov) * 16.0f) / 9.0f
	);

	gl_Position = vec4(movedObj + scaledObj, 0.0, 1.0);
    fragTexCoord = vec2(inTexCoord.x, inTexCoord.y);
	fragColor = ubo.color;

    //gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    
}