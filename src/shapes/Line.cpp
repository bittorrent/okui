#include "okui/shapes/Line.h"

#include "okui/Shader.h"

#include <cmath>

namespace okui {
namespace shapes {

void Line::draw(Shader* shader) {
    Point<double> perpendicular{-(p2.y - p1.y), p2.x - p1.x};
    perpendicular.normalize();
    perpendicular *= thickness * 0.5;
    auto c1 = p1 + perpendicular;
    auto c2 = p1 - perpendicular;
    auto c3 = p2 + perpendicular;
    auto c4 = p2 - perpendicular;
    shader->drawTriangle(c1.x, c1.y, c2.x, c2.y, c3.x, c3.y);
    shader->drawTriangle(c2.x, c2.y, c3.x, c3.y, c4.x, c4.y);
}

} } // namespace okui::shapes
