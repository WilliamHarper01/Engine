#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragPos;
layout(location = 3) in vec4 fragColor;

layout(location = 0) out vec4 outColor;


void main() {
    vec3 ambientStrength = vec3(0.1, 0.1, 0.1);
    vec3 lightPos = vec3(-5.0, 0.0, 5.0);
    vec3 lightColor = vec3(0.5, 0.5, 0.5);
    vec3 viewPos = vec3(-5.0f, 0.0f, 1.5f);
    float specularStrength = 0.5f;

    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm); 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
    vec3 specular = specularStrength * spec * lightColor;

    outColor = texture(texSampler, fragTexCoord) * vec4(ambientStrength + diffuse + specular, 1.0);
}