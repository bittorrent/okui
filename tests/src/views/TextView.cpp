#include "../TestApplication.h"

#include <okui/views/TextView.h>

#include <gtest/gtest.h>

using namespace okui;
using namespace okui::views;

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION

TEST(TextView, utf8Text) {
    TestApplication application;
    okui::Window window(&application);

    TextView text;
    text.setFont("Montserrat-regular.png", "Montserrat-regular.fnt");
    window.contentView()->addSubview(&text);

    text.setText("Toshirô Mifune");
}

#endif // ONAIR_OKUI_HAS_NATIVE_APPLICATION
