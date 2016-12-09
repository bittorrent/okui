#include "../../TestApplication.h"

#include <okui/View.h>
#include <okui/ml/Context.h>
#include <okui/ml/Environment.h>

#include <gtest/gtest.h>

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION
TEST(ml_elements_StateMachine, basics) {
    TestApplication application;
    okui::Window window(&application);

    okui::ml::Environment ml;
    okui::ml::Context context(&ml);
    context.define("state", "green");

    auto element = context.load(R"(
    <view>
        <statemachine state="{state}">
            <state id="green" background-color="green" />
            <state id="blue" background-color="blue" />
        </statemachine>
    </view>
    )");

    auto view = element->view();
    ASSERT_TRUE(view);
    window.contentView()->addSubview(view);

    window.open();

    view->asyncAfter(100ms, [&] {
        EXPECT_EQ(view->backgroundColor(), okui::Color::kGreen);
        context.define("state", "blue");
        element->update();
        view->asyncAfter(100ms, [&] {
            EXPECT_EQ(view->backgroundColor(), okui::Color::kBlue);
            application.quit();
        });
    });

    application.run();
}
#endif
