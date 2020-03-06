#include "ShaderIncludes.hlsli"

cbuffer ExternalData : register(b0) 
{
	Light dirLight;
	Light pointLight;
	Light dirLight3;

	float3 cameraPosition;
	float shininess;
}

Texture2D diffuseTexture: register(t0);
SamplerState samplerOptions: register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	input.color = diffuseTexture.Sample(samplerOptions, input.uv) * input.color;

	float4 light1 = CalculateLight(input.normal, input.color, input.worldPos, dirLight, cameraPosition, shininess);
	float4 light2 = CalculateLight(input.normal, input.color, input.worldPos, pointLight, cameraPosition, shininess);
	float4 light3 = CalculateLight(input.normal, input.color, input.worldPos, dirLight3, cameraPosition, shininess);

	return (light1 + light2 + light3);
}