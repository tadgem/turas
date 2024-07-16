#version 450

#define MAX_NUM_EACH_LIGHTS 32

#define ATTENUATION_CONSTANT 1.0
#define ATTENUATION_LINEAR_CONSTANT 4.5
#define ATTENUATION_QUADRATIC_CONSTANT 75.0

struct DirectionalLight
{
    vec4 Direction;
    vec4 Ambient;
    vec4 Colour;
};

struct PointLight
{
    vec4 PositionRadius;
    vec4 Ambient;
    vec4 Colour;
};

struct SpotLight
{
    vec4 PositionRadius;
    vec4 DirectionAngle;
    vec4 Ambient;
    vec4 Colour;
};

layout(location = 0) in vec2 a_FragUV;
layout(location = 0) out vec4 a_PixelColor;

layout(push_constant, std430) uniform PUSH_CONSTANTS {
    mat4 pc_Model;
    mat4 pc_View;
    mat4 pc_Proj;
};

layout(binding = 0) uniform sampler2D u_PositionBufferSampler;
layout(binding = 1) uniform sampler2D u_NormalBufferSampler;
layout(binding = 2) uniform sampler2D u_ColourBufferSampler;

layout(binding = 3) uniform UniformLightObject{
    DirectionalLight    u_DirectionalLight;
    PointLight          u_PointLights[MAX_NUM_EACH_LIGHTS];
    SpotLight           u_SpotLights[MAX_NUM_EACH_LIGHTS];

    uint                u_DirLightActive;
    uint                u_PointLightsActive;
    uint                u_SpotLightsActive;
} u_LightData;


float BlinnPhong_Attenuation(float range, float dist)
{
    float l     = ATTENUATION_LINEAR_CONSTANT / range;
    float q     = ATTENUATION_QUADRATIC_CONSTANT / range;

    return 1.0 / (ATTENUATION_CONSTANT + l * dist + q * (dist * dist));
}

vec3 BlinnPhong_Directional(vec3 normal, vec3 materialAmbient, vec3 materialDiffuse, vec3 materialSpecular, float shininess)
{
    vec3 lightDir = normalize(-u_LightData.u_DirectionalLight.Direction.xyz);
    float diff = max(dot(normal, lightDir), 0.0);
    float s = pow(max(dot(normal, lightDir), 0.0), shininess);

    vec3    ambient     = u_LightData.u_DirectionalLight.Ambient.xyz * materialAmbient;
    vec3    diffuse     = u_LightData.u_DirectionalLight.Colour.xyz * diff * materialDiffuse;
	vec3    spec        = u_LightData.u_DirectionalLight.Colour.xyz * s * materialSpecular;

    return (ambient + diffuse + spec);
}

vec3 BlinnPhong_Point(int lightIdx, vec3 position, vec3 normal, vec3 materialAmbient, vec3 materialDiffuse, vec3 materialSpecular, float shininess)
{
    vec3    ambient    = u_LightData.u_PointLights[lightIdx].Ambient.xyz * materialAmbient;
    vec3    plDir      = u_LightData.u_PointLights[lightIdx].PositionRadius.xyz - position;
    vec3    s          = normalize(plDir);
    float   sDotN      = max(dot(s, normal), 0.0);
    vec3    diffuse    = materialDiffuse * sDotN;
    vec3    spec       = vec3(0.0, 0.0, 0.0);
    
    if(sDotN > 0.0)
    {
        vec3 v = normalize(-position);
        vec3 h = normalize(v + s);
        spec =  materialSpecular * pow(max(dot(h, normal), 0.0), shininess);
    }
    float range     = u_LightData.u_PointLights[lightIdx].PositionRadius.w;

    float attenuation = BlinnPhong_Attenuation(range, length(plDir));

    ambient     *= attenuation;
    diffuse     *= attenuation;
    spec        *= attenuation;

    return (ambient + u_LightData.u_PointLights[lightIdx].Colour.xyz * (diffuse + spec));
}

vec3 BlinnPhong_Spot(int lightIdx, vec3 position, vec3 normal, vec3 materialAmbient, vec3 materialDiffuse, vec3 materialSpecular, float shininess)
{
    vec3    slDir      = u_LightData.u_SpotLights[lightIdx].PositionRadius.xyz - position;
    vec3    s          = normalize(slDir);

    float cosAngle     = dot(-s, normalize(u_LightData.u_SpotLights[lightIdx].DirectionAngle.xyz));
    float angle        = acos(cosAngle);
    float   theta      = dot(slDir, normalize(-u_LightData.u_SpotLights[lightIdx].DirectionAngle.xyz ));

    if(angle < u_LightData.u_SpotLights[lightIdx].DirectionAngle.w)
    {
        vec3 ambient    = u_LightData.u_SpotLights[lightIdx].Ambient.xyz * materialAmbient;

        vec3    norm    = normalize(normal);
        float   diff    = max(dot(norm, slDir), 0.0);
        vec3    diffuse = u_LightData.u_SpotLights[lightIdx].Colour.xyz * diff * materialDiffuse; 
        
        vec3    viewPos = vec3(pc_View[3][0], pc_View[3][1], pc_View[3][2]);
        vec3    viewDir = normalize(viewPos - position);
        vec3    reflectDir = reflect(-slDir, norm);  
        float   spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        vec3    specular = u_LightData.u_SpotLights[lightIdx].Colour.xyz * spec * materialSpecular;

        float   range     = u_LightData.u_SpotLights[lightIdx].PositionRadius.w;
        float   attenuation = BlinnPhong_Attenuation(range, length(slDir));

        diffuse     *= attenuation;
        specular    *= attenuation;

        return (ambient + diffuse + specular);

    }
    else
    {
        return u_LightData.u_SpotLights[lightIdx].Ambient.xyz * materialAmbient;
    }
}

void main() {

    vec4 textureColour = vec4(texture(u_ColourBufferSampler, a_FragUV).rgb, 1.0f);
    vec3 position = texture(u_PositionBufferSampler, a_FragUV).xyz;
    vec3 normal = texture(u_NormalBufferSampler, a_FragUV).xyz;

    vec3 lightColour = BlinnPhong_Directional(normal, vec3(0.0f), textureColour.xyz, vec3(0.0f), 0.0f);

    for(int i = 0; i < MAX_NUM_EACH_LIGHTS; i++)
    {
        lightColour += BlinnPhong_Point(i, position, normal, vec3(0.0f), textureColour.xyz, vec3(0.0f), 0.0f);
        lightColour += BlinnPhong_Spot(i, position, normal, vec3(0.0f), textureColour.xyz, vec3(0.0f), 0.0f);
    }

    a_PixelColor = vec4(lightColour, 1.0);
}