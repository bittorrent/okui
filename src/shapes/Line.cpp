/**
* Copyright 2017 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include <okui/shapes/Line.h>

#include <okui/Shader.h>

#include <cmath>

namespace okui::shapes {

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

} // namespace okui::shapes
