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

    return {};
}

stdts::optional<double> ParseNumber(stdts::string_view str, double oneHundredPercent) {
    if (!str.empty() && str.back() == '%') {
        return std::strtod(str.substr(0, str.size() - 1).to_string().c_str(), nullptr) / 100.0 * oneHundredPercent;
    }
    return std::strtod(str.to_string().c_str(), nullptr);
}

}} // namespace okui::ml

