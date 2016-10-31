#include <okui/ml/parsing.h>

#include <scraps/utility.h>

namespace okui {
namespace ml {

stdts::optional<Color> ParseColor(stdts::string_view str) {
    if (scraps::CaseInsensitiveEquals(str, "white")) { return Color::kWhite; }
    if (scraps::CaseInsensitiveEquals(str, "silver")) { return Color::kSilver; }
    if (scraps::CaseInsensitiveEquals(str, "gray")) { return Color::kGray; }
    if (scraps::CaseInsensitiveEquals(str, "black")) { return Color::kBlack; }
    if (scraps::CaseInsensitiveEquals(str, "transparentblack")) { return Color::kTransparentBlack; }
    if (scraps::CaseInsensitiveEquals(str, "red")) { return Color::kRed; }
    if (scraps::CaseInsensitiveEquals(str, "maroon")) { return Color::kMaroon; }
    if (scraps::CaseInsensitiveEquals(str, "yellow")) { return Color::kYellow; }
    if (scraps::CaseInsensitiveEquals(str, "olive")) { return Color::kOlive; }
    if (scraps::CaseInsensitiveEquals(str, "lime")) { return Color::kLime; }
    if (scraps::CaseInsensitiveEquals(str, "green")) { return Color::kGreen; }
    if (scraps::CaseInsensitiveEquals(str, "aqua")) { return Color::kAqua; }
    if (scraps::CaseInsensitiveEquals(str, "teal")) { return Color::kTeal; }
    if (scraps::CaseInsensitiveEquals(str, "blue")) { return Color::kBlue; }
    if (scraps::CaseInsensitiveEquals(str, "navy")) { return Color::kNavy; }
    if (scraps::CaseInsensitiveEquals(str, "fuchsia")) { return Color::kFuchsia; }
    if (scraps::CaseInsensitiveEquals(str, "purple")) { return Color::kPurple; }

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
        auto it = units.find(suffix.to_string());
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
    stdts::optional<double> result;

    while (true) {
        while (str && *str == ' ') {
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
    auto string = str.to_string();
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

}} // namespace okui::ml

