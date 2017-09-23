#version 420 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

out vec4 fragColor;

struct Material
{
    vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Attenuation
{
    float constant;
	float linear;
	float quadratic;
};

struct Cutoff
{
    float cutoff;
	float outCutoff;
};

struct DirectionalLight
{
    vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
    vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	//the attenuation factor
	Attenuation atten;
};

struct SpotLight
{
    vec3 position;
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	//the attenuation factor
	Attenuation atten;

	//cutoff angle
	Cutoff co;
};

const int PointLightNum = 1;
const int SpotLightNum = 1;

uniform Material material;

layout(std140, binding = 1) uniform Lights
{
    DirectionalLight dirLight;
    PointLight pointLights[PointLightNum];
    SpotLight torch[SpotLightNum];
};

uniform vec3 viewPos;

float diff(vec3 lightDir, vec3 normal);
float spec(vec3 lightDir, vec3 normal, vec3 viewDir, float shininess);

vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 viewPos);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewPos);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewPos);

void main()
{
    vec3 result;
    result += calcDirLight(dirLight, Normal, viewPos);
	for(int i = 0; i < PointLightNum; ++i)
	    result += calcPointLight(pointLights[i], Normal, FragPos, viewPos);
	for(int i = 0; i < SpotLightNum; ++i)
	    result += calcSpotLight(torch[i], Normal, FragPos, viewPos);

    fragColor = vec4(result, 1.0f);
}

float diff(vec3 lightDir, vec3 normal)
{
	return max(dot(lightDir, normal), 0.0f);
}

float spec(vec3 lightDir, vec3 normal, vec3 viewDir, float shininess)
{
	vec3 rd = reflect(-lightDir, normal);
	return pow(max(dot(rd, viewDir), 0.0f), shininess);
}

vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 viewPos)
{
    vec3 ld = normalize(-light.direction);
	vec3 n = normalize(normal);
	vec3 vd = normalize(viewPos - FragPos);
    vec3 ambient = light.ambient * material.ambient;
	vec3 diffuse = light.diffuse * diff(ld, n) * material.diffuse;
	vec3 specular = light.specular * spec(ld, n, vd, material.shininess) * material.specular;

	return ambient + diffuse + specular;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewPos)
{
    vec3 ld = normalize(light.position - fragPos);
	vec3 n = normalize(normal);
	vec3 vd = normalize(viewPos - FragPos);

	//calculate attenuation
	float d = length(light.position - fragPos);
	float atten = 1.0f / (light.atten.constant + light.atten.linear * d + light.atten.quadratic * d * d);

	vec3 ambient = light.ambient * material.ambient;
	vec3 diffuse = light.diffuse * diff(ld, n) * material.diffuse;
	vec3 specular = light.specular * spec(ld, n, vd, material.shininess) * material.specular;

	return (ambient + diffuse + specular) * atten;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewPos)
{
    vec3 ld = normalize(light.position - fragPos);
	vec3 n = normalize(normal);
	vec3 vd = normalize(viewPos - FragPos);

	//calculate cutoff
	float theta = dot(-ld, light.direction);
	float epsilon = light.co.cutoff - light.co.outCutoff;
	float iten = clamp((theta - light.co.outCutoff) / epsilon, 0.0f, 1.0f);

	//calculate attenuation
	float d = length(light.position - fragPos);
	float atten = 1.0f / (light.atten.constant + light.atten.linear * d + light.atten.quadratic * d * d);

	vec3 ambient = light.ambient * material.ambient;
	vec3 diffuse = light.diffuse * diff(ld, n) * material.diffuse;
	vec3 specular = light.specular * spec(ld, n, vd, material.shininess) * material.specular;

	return (ambient + (diffuse + specular) * iten) * atten;
}