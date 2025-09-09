#include "LightSpot.h"


void LightSpot::AddTo(LightBuffer& lightBuffer)
{
	if (lightBuffer.NR_SPOT_LIGHTS >= MAX_SPOT_LIGHTS)
		return;

	int index = lightBuffer.NR_SPOT_LIGHTS;

	lightBuffer.spotLights[index].position = position;
	lightBuffer.spotLights[index].constant = constant;
	lightBuffer.spotLights[index].linear = linear;
	lightBuffer.spotLights[index].quadratic = quadratic;
	lightBuffer.spotLights[index].cutOff = cutOff;
	lightBuffer.spotLights[index].outerCutOff = outerCutOff;
	lightBuffer.spotLights[index].direction = direction;
	lightBuffer.spotLights[index].ambient = ambient;
	lightBuffer.spotLights[index].diffuse = diffuse;
	lightBuffer.spotLights[index].specular = specular;
	lightBuffer.spotLights[index].color = color;
	lightBuffer.NR_SPOT_LIGHTS++;
}