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
