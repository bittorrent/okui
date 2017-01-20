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
