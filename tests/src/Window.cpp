#include "TestApplication.h"

#include <okui/Window.h>

#include <gtest/gtest.h>

using namespace okui;

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION
namespace {
    struct FocusableView : public View {
        virtual bool canBecomeDirectFocus() override { return true; }
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
}

TEST(Window, setFocus) {
    TestApplication application;
    okui::Window window(&application);
    window.open();

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

TEST(Window, disabledPreferredFocus) {
    TestApplication application;
    okui::Window window(&application);
    window.open();

    FocusableView a, b;
    a.addSubview(&b);
    window.contentView()->addSubview(&a);
    a.setPreferredFocus(&b);

    a.focus();
    EXPECT_TRUE(b.isFocus());

    b.hide();
    EXPECT_FALSE(b.isFocus());
    EXPECT_TRUE(a.isFocus());

    a.unfocus();
    EXPECT_FALSE(a.isFocus());
    EXPECT_FALSE(b.isFocus());

    a.focus();
    EXPECT_TRUE(a.isFocus());
    EXPECT_FALSE(b.isFocus());
}
#endif
