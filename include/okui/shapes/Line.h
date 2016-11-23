#pragma once

#include <okui/config.h>

#include <okui/Point.h>

namespace okui {
class Shader;
}

namespace okui::shapes {

class Line {
public:
    Line(const okui::Point<double>& p1, const okui::Point<double>& p2, double thickness = 2.0)
        : p1{p1}, p2{p2}, thickness{thickness} {}

    Line(double p1x, double p1y, double p2x, double p2y, double thickness = 2.0)
        : p1{p1x, p1y}, p2{p2x, p2y}, thickness{thickness} {}

    void draw(Shader* shader);

    okui::Point<double> p1, p2;
    double thickness = 2.0;
};

} // namespace okui::shapes
