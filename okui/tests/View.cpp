#include "gtest/gtest.h"

#include "TestApplication.h"

#include "onair/okui/View.h"

using namespace onair;
using namespace onair::okui;

TEST(View, localToSuperview) {
    View a, b;
    b.setBounds(10, 20, 10, 10);
    a.addSubview(&b);
    auto p = b.localToSuperview(0, 0);
    EXPECT_EQ(p.x, 10);
    EXPECT_EQ(p.y, 20);
}

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION
TEST(View, localToWindow) {
    TestApplication application;
    Window window(&application);

    View a, b, c;
    b.setBounds(10, 20, 10, 10);
    a.addSubview(&b);
    c.setBounds(15, 27, 10, 10);
    b.addSubview(&c);
    window.contentView()->addSubview(&a);

    auto p = c.localToWindow(10, 10);
    EXPECT_EQ(p.x, 35);
    EXPECT_EQ(p.y, 57);
}
#endif

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

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION
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
#endif // ONAIR_OKUI_HAS_NATIVE_APPLICATION

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION
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
    EXPECT_TRUE(window.contentView()->isFocus());

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
#endif // ONAIR_OKUI_HAS_NATIVE_APPLICATION

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

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION
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
#endif // ONAIR_OKUI_HAS_NATIVE_APPLICATION

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION
TEST(View, hasRelation) {
    View a, b, c;

    EXPECT_FALSE(a.hasRelation(View::Relation::kApplication, &b));
    EXPECT_FALSE(a.hasRelation(View::Relation::kWindow, &b));
    EXPECT_FALSE(a.hasRelation(View::Relation::kDescendant, &b));
    EXPECT_FALSE(a.hasRelation(View::Relation::kAncestor, &b));
    EXPECT_FALSE(a.hasRelation(View::Relation::kSibling, &b));

    a.addSubview(&b);
    EXPECT_FALSE(a.hasRelation(View::Relation::kApplication, &b));
    EXPECT_FALSE(a.hasRelation(View::Relation::kWindow, &b));
    EXPECT_FALSE(a.hasRelation(View::Relation::kDescendant, &b));
    EXPECT_TRUE(a.hasRelation(View::Relation::kAncestor, &b));
    EXPECT_TRUE(b.hasRelation(View::Relation::kDescendant, &a));
    EXPECT_FALSE(a.hasRelation(View::Relation::kSibling, &b));

    a.addSubview(&c);
    EXPECT_FALSE(c.hasRelation(View::Relation::kApplication, &b));
    EXPECT_FALSE(c.hasRelation(View::Relation::kWindow, &b));
    EXPECT_FALSE(c.hasRelation(View::Relation::kDescendant, &b));
    EXPECT_FALSE(c.hasRelation(View::Relation::kAncestor, &b));
    EXPECT_FALSE(a.hasRelation(View::Relation::kSibling, &c));
    EXPECT_FALSE(c.hasRelation(View::Relation::kSibling, &a));
    EXPECT_TRUE(c.hasRelation(View::Relation::kSibling, &b));
    EXPECT_TRUE(b.hasRelation(View::Relation::kSibling, &c));
    EXPECT_FALSE(b.hasRelation(View::Relation::kSibling, &b));

    TestApplication application;
    Window window(&application);
    window.contentView()->addSubview(&a);

    EXPECT_TRUE(a.hasRelation(View::Relation::kApplication, &b));
    EXPECT_TRUE(a.hasRelation(View::Relation::kWindow, &b));
    EXPECT_TRUE(a.hasRelation(View::Relation::kApplication, &c));
    EXPECT_TRUE(a.hasRelation(View::Relation::kWindow, &c));
    EXPECT_TRUE(b.hasRelation(View::Relation::kApplication, &c));
    EXPECT_TRUE(b.hasRelation(View::Relation::kWindow, &c));

    Window secondWindow(&application);
    secondWindow.contentView()->addSubview(&c);

    EXPECT_TRUE(a.hasRelation(View::Relation::kApplication, &b));
    EXPECT_TRUE(a.hasRelation(View::Relation::kWindow, &b));
    EXPECT_TRUE(a.hasRelation(View::Relation::kApplication, &c));
    EXPECT_FALSE(a.hasRelation(View::Relation::kWindow, &c));
    EXPECT_TRUE(b.hasRelation(View::Relation::kApplication, &c));
    EXPECT_FALSE(b.hasRelation(View::Relation::kWindow, &c));
}
#endif // ONAIR_OKUI_HAS_NATIVE_APPLICATION

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION
TEST(View, messagePosting) {
    View a, b, c;

    int received = 0;

    b.listen([&] (const int& message) {
        EXPECT_EQ(message, 7);
        ++received;
    }, View::Relation::kAncestor);

    c.listen([&] (const int& message, View* sender) {
        EXPECT_EQ(sender, &a);
        EXPECT_EQ(message, 7);
        ++received;
    }, View::Relation::kAncestor);

    TestApplication application;
    Window window(&application);
    window.contentView()->addSubview(&a);
    a.addSubview(&b);
    a.addSubview(&c);

    int message = 7;
    a.post(message, View::Relation::kDescendant);
        
    EXPECT_EQ(received, 2);
}
#endif // ONAIR_OKUI_HAS_NATIVE_APPLICATION

TEST(View, provisions) {
    View a, b, c;

    int n = 0;
    
    a.provide(&n);

    a.addSubview(&b);
    b.addSubview(&c);
    
    EXPECT_EQ(b.get<int>(), &n);
    EXPECT_EQ(c.get<int>(), &n);

    int x = 1;
    a.provide(&x, 1);
    
    EXPECT_EQ(b.get<int>(1), &x);
    EXPECT_EQ(c.get<int>(1), &x);
}
