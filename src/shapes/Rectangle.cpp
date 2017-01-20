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
#include <okui/shapes/Rectangle.h>

#include <okui/Shader.h>
#include <okui/shapes/Arc.h>

#include <cmath>

namespace okui::shapes {

Rectangle& Rectangle::round(double radius, int corners) {
    cornerRadius = radius;
    roundedCorners = corners;
    return *this;
}

Rectangle& Rectangle::rotate(double r) {
    _rotation = r;
    _transform = AffineTransformation{(x + w/2), (y + h/2), -(x + w/2), -(y + h/2), 1.0, 1.0, r};
    return *this;
}

Rectangle& Rectangle::rotate(double r, double cX, double cY) {
    _rotation = r;
    _transform = AffineTransformation{cX, cY, -cX, -cY, 1.0, 1.0, r};
    return *this;
}

void Rectangle::draw(Shader* shader) {
    if (cornerRadius && roundedCorners) {
        auto centerX = (x + w/2),
             centerY = (y + h/2);

        // draw a cross shape
        Rectangle(x + cornerRadius, y, w - 2.0 * cornerRadius, h).rotate(_rotation).draw(shader);
        Rectangle(x, y + cornerRadius, cornerRadius, h - 2.0 * cornerRadius).rotate(_rotation, centerX, centerY).draw(shader);
        Rectangle(x + w - cornerRadius, y + cornerRadius, cornerRadius, h - 2.0 * cornerRadius).rotate(_rotation, centerX, centerY).draw(shader);

        // draw each corner
        double x1, y1;

        if (roundedCorners & kCornerMinMax) {
            _transform.transform(x + cornerRadius, y + h - cornerRadius, &x1, &y1);
            Arc(x1, y1, cornerRadius, 0.5 * M_PI + _rotation, 2.0 * M_PI / 4.0).draw(shader);
        } else {
            Rectangle(x, y + h - cornerRadius, cornerRadius, cornerRadius).rotate(_rotation, centerX, centerY).draw(shader);
        }

        if (roundedCorners & kCornerMinMin) {
            _transform.transform(x + cornerRadius, y + cornerRadius, &x1, &y1);
            Arc(x1, y1, cornerRadius, 1.0 * M_PI + _rotation, 2.0 * M_PI / 4.0).draw(shader);
        } else {
            Rectangle(x, y, cornerRadius, cornerRadius).rotate(_rotation, centerX, centerY).draw(shader);
        }

        if (roundedCorners & kCornerMaxMin) {
            _transform.transform(x + w - cornerRadius, y + cornerRadius, &x1, &y1);
            Arc(x1, y1, cornerRadius, 1.5 * M_PI + _rotation, 2.0 * M_PI / 4.0).draw(shader);
        } else {
            Rectangle(x + w - cornerRadius, y, cornerRadius, cornerRadius).rotate(_rotation, centerX, centerY).draw(shader);
        }

        if (roundedCorners & kCornerMaxMax) {
            _transform.transform(x + w - cornerRadius, y + h - cornerRadius, &x1, &y1);
            Arc(x1, y1, cornerRadius, 0.0 * M_PI + _rotation, 2.0 * M_PI / 4.0).draw(shader);
        } else {
            Rectangle(x + w - cornerRadius, y + h - cornerRadius, cornerRadius, cornerRadius).rotate(_rotation, centerX, centerY).draw(shader);
        }

        shader->flush();
    } else {
        double p1X, p1Y;
        _transform.transform(x, y, &p1X, &p1Y);
        double p2X, p2Y;
        _transform.transform(x+w, y, &p2X, &p2Y);
        double p3X, p3Y;
        _transform.transform(x, y+h, &p3X, &p3Y);
        double p4X, p4Y;
        _transform.transform(x+w, y+h, &p4X, &p4Y);

        shader->drawTriangle(p1X, p1Y, p2X, p2Y, p3X, p3Y);
        shader->drawTriangle(p3X, p3Y, p2X, p2Y, p4X, p4Y);
    }
}

} // namespace okui::shapes
