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

namespace okui {
class Shader;
}

namespace okui::shapes {

class Arc {
public:
    Arc(double x, double y, double r, double orientation, double angle)
        : x(x), y(y), r(r), orientation(orientation), angle(angle) {}

    void draw(Shader* shader);

    double x, y, r, orientation, angle;
};

} // namespace okui::shapes
