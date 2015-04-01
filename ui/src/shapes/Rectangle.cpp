#include "bittorrent/ui/shapes/Rectangle.h"

#include "bittorrent/ui/Shader.h"

#include "bittorrent/ui/shapes/Arc.h"

#include <cmath>

namespace bittorrent {
namespace ui {
namespace shapes {

Rectangle& Rectangle::round(double radius, int corners) {
	cornerRadius = radius;
	roundedCorners = corners;
	return *this;
}

void Rectangle::draw(Shader* shader) {
	if (cornerRadius && roundedCorners) {
		// draw a cross shape
		Rectangle(x + cornerRadius, y, w - 2.0 * cornerRadius, h).draw(shader);
		Rectangle(x, y + cornerRadius, cornerRadius, h - 2.0 * cornerRadius).draw(shader);
		Rectangle(x + w - cornerRadius, y + cornerRadius, cornerRadius, h - 2.0 * cornerRadius).draw(shader);		

		// draw each corner
		
		auto segmentation = circularCorners ? 12.0 : 1.0;

		if (roundedCorners & kCornerMinMax) {
			Arc(x + cornerRadius, y + h - cornerRadius, cornerRadius, 0.5 * M_PI, 2.0 * M_PI / 4.0, segmentation).draw(shader);
		} else {
			Rectangle(x, y + h - cornerRadius, cornerRadius, cornerRadius).draw(shader);
		}

		if (roundedCorners & kCornerMinMin) {
			Arc(x + cornerRadius, y + cornerRadius, cornerRadius, 1.0 * M_PI, 2.0 * M_PI / 4.0, segmentation).draw(shader);
		} else {
			Rectangle(x, y, cornerRadius, cornerRadius).draw(shader);
		}

		if (roundedCorners & kCornerMaxMin) {
			Arc(x + w - cornerRadius, y + cornerRadius, cornerRadius, 1.5 * M_PI, 2.0 * M_PI / 4.0, segmentation).draw(shader);
		} else {
			Rectangle(x + w - cornerRadius, y, cornerRadius, cornerRadius).draw(shader);
		}

		if (roundedCorners & kCornerMaxMax) {
			Arc(x + w - cornerRadius, y + h - cornerRadius, cornerRadius, 0.0 * M_PI, 2.0 * M_PI / 4.0, segmentation).draw(shader);
		} else {
			Rectangle(x + w - cornerRadius, y + h - cornerRadius, cornerRadius, cornerRadius).draw(shader);
		}
	} else {
		shader->drawTriangle(x, y, x + w, y, x, y + h);
		shader->drawTriangle(x, y + h, x + w, y, x + w, y + h);
	}
}

}}}