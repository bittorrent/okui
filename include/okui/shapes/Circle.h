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

#include <okui/shapes/Arc.h>

namespace okui {
class Shader;
}

namespace okui::shapes {

class Circle {
public:
    Circle(double x, double y, double r)
        : x(x), y(y), r(r) {}

    void draw(Shader* shader) {
        Arc(x, y, r, 0.0, 2.0 * M_PI).draw(shader);
    }

    double x, y, r;
};

} // namespace okui::shapes
