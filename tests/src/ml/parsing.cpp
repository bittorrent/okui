#include <gtest/gtest.h>

#include <okui/ml/parsing.h>

TEST(ml_parsing, colors) {
    EXPECT_EQ(okui::ml::ParseColor("green"), okui::Color::kGreen);
}

TEST(ml_parsing, numbers) {
    EXPECT_EQ(okui::ml::ParseNumber("1"), 1.0);
    EXPECT_EQ(okui::ml::ParseNumber("50%", 4.0), 2.0);
}

