#pragma once

#include <okui/config.h>

#include <okui/Color.h>

#include <stdts/optional.h>
#include <stdts/string_view.h>

#include <unordered_map>

namespace okui::ml {

stdts::optional<Color> ParseColor(stdts::string_view str);

/**
* Returns the given string as a number.
*
* @param units a mapping of suffixes to unit base. suffixes can contain lowercase letters and '%'
*/
stdts::optional<double> ParseNumber(stdts::string_view str, const std::unordered_map<std::string, double>& units = {});

stdts::optional<bool> ParseBoolean(stdts::string_view str);

} // namespace okui::ml
