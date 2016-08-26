#include "gtest/gtest.h"

#include "TestApplication.h"

#include "okui/Window.h"

using namespace onair;
using namespace okui;

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION
TEST(Window, setFocus) {
    TestApplication application;
    okui::Window window(&application);
    window.open();

    struct FocusableView : public View {
        virtual bool canBecomeFocus() override { return true; }
        virtual void focusGained() override {
            EXPECT_TRUE(isFocus());
            gained = true;
        }
        virtual void focusLost() override {
            EXPECT_FALSE(isFocus());
            lost = true;
        }

        bool gained = false;
        bool lost = false;
    };

    {
        FocusableView a, b, c;
        a.addSubview(&b);
        b.addSubview(&c);
        window.contentView()->addSubview(&a);

        b.focus();

        EXPECT_TRUE(a.gained);
        EXPECT_FALSE(a.lost);
        EXPECT_TRUE(b.gained);
        EXPECT_FALSE(b.lost);
        EXPECT_FALSE(c.gained);
        EXPECT_FALSE(c.lost);

        c.focus();

        EXPECT_TRUE(c.gained);

        a.focus();

        EXPECT_TRUE(b.lost);
        EXPECT_TRUE(c.lost);
    }
}
#endif
