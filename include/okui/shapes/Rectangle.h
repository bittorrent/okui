#pragma once

#include <okui/config.h>

#include <okui/Rectangle.h>
#include <okui/AffineTransformation.h>

namespace okui {
class Shader;
}

namespace okui::shapes {

class Rectangle {
public:
    Rectangle(const okui::Rectangle<double>& r) : Rectangle(r.x, r.y, r.width, r.height) {}
    Rectangle(double x, double y, double w, double h) : x(x), y(y), w(w), h(h) {}

    enum {
        kCornerMinMin = (1 << 0),
        kCornerMinMax = (1 << 1),
        kCornerMaxMin = (1 << 2),
        kCornerMaxMax = (1 << 3),
    };

    Rectangle& round(double radius, int corners = kCornerMinMin | kCornerMinMax | kCornerMaxMin | kCornerMaxMax);

    Rectangle& rotate(double r);

    Rectangle& rotate(double r, double cX, double cY);

    void draw(Shader* shader);

    double x, y, w, h;
    double cornerRadius = 0.0;
    int roundedCorners = 0;

private:
    AffineTransformation _transform;
    double _rotation = 0;
};

} // namespace okui::shapes
