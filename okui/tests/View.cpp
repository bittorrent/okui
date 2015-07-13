#include "gtest/gtest.h"

#include "TestApplication.h"

#include "onair/okui/View.h"

using namespace onair;
using namespace onair::okui;

TEST(View, ancestorsAreVisible) {
    View a;
    EXPECT_TRUE(a.ancestorsAreVisible());

    View b;
    b.addSubview(&a);
    EXPECT_TRUE(a.ancestorsAreVisible());

    b.setIsVisible(false);
    EXPECT_FALSE(a.ancestorsAreVisible());

    View c;
    c.addSubview(&b);
    EXPECT_FALSE(a.ancestorsAreVisible());

    b.setIsVisible(true);
    EXPECT_TRUE(a.ancestorsAreVisible());

    c.setIsVisible(false);
    EXPECT_FALSE(a.ancestorsAreVisible());
}

TEST(View, isVisibleInOpenWindow) {
    View a;
    EXPECT_FALSE(a.isVisibleInOpenWindow());

    TestApplication application;
    Window window(&application);

    window.contentView()->addSubview(&a);
    EXPECT_FALSE(a.isVisibleInOpenWindow());

    window.open();
    EXPECT_TRUE(a.isVisibleInOpenWindow());
    
    a.setIsVisible(false);
    EXPECT_FALSE(a.isVisibleInOpenWindow());
}

TEST(View, focus) {
    struct FocusableView : View {
        virtual bool canBecomeFocus() override { return true; }
    };
    
    View a;
    EXPECT_FALSE(a.isFocus());

    a.focus();
    EXPECT_FALSE(a.isFocus());

    FocusableView b;
    EXPECT_FALSE(b.isFocus());

    b.focus();
    EXPECT_FALSE(b.isFocus());

    TestApplication application;
    Window window(&application);
    window.contentView()->addSubviews(&a, &b);
    EXPECT_FALSE(a.isFocus());
    EXPECT_FALSE(b.isFocus());
    
    a.focus();
    EXPECT_FALSE(a.isFocus());
    EXPECT_FALSE(b.isFocus());

    b.focus();
    EXPECT_FALSE(a.isFocus());
    EXPECT_TRUE(b.isFocus());

    a.focus();
    EXPECT_FALSE(a.isFocus());
    EXPECT_TRUE(b.isFocus());

    a.unfocus();
    EXPECT_FALSE(a.isFocus());
    EXPECT_TRUE(b.isFocus());

    b.unfocus();
    EXPECT_FALSE(a.isFocus());
    EXPECT_FALSE(b.isFocus());
}
