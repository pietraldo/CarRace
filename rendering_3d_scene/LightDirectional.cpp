#include "LightDirectional.h"

void LightDirectional::AddTo(LightBuffer& lightBuffer)
{
	if (lightBuffer.NR_DIR_LIGHTS >= MAX_DIR_LIGHTS)
		return;

	int index = lightBuffer.NR_DIR_LIGHTS;

	lightBuffer.dirLights[index].direction = direction;
	lightBuffer.dirLights[index].ambient = ambient;
	lightBuffer.dirLights[index].diffuse = diffuse;
	lightBuffer.dirLights[index].specular = specular;
	lightBuffer.dirLights[index].color = color;
	lightBuffer.NR_DIR_LIGHTS++;
}