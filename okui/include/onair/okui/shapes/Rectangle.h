#pragma once

#include "onair/okui/config.h"
#include "onair/okui/Rectangle.h"

namespace onair {
namespace okui {

class Shader;

namespace shapes {

class Rectangle {
public:
    Rectangle(const onair::okui::Rectangle<double>& r) : Rectangle(r.x, r.y, r.width, r.height) {}

    Rectangle(double x, double y, double w, double h)
        : x(x), y(y), w(w), h(h) {}
    
    enum {
        kCornerMinMin = (1 << 0),
        kCornerMinMax = (1 << 1),
        kCornerMaxMin = (1 << 2),
        kCornerMaxMax = (1 << 3),
    };
    
    Rectangle& round(double radius, int corners = kCornerMinMin | kCornerMinMax | kCornerMaxMin | kCornerMaxMax);

    void draw(Shader* shader);

    double x, y, w, h;
    double cornerRadius = 0.0;
    int roundedCorners = 0;
};

}}}
