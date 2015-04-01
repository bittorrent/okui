#include "bittorrent/ui/shapes/Arc.h"

#include "bittorrent/ui/Shader.h"

#include <cmath>

namespace bittorrent {
namespace ui {
namespace shapes {

void Arc::draw(Shader* shader) {
	int segments = ceil(angle / (2.0 * M_PI / segmentation));
	
	auto segmentAngle = angle / segments;
	auto halfSegmentAngle = 0.5 * segmentAngle;
	auto outerRadius = r / cos(halfSegmentAngle);
	
	auto a = orientation;
	
	auto pX = x + r * cos(a);
	auto pY = y + r * sin(a);

	for (int i = 0; i < segments; ++i) {
		auto pX2 = x + r * cos(a + segmentAngle);
		auto pY2 = y + r * sin(a + segmentAngle);
		shader->drawTriangle(pX, pY, x + outerRadius * cos(a + halfSegmentAngle), y + outerRadius * sin(a + halfSegmentAngle), pX2, pY2, Shader::kBezierConvex);
		shader->drawTriangle(x, y, pX, pY, pX2, pY2);
		pX = pX2;
		pY = pY2;
		a += segmentAngle;
	}
}

}}}