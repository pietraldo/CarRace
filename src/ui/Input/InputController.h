#pragma once

#include <string>

#include "InputStructures.h"

class InputController {
public:
    virtual CarControlInput getCarControlInput() = 0;
    virtual CameraControlInput getCameraControlInput() = 0;
    virtual AdditionalInputInfo getAdditionalInputInfo() = 0;
    virtual std::string GetCarControllBindings() = 0;
    virtual std::string GetCameraControllBindings() = 0;
    virtual std::string GetAdditionalControllBindings() = 0;
    virtual bool updateInput() = 0;
    virtual bool setEffectsOnInputer(EffectsOnInputer effects) { return true; }
};