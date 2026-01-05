#pragma once

#include "InputController.h"

struct XboxControllerData;

class XboxController : public InputController {
public:
    XboxController(int controllerIndex = 0);
    ~XboxController();

    CarControlInput getCarControlInput() override;
    CameraControlInput getCameraControlInput() override;
    AdditionalInputInfo getAdditionalInputInfo() override;
    std::string GetCarControllBindings() override;
    std::string GetCameraControllBindings() override;
    std::string GetAdditionalControllBindings() override;
    bool updateInput() override;
    bool setEffectsOnInputer(EffectsOnInputer effects) override;

    bool connect();
    bool isConnected() const;

private:
    int _controllerIndex;
    bool _connected;

    XboxControllerData* _data;

    // Config
    const float LEFT_STICK_DEADZONE = 0.15f;
    const float RIGHT_STICK_DEADZONE = 0.15f;
    const float TRIGGER_THRESHOLD = 0.05f;

    bool isButtonPressed(unsigned short buttonMask) const;
    bool isButtonJustPressed(unsigned short buttonMask);

    float applyDeadzone(short value, float deadzone);
    float getStickValue(short value, short deadzone);
};
