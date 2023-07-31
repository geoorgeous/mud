#version 450

layout(set = 0, binding = 0) uniform sampler2D textureSampler;

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec2 inTextureCoordinates;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(textureSampler, inTextureCoordinates) * inColor;

    if (outColor.a == 0)
        discard;
}