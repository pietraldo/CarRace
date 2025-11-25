#pragma once

#include "InputStructures.h"

class InputController {
public:
    virtual CarControlInput getCarControlInput() = 0;
    virtual CameraControlInput getCameraControlInput() = 0;
    virtual AdditionalInputInfo getAdditionalInputInfo() = 0;
    virtual bool updateInput() = 0;
    virtual bool setEffectsOnInputer(bool vibration) { return true; }
};