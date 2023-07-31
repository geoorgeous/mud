#version 450

struct Light_Directional
{
    vec3 direction;
    vec4 color;
};

struct Light_Point
{
    vec4 position;
    vec4 color;
    vec4 attenuation;
};
const int MAX_LIGHTS_POINT = 256;

layout(push_constant) uniform FragmentShaderPushConstants
{
    layout(offset = 128) Light_Directional directionalLight;
} pushConstants;

layout(set = 1, binding = 0) uniform LightsArray_Point
{
    Light_Point lights[MAX_LIGHTS_POINT];
    uint num;
} lights_Point;

layout(set = 2, binding = 0) uniform sampler2D samplerBaseColor;
layout(set = 2, binding = 1) uniform sampler2D samplerNormals;
layout(set = 2, binding = 2) uniform sampler2D samplerMetalness;
layout(set = 2, binding = 3) uniform sampler2D samplerRoughness;

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec2 inTextureCoordinates;
layout(location = 2) in vec3 inWorldPosition;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec3 inViewPosition;

layout(location = 0) out vec4 outColor;

vec3 calculateDirectionalLight(Light_Directional light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // ambient
    vec3 ambient  = light.color.rgb  * texture(samplerBaseColor, inTextureCoordinates).rgb;

    // diffuse shading
    float diffuseStrength = max(dot(normal, lightDir), 0.0);
    vec3 diffuse  = light.color.rgb  * diffuseStrength * texture(samplerBaseColor, inTextureCoordinates).rgb;

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = vec3(0.5) * specularStrength * texture(samplerMetalness, inTextureCoordinates).b;
    
    return ambient + diffuse + specular;
} 

vec3 calculateLighting(Light_Point light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // attenuation
    const float distance = length(light.position.xyz - fragPos);
    const float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * (distance * distance));

    // ambient
    const float ambientStrength = 0.04;
    vec3 ambient = light.color.rgb * ambientStrength * texture(samplerBaseColor, inTextureCoordinates).rgb * attenuation;

    const vec3 lightDir = normalize(light.position.xyz - fragPos);
    
    // diffuse
    const float diffuseStrength = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color.rgb * diffuseStrength * texture(samplerBaseColor, inTextureCoordinates).rgb * attenuation;
    
    // specular
    const vec3 reflectDir = reflect(-lightDir, normal);
    const float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = vec3(0.05) * specularStrength * texture(samplerMetalness, inTextureCoordinates).b * attenuation;

    return ambient + diffuse + specular;
}

void main()
{
    if (texture(samplerBaseColor, inTextureCoordinates).a == 0)
        discard;
        
    vec3 normal = normalize(inNormal);
    vec3 viewDirection = normalize(inViewPosition - inWorldPosition);

    vec4 result = vec4(calculateDirectionalLight(pushConstants.directionalLight, normal, viewDirection), 1.0);

    for (int i = 0; i < lights_Point.num; ++i)
        result += vec4(calculateLighting(lights_Point.lights[i], normal, inWorldPosition, viewDirection), 1.0);

    outColor = result * inColor;

    if (outColor.a == 0)
        discard;
}