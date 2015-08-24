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

TEST(View, focusCycle) {
    struct FocusableView : View {
        virtual bool canBecomeFocus() override { return enabled; }
        bool enabled = false;
    };
    
    FocusableView a;
    EXPECT_FALSE(a.nextAvailableFocus());

    FocusableView b;
    a.setNextFocus(&b);
    b.setNextFocus(&a);
    EXPECT_FALSE(a.nextAvailableFocus());
    EXPECT_FALSE(b.nextAvailableFocus());

    FocusableView c;
    b.setNextFocus(&c);
    c.setNextFocus(&a);
    EXPECT_FALSE(a.nextAvailableFocus());
    EXPECT_FALSE(b.nextAvailableFocus());
    EXPECT_FALSE(c.nextAvailableFocus());
    EXPECT_FALSE(a.previousAvailableFocus());
    EXPECT_FALSE(b.previousAvailableFocus());
    EXPECT_FALSE(c.previousAvailableFocus());

    b.enabled = true;
    EXPECT_EQ(a.nextAvailableFocus(), &b);
    EXPECT_FALSE(b.nextAvailableFocus());
    EXPECT_EQ(c.nextAvailableFocus(), &b);
    EXPECT_EQ(a.previousAvailableFocus(), &b);
    EXPECT_FALSE(b.previousAvailableFocus());
    EXPECT_EQ(c.previousAvailableFocus(), &b);

    a.enabled = true;
    EXPECT_EQ(a.nextAvailableFocus(), &b);
    EXPECT_EQ(b.nextAvailableFocus(), &a);
    EXPECT_EQ(c.nextAvailableFocus(), &a);
    EXPECT_EQ(a.previousAvailableFocus(), &b);
    EXPECT_EQ(b.previousAvailableFocus(), &a);
    EXPECT_EQ(c.previousAvailableFocus(), &b);

    c.enabled = true;
    EXPECT_EQ(a.nextAvailableFocus(), &b);
    EXPECT_EQ(b.nextAvailableFocus(), &c);
    EXPECT_EQ(c.nextAvailableFocus(), &a);
    EXPECT_EQ(a.previousAvailableFocus(), &c);
    EXPECT_EQ(b.previousAvailableFocus(), &a);
    EXPECT_EQ(c.previousAvailableFocus(), &b);
}

TEST(View, isDescendantOf) {
    View a, b, c;

    EXPECT_FALSE(a.isDescendantOf(&b));

    a.addSubview(&b);
    EXPECT_TRUE(b.isDescendantOf(&a));
    EXPECT_FALSE(a.isDescendantOf(&b));

    b.addSubview(&c);
    EXPECT_TRUE(c.isDescendantOf(&a));
    EXPECT_TRUE(c.isDescendantOf(&b));
    EXPECT_TRUE(b.isDescendantOf(&a));
    EXPECT_FALSE(b.isDescendantOf(&c));
    EXPECT_FALSE(a.isDescendantOf(&b));
    EXPECT_FALSE(a.isDescendantOf(&c));
}

TEST(View, appearedDisappeared) {
    View a;
    struct TestView : View {
        virtual void willAppear() override {
            EXPECT_EQ(step, 0);
            EXPECT_FALSE(isVisibleInOpenWindow());
            ++step;
        }
        virtual void appeared() override {
            EXPECT_EQ(step, 1);
            EXPECT_TRUE(isVisibleInOpenWindow());
            ++step;
        }
        virtual void willDisappear() override {
            EXPECT_EQ(step, 2);
            EXPECT_TRUE(isVisibleInOpenWindow());
            ++step;
        }
        virtual void disappeared() override {
            EXPECT_EQ(step, 3);
            EXPECT_FALSE(isVisibleInOpenWindow());
            ++step;
        }
        
        int step = 0;
    } b;

    EXPECT_FALSE(a.isVisibleInOpenWindow());

    a.addSubview(&b);
    EXPECT_FALSE(a.isVisibleInOpenWindow());
    EXPECT_FALSE(b.isVisibleInOpenWindow());

    TestApplication application;
    Window window(&application);
    window.contentView()->addSubview(&a);

    EXPECT_FALSE(a.isVisibleInOpenWindow());
    EXPECT_FALSE(b.isVisibleInOpenWindow());

    window.open();

    EXPECT_TRUE(a.isVisibleInOpenWindow());
    EXPECT_TRUE(b.isVisibleInOpenWindow());

    a.setIsVisible(false);

    EXPECT_FALSE(a.isVisibleInOpenWindow());
    EXPECT_FALSE(b.isVisibleInOpenWindow());

    EXPECT_EQ(b.step, 4);
}