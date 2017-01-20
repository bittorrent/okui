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
#include <okui/ml/parsing.h>

#include <scraps/utility.h>

#include <cassert>

namespace okui::ml {

stdts::optional<Color> ParseColor(stdts::string_view str) {
    if (str == "white") { return Color::kWhite; }
    if (str == "silver") { return Color::kSilver; }
    if (str == "gray") { return Color::kGray; }
    if (str == "black") { return Color::kBlack; }
    if (str == "transparentblack") { return Color::kTransparentBlack; }
    if (str == "red") { return Color::kRed; }
    if (str == "maroon") { return Color::kMaroon; }
    if (str == "yellow") { return Color::kYellow; }
    if (str == "olive") { return Color::kOlive; }
    if (str == "lime") { return Color::kLime; }
    if (str == "green") { return Color::kGreen; }
    if (str == "aqua") { return Color::kAqua; }
    if (str == "teal") { return Color::kTeal; }
    if (str == "blue") { return Color::kBlue; }
    if (str == "navy") { return Color::kNavy; }
    if (str == "fuchsia") { return Color::kFuchsia; }
    if (str == "purple") { return Color::kPurple; }

    if ((str.size() == 7 || str.size() == 9) && str[0] == '#' && str.find_first_not_of("0123456789abcdef", 1) == stdts::string_view::npos) {
        return RGBA(
            scraps::HexToDec(str[1]) << 4 | scraps::HexToDec(str[2]),
            scraps::HexToDec(str[3]) << 4 | scraps::HexToDec(str[4]),
            scraps::HexToDec(str[5]) << 4 | scraps::HexToDec(str[6]),
            str.size() == 9 ? scraps::HexToDec(str[7]) << 4 | scraps::HexToDec(str[8]) : 255
        );
    }

    return {};
}

namespace {

stdts::optional<double> EvaluateNumber(const char* str, const char** end, const std::unordered_map<std::string, double>& units) {
    double n = 0.0;
    int numberLength = 0;
    if (sscanf(str, "%lf%n", &n, &numberLength) != 1) {
        SCRAPS_LOG_WARNING("unable to parse number from expression: {}", str);
        return {};
    }
    str += numberLength;

    auto suffix = stdts::string_view(str, strspn(str, "abcdefghijklmnopqrstuvwxyz%"));
    if (!suffix.empty()) {
        auto it = units.find(std::string(suffix));
        if (it != units.end()) {
            n *= it->second;
        } else if (suffix == "%") {
            n *= 0.01;
        } else {
            SCRAPS_LOG_WARNING("invalid suffix: {}", suffix);
            return {};
        }
        str += suffix.size();
    }
    *end = str;
    return n;
}

stdts::optional<double> EvaluateNumberExpression(const char* str, const char** end, const std::unordered_map<std::string, double>& units, int minBinding = 0) {
    assert(str);
    stdts::optional<double> result;

    while (true) {
        while (*str == ' ') {
            ++str;
        }

        if (!*str) { break; }

        if (result) {
            if (*str == '+' || *str == '-' || *str == '*' || *str == '/') {
                auto op = *str;
                auto binding = (op == '+' || op == '-') ? 10 : 20;
                if (binding < minBinding) {
                    break;
                }
                ++str;
                auto right = EvaluateNumberExpression(str, &str, units, binding);
                if (!right) { return {}; }
                switch (op) {
                    case '+': result = *result + *right; break;
                    case '-': result = *result - *right; break;
                    case '*': result = *result * *right; break;
                    case '/': result = *result / *right; break;
                }
            } else if (*str == ')') {
                break;
            } else {
                SCRAPS_LOG_WARNING("unexpected token: {}", str);
                return {};
            }
        } else if (*str == '(') {
            ++str;
            result = EvaluateNumberExpression(str, &str, units);
            if (!result) { return {}; }
            if (*str != ')') {
                SCRAPS_LOG_WARNING("expected closing parenthesis: {}", str);
                return {};
            }
            ++str;
        } else {
            result = EvaluateNumber(str, &str, units);
            if (!result) { return {}; }
        }
    }

    if (end) { *end = str; }
    return result;
}

}

stdts::optional<double> ParseNumber(stdts::string_view str, const std::unordered_map<std::string, double>& units) {
    std::string string(str);
    const char* end = nullptr;
    if (auto n = EvaluateNumberExpression(string.c_str(), &end, units)) {
        if (end != string.c_str() + string.size()) {
            SCRAPS_LOG_WARNING("unexpected token: {}", end);
            return {};
        }
        return n;
    }
    return {};
}

stdts::optional<bool> ParseBoolean(stdts::string_view str) {
    if (str == "true" || str == "1") {
        return true;
    } else if (str == "false" || str == "0") {
        return false;
    }
    return {};
}

stdts::optional<std::chrono::duration<double>> ParseDuration(stdts::string_view str) {
    static const std::unordered_map<std::string, double> units{
        {"h", 60 * 60.0},
        {"m", 60.0},
        {"s", 1.0},
        {"ms", 0.001},
        {"us", 0.000001},
    };
    if (auto seconds = ParseNumber(str, units)) {
        return std::chrono::duration<double>(*seconds);
    }
    return {};
}

} // namespace okui::ml
