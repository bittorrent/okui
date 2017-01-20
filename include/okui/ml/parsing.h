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

#include <okui/Color.h>
#include <okui/interpolation.h>

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

stdts::optional<std::chrono::duration<double>> ParseDuration(stdts::string_view str);

template <typename T>
auto ParseInterpolator(stdts::string_view str) -> T(*)(const T&, const T&, double) {
    if (str == "linear") { return interpolation::Linear<T>; }

    if (str == "sine-ease-in") { return interpolation::Sine::EaseIn<T>; }
    if (str == "sine-ease-out") { return interpolation::Sine::EaseOut<T>; }
    if (str == "sine-ease-in-out") { return interpolation::Sine::EaseInOut<T>; }

    if (str == "quadratic-ease-in") { return interpolation::Quadratic::EaseIn<T>; }
    if (str == "quadratic-ease-out") { return interpolation::Quadratic::EaseOut<T>; }
    if (str == "quadratic-ease-in-out") { return interpolation::Quadratic::EaseInOut<T>; }

    if (str == "cubic-ease-in") { return interpolation::Cubic::EaseIn<T>; }
    if (str == "cubic-ease-out") { return interpolation::Cubic::EaseOut<T>; }
    if (str == "cubic-ease-in-out") { return interpolation::Cubic::EaseInOut<T>; }

    if (str == "quartic-ease-in") { return interpolation::Quartic::EaseIn<T>; }
    if (str == "quartic-ease-out") { return interpolation::Quartic::EaseOut<T>; }
    if (str == "quartic-ease-in-out") { return interpolation::Quartic::EaseInOut<T>; }

    if (str == "quintic-ease-in") { return interpolation::Quintic::EaseIn<T>; }
    if (str == "quintic-ease-out") { return interpolation::Quintic::EaseOut<T>; }
    if (str == "quintic-ease-in-out") { return interpolation::Quintic::EaseInOut<T>; }

    if (str == "exponential-ease-in") { return interpolation::Exponential::EaseIn<T>; }
    if (str == "exponential-ease-out") { return interpolation::Exponential::EaseOut<T>; }
    if (str == "exponential-ease-in-out") { return interpolation::Exponential::EaseInOut<T>; }

    if (str == "circular-ease-in") { return interpolation::Circular::EaseIn<T>; }
    if (str == "circular-ease-out") { return interpolation::Circular::EaseOut<T>; }
    if (str == "circular-ease-in-out") { return interpolation::Circular::EaseInOut<T>; }

    if (str == "back-ease-in") { return interpolation::Back::EaseIn<T>; }
    if (str == "back-ease-out") { return interpolation::Back::EaseOut<T>; }
    if (str == "back-ease-in-out") { return interpolation::Back::EaseInOut<T>; }

    return nullptr;
}

} // namespace okui::ml
