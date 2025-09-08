#include "LightPoint.h"

void LightPoint::AddTo(LightBuffer& lightBuffer)
{
	if (lightBuffer.NR_POINT_LIGHTS >= MAX_POINT_LIGHTS)
		return;

	int index = lightBuffer.NR_POINT_LIGHTS;

	lightBuffer.pointLights[index].position = position;
	lightBuffer.pointLights[index].constant = constant;
	lightBuffer.pointLights[index].linear = linear;
	lightBuffer.pointLights[index].quadratic = quadratic;
	lightBuffer.pointLights[index].ambient = ambient;
	lightBuffer.pointLights[index].diffuse = diffuse;
	lightBuffer.pointLights[index].specular = specular;
	lightBuffer.pointLights[index].color = color;
	lightBuffer.NR_POINT_LIGHTS++;
}