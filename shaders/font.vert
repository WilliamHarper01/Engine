#version 450

vec2 positions[6] = vec2[](
    vec2(-1.0, -1.0),
    vec2(1.0, 1.0),
    vec2(-1.0, 1.0),

	vec2(-1.0, -1.0),
    vec2(1.0, -1.0),
    vec2(1.0, 1.0)
    
);

vec2 texCoords[6] = vec2[](
    
    vec2(0.0, 0.0),
    vec2(1.0, 1.0),
    vec2(0.0, 1.0),
	
	vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(1.0, 1.0)

);

layout(binding = 0) uniform UniformBufferObject {
    vec2 pos;
	vec2 size;
	float fontSize;
    float glyphWidth;
	vec2 cameraPos;
    vec4 color;

} ubo;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec4 fragColor;

void main() {
    
    vec2 gSize = ubo.size / ubo.fontSize;
    gSize = gSize / ubo.glyphWidth;  
    
    vec2 gPos = gSize * positions[gl_VertexIndex];

    gPos = vec2(
        gPos.x - 1.0 + (gSize.x),
    	gPos.y - 1.0 + (gSize.y)
    );
    gPos = vec2(
        gPos.x + 2*(ubo.pos.x/ubo.glyphWidth),
        gPos.y + 2*(ubo.pos.y/ubo.glyphWidth)
    );

    fragTexCoord = texCoords[gl_VertexIndex];    
    fragColor = ubo.color;
    gl_Position = vec4(gPos, 0.0, 1.0);
}