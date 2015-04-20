#include "bittorrent/ui/BitmapFont.h"

#include <regex>

namespace bittorrent {
namespace ui {

BitmapFont::BitmapFont(shared_ptr<Texture> texture, const std::string& metadata) : _texture(texture) {
	_parseMetadata(metadata.data(), metadata.size());
}

const BitmapFont::Glyph* BitmapFont::glyph(GlyphId id) const {
	auto it = _glyphs.find(id);
	return it == _glyphs.end() ? nullptr : &it->second;
}

double BitmapFont::kerning(GlyphId first, GlyphId second) const {
	auto it = _kernings.find(first);
	if (it == _kernings.end()) { return 0.0; }

	auto it2 = it->second.find(second);
	return it2 == it->second.end() ? 0.0 : it2->second;
}

double BitmapFont::width(const GlyphId* glyphs, size_t count) const {
	double width = 0.0;
	for (size_t i = 0; i < count; ++i) {
		auto g = glyph(glyphs[i]);
		if (!g) { continue; }
		if (i > 0) {
			width += kerning(glyphs[i - 1], glyphs[i]);
		}
		width += (i + 1 == count) ? (g->width - _padding) : g->xAdvance;
	}
	return width;
}

void BitmapFont::_parseMetadata(const void* data, size_t len) {
	std::stringstream ss(std::string((const char*)data, len));
	std::string line;

	while (std::getline(ss, line, '\n')) {
		_parseMetadataLine(line.c_str());
	}
}

void BitmapFont::_parseMetadataLine(const char* line) {
	std::cmatch matches;
	
	if (std::regex_match(line, matches, std::regex("info.*size=([0-9]+) .*padding=([0-9]+).*"))) {
		_size = std::strtod(matches.str(1).c_str(), nullptr);
		_padding = std::strtod(matches.str(2).c_str(), nullptr);
	} else if (std::regex_match(line, matches, std::regex("common +lineHeight=([0-9]+) +base=([0-9]+) +scaleW=([0-9]+) +scaleH=([0-9]+) .*"))) {
		_lineHeight = std::strtod(matches.str(1).c_str(), nullptr);
		_base       = _lineHeight - std::strtod(matches.str(2).c_str(), nullptr);
		_scaleW     = std::strtod(matches.str(3).c_str(), nullptr);
		_scaleH     = std::strtod(matches.str(4).c_str(), nullptr);
	} else if (std::regex_match(line, matches, std::regex("char +id=([0-9]+) +x=([\\-0-9]+) +y=([\\-0-9]+) +width=([0-9]+) +height=([0-9]+) +xoffset=([\\-0-9]+) +yoffset=([\\-0-9]+) +xadvance=([\\-0-9]+) +.*"))) {
		auto& glyph    = _glyphs[std::strtol(matches.str(1).c_str(), nullptr, 0)];

		auto textureScale = _texture->width() / _scaleW;

		glyph.width         = std::strtod(matches.str(4).c_str(), nullptr);
		glyph.height        = std::strtod(matches.str(5).c_str(), nullptr);
		glyph.textureWidth  = glyph.width * textureScale;
		glyph.textureHeight = glyph.height * textureScale;
		glyph.textureX      = std::strtod(matches.str(2).c_str(), nullptr) * textureScale;
		glyph.textureY      = (_scaleH - std::strtod(matches.str(3).c_str(), nullptr)) * textureScale - glyph.textureHeight;
		glyph.xOffset       = std::strtod(matches.str(6).c_str(), nullptr);
		glyph.yOffset       = _lineHeight - glyph.height - std::strtod(matches.str(7).c_str(), nullptr);
		glyph.xAdvance      = std::strtod(matches.str(8).c_str(), nullptr);
	} else if (std::regex_match(line, matches, std::regex("kerning +first=([0-9]+) +second=([0-9]+) +amount=([\\-0-9]+) .*"))) {
		_kernings[std::strtol(matches.str(1).c_str(), nullptr, 0)][std::strtol(matches.str(2).c_str(), nullptr, 0)] = std::strtod(matches.str(3).c_str(), nullptr);
	}
}

}}