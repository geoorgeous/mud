#version 450

layout(push_constant) uniform PushConstants
{
    mat4 projectionMatrix;
    mat4 viewMatrix;
} pushConstants;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTextureCooridnates;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec2 outTextureCoordinates;

void main()
{
    gl_Position = pushConstants.projectionMatrix * pushConstants.viewMatrix * vec4(inPosition, 1.0);
    outColor = inColor;
    outTextureCoordinates = inTextureCooridnates;
}