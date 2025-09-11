#include "CubeObejct.h"
#include "../../gfx/Cube.h"

void CubeObject::Draw()
{
    Cube::Draw(position, rotation, scale, color);
}