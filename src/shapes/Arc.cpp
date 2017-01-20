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
#include <okui/shapes/Arc.h>

#include <okui/Shader.h>

#include <cmath>

namespace okui::shapes {

void Arc::draw(Shader* shader) {
    int segments = ceil(angle / (2.0 * M_PI / 4.0));

    auto segmentAngle = angle / segments;
    auto outerRadius = 2.0 * r;

    auto a = orientation;

    auto pX = x + outerRadius * cos(a);
    auto pY = y + outerRadius * sin(a);

    for (int i = 0; i < segments; ++i) {
        auto pX2 = x + outerRadius * cos(a + segmentAngle);
        auto pY2 = y + outerRadius * sin(a + segmentAngle);
        shader->drawTriangle(pX, pY, x, y, pX2, pY2, Shader::kCurveCircularConvex);
        pX = pX2;
        pY = pY2;
        a += segmentAngle;
    }
}

} // namespace okui::shapes
