#include <okui/ml/parsing.h>

#include <gtest/gtest.h>

using namespace okui::literals;
using namespace std::literals;

TEST(ml_parsing, colors) {
    EXPECT_EQ(okui::ml::ParseColor("green"), okui::Color::kGreen);
    EXPECT_EQ(okui::ml::ParseColor("#c7b299"), 0xc7b299_rgb);
    EXPECT_EQ(okui::ml::ParseColor("#c7b29970"), 0xc7b29970_rgba);
}

TEST(ml_parsing, numbers) {
    std::unordered_map<std::string, double> units;
    units["%"] = 4.0 / 100.0;

    EXPECT_EQ(okui::ml::ParseNumber("50%"), 0.5);
    EXPECT_EQ(okui::ml::ParseNumber("1"), 1.0);
    EXPECT_EQ(okui::ml::ParseNumber("50%", units), 2.0);
    EXPECT_EQ(okui::ml::ParseNumber("50% + 1", units), 3.0);
    EXPECT_EQ(okui::ml::ParseNumber("2 + 3 * 4"), 14.0);
    EXPECT_EQ(okui::ml::ParseNumber("2 * 3 + 4"), 10.0);
    EXPECT_EQ(okui::ml::ParseNumber("2 * (3 + 4)"), 14.0);
    EXPECT_EQ(okui::ml::ParseNumber("-2"), -2.0);
    EXPECT_EQ(okui::ml::ParseNumber("3 * -2"), -6.0);

    EXPECT_FALSE(okui::ml::ParseNumber("2 * (3 + 4) !"));
    EXPECT_FALSE(okui::ml::ParseNumber("2 * (3 + 4"));
    EXPECT_FALSE(okui::ml::ParseNumber("2 * 3 + 4)"));
}

TEST(ml_parsing, boolean) {
    EXPECT_FALSE(okui::ml::ParseBoolean("0").value_or(true));
    EXPECT_FALSE(okui::ml::ParseBoolean("false").value_or(true));
    EXPECT_TRUE(okui::ml::ParseBoolean("1").value_or(false));
    EXPECT_TRUE(okui::ml::ParseBoolean("true").value_or(false));
}

TEST(ml_parsing, duration) {
    EXPECT_EQ(*okui::ml::ParseDuration("300ms"), 300ms);
    EXPECT_EQ(*okui::ml::ParseDuration("2s + 300ms"), 2300ms);
    EXPECT_EQ(*okui::ml::ParseDuration("4h"), 4h);
    EXPECT_EQ(*okui::ml::ParseDuration("5m"), 5min);
}

TEST(ml_parsing, interpolator) {
    EXPECT_TRUE(okui::ml::ParseInterpolator<double>("quadratic-ease-in") == okui::interpolation::Quadratic::EaseIn<double>);
    EXPECT_TRUE(okui::ml::ParseInterpolator<double>("quadratic-ease-out") == okui::interpolation::Quadratic::EaseOut<double>);
}
