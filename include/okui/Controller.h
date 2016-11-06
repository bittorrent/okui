#pragma once

#include <okui/config.h>

namespace okui {

class Controller {
public:
    enum class AxisType {
        kJoystickX, kJoystickY, kUnknown
    };

    virtual ~Controller() {}

    virtual std::string name() const = 0;
    virtual std::string guid() const = 0;

    virtual size_t buttons() const = 0;

    virtual size_t axes() const = 0;
    virtual AxisType axisType(size_t index) const = 0;
};

} // namespace okui
