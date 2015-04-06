#include "bittorrent/ui/shapes/Arc.h"

#include "bittorrent/ui/Shader.h"

#include <cmath>

namespace bittorrent {
namespace ui {
namespace shapes {

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

}}}