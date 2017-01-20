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
