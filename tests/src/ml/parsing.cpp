#include <gtest/gtest.h>

#include <okui/ml/parsing.h>

TEST(ml_parsing, colors) {
    EXPECT_EQ(okui::ml::ParseColor("green"), okui::Color::kGreen);
}

TEST(ml_parsing, numbers) {
    std::unordered_map<std::string, double> units;
    units["%"] = 4.0 / 100.0;

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

