#pragma once

#include "InputController.h"

// Forward declaration of internal data struct to hide Windows headers
struct XboxControllerData;

class XboxController : public InputController {
public:
    XboxController(int controllerIndex = 0);
    ~XboxController();  // Destructor needed for PIMPL cleanup

    // InputController interface
    CarControlInput getCarControlInput() override;
    CameraControlInput getCameraControlInput() override;
    AdditionalInputInfo getAdditionalInputInfo() override;
    std::string GetCarControllBindings() override;
    std::string GetCameraControllBindings() override;
    std::string GetAdditionalControllBindings() override;
    bool updateInput() override;
    bool setEffectsOnInputer(EffectsOnInputer effects) override;

    // Custom
    bool connect();
    bool isConnected() const;

private:
    int _controllerIndex;
    bool _connected;

    // Opaque pointer to implementation details (PIMPL)
    XboxControllerData* _data;

    // Config
    const float LEFT_STICK_DEADZONE = 0.15f;
    const float RIGHT_STICK_DEADZONE = 0.15f;
    const float TRIGGER_THRESHOLD = 0.05f;

    // Use standard types instead of Windows WORD/SHORT in header
    bool isButtonPressed(unsigned short buttonMask) const;
    bool isButtonJustPressed(unsigned short buttonMask);

    float applyDeadzone(short value, float deadzone);
    float getStickValue(short value, short deadzone);
};
