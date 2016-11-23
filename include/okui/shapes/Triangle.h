#pragma once

#include <okui/config.h>

namespace okui {
class Shader;
}

namespace okui::shapes {

class Triangle {
public:
    Triangle(double x1, double y1, double x2, double y2, double x3, double y3)
        : x1(x1), y1(y1), x2(x2), y2(y2), x3(x3), y3(y3) {}

    void draw(Shader* shader) { shader->drawTriangle(x1, y1, x2, y2, x3, y3); }

    double x1, y1, x2, y2, x3, y3;
};

} // namespace okui::shapes
