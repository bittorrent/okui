#pragma once

#include <okui/config.h>

#include <okui/Color.h>

#include <stdts/optional.h>
#include <stdts/string_view.h>

namespace okui {
namespace ml {

stdts::optional<Color> ParseColor(stdts::string_view str);

/**
* Returns the given string as a number.
*
* If oneHundredPercent is given, strings with a '%' suffix are interpreted as percentages.
*/
stdts::optional<double> ParseNumber(stdts::string_view str, double oneHundredPercent = 0.0);

}} // namespace okui::ml
