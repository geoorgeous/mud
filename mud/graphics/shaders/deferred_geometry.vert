#version 450

layout(push_constant) uniform VertexShaderPushConstants
{
    mat4 projectionMatrix;
    mat4 viewMatrix;
} pushConstants;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 data;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTextureCooridnates;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec2 outTextureCoordinates;
layout(location = 2) out vec3 outWorldPosition;
layout(location = 3) out vec3 outNormal;
layout(location = 4) out vec3 outViewPosition;

void main()
{
    outColor = inColor * ubo.data[0];
    outTextureCoordinates = inTextureCooridnates;
    outWorldPosition = vec3(ubo.model * vec4(inPosition, 1.0));
    outNormal = mat3(transpose(inverse(ubo.model))) * inNormal;
    outViewPosition = -pushConstants.viewMatrix[3].xyz;

    gl_Position = pushConstants.projectionMatrix * pushConstants.viewMatrix * vec4(outWorldPosition, 1.0);
}