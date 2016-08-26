#include "gtest/gtest.h"

#include "TestApplication.h"

#include "okui/View.h"

using namespace onair;
using namespace okui;

TEST(View, localToSuperview) {
    View a, b;
    b.setBounds(10, 20, 10, 10);
    a.addSubview(&b);
    auto p = b.localToSuperview(0, 0);
    EXPECT_EQ(p.x, 10);
    EXPECT_EQ(p.y, 20);
}

TEST(View, localToAncestor) {
    View a, b, c;
    b.setBounds(10, 20, 10, 10);
    a.addSubview(&b);
    c.setBounds(20, 25, 5, 5);
    b.addSubview(&c);
    auto p = c.localToAncestor(0, 0, &a);
    EXPECT_EQ(p.x, 30);
    EXPECT_EQ(p.y, 45);
}

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION
TEST(View, localToWindow) {
    TestApplication application;
    okui::Window window(&application);

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
    okui::Window window(&application);

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
    okui::Window window(&application);
    window.open();
    window.contentView()->addSubview(&a);
    window.contentView()->addSubview(&b);
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

TEST(View, removeSubviewRefocuses) {
    struct FocusableView : View {
        virtual bool canBecomeFocus() override { return true; }
    };

    TestApplication application;
    Window window{&application};
    window.open();
    FocusableView a, b, c;
    window.contentView()->addSubview(&a);
    a.addSubview(&b);
    a.addSubview(&c);

    b.focus();
    ASSERT_TRUE(a.isFocus());
    ASSERT_TRUE(b.isFocus());
    ASSERT_FALSE(c.isFocus());

    a.removeSubview(&b);
    EXPECT_TRUE(a.isFocus());
    EXPECT_FALSE(b.isFocus());
    EXPECT_FALSE(c.isFocus());

    a.setPreferredFocus(&c);
    a.addSubview(&b);
    b.focus();
    EXPECT_TRUE(a.isFocus());
    EXPECT_TRUE(b.isFocus());
    EXPECT_FALSE(c.isFocus());

    a.removeSubview(&b);
    EXPECT_TRUE(a.isFocus());
    EXPECT_FALSE(b.isFocus());
    EXPECT_TRUE(c.isFocus());
}

TEST(View, removeSubviews) {
    View a, b, c;
    a.addSubview(&b);
    a.addSubview(&c);
    EXPECT_EQ(a.subviews().size(), 2);
    a.removeSubviews();
    EXPECT_EQ(a.subviews().size(), 0);
}

TEST(View, destructionRefocuses) {
    struct FocusableView : View {
        virtual bool canBecomeFocus() override { return true; }
    };

    TestApplication application;
    Window window{&application};
    window.open();
    FocusableView a;
    window.contentView()->addSubview(&a);
    {
        FocusableView b;
        a.addSubview(&b);
        b.focus();
        ASSERT_TRUE(a.isFocus());
        ASSERT_TRUE(b.isFocus());
    }

    EXPECT_TRUE(a.isFocus());
}

TEST(View, focusAncestor) {
    struct FocusableView : View {
        virtual bool canBecomeFocus() override { return true; }
    };

    TestApplication application;
    Window window{&application};
    window.open();
    FocusableView a;
    View b;
    FocusableView c;
    window.contentView()->addSubview(&a);
    a.addSubview(&b);
    b.addSubview(&c);

    c.focus();
    ASSERT_TRUE(a.isFocus());
    ASSERT_TRUE(b.isFocus());
    ASSERT_TRUE(c.isFocus());

    c.focusAncestor();
    EXPECT_TRUE(a.isFocus());
    EXPECT_FALSE(b.isFocus());
    EXPECT_FALSE(c.isFocus());
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
    okui::Window window(&application);
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

    EXPECT_FALSE(a.hasRelation(Relation::kAny, &b));
    EXPECT_FALSE(a.hasRelation(Relation::kHierarchy, &b));
    EXPECT_FALSE(a.hasRelation(Relation::kDescendant, &b));
    EXPECT_FALSE(a.hasRelation(Relation::kAncestor, &b));
    EXPECT_FALSE(a.hasRelation(Relation::kSibling, &b));

    a.addSubview(&b);
    EXPECT_TRUE(a.hasRelation(Relation::kAny, &b));
    EXPECT_TRUE(a.hasRelation(Relation::kHierarchy, &b));
    EXPECT_TRUE(b.hasRelation(Relation::kHierarchy, &a));
    EXPECT_FALSE(a.hasRelation(Relation::kDescendant, &b));
    EXPECT_TRUE(a.hasRelation(Relation::kAncestor, &b));
    EXPECT_TRUE(b.hasRelation(Relation::kDescendant, &a));
    EXPECT_FALSE(a.hasRelation(Relation::kSibling, &b));

    a.addSubview(&c);
    EXPECT_TRUE(c.hasRelation(Relation::kAny, &b));
    EXPECT_TRUE(c.hasRelation(Relation::kHierarchy, &b));
    EXPECT_FALSE(c.hasRelation(Relation::kDescendant, &b));
    EXPECT_FALSE(c.hasRelation(Relation::kAncestor, &b));
    EXPECT_FALSE(a.hasRelation(Relation::kSibling, &c));
    EXPECT_FALSE(c.hasRelation(Relation::kSibling, &a));
    EXPECT_TRUE(c.hasRelation(Relation::kSibling, &b));
    EXPECT_TRUE(b.hasRelation(Relation::kSibling, &c));
    EXPECT_FALSE(b.hasRelation(Relation::kSibling, &b));

    TestApplication application;
    okui::Window window(&application);
    window.open();
    window.contentView()->addSubview(&a);

    EXPECT_TRUE(a.hasRelation(Relation::kAny, &b));
    EXPECT_TRUE(a.hasRelation(Relation::kHierarchy, &b));
    EXPECT_TRUE(a.hasRelation(Relation::kAny, &c));
    EXPECT_TRUE(a.hasRelation(Relation::kHierarchy, &c));
    EXPECT_TRUE(b.hasRelation(Relation::kAny, &c));
    EXPECT_TRUE(b.hasRelation(Relation::kHierarchy, &c));

    okui::Window secondWindow(&application);
    secondWindow.open();
    secondWindow.contentView()->addSubview(&c);

    EXPECT_TRUE(a.hasRelation(Relation::kAny, &b));
    EXPECT_TRUE(a.hasRelation(Relation::kHierarchy, &b));
    EXPECT_TRUE(a.hasRelation(Relation::kAny, &c));
    EXPECT_FALSE(a.hasRelation(Relation::kHierarchy, &c));
    EXPECT_TRUE(b.hasRelation(Relation::kAny, &c));
    EXPECT_FALSE(b.hasRelation(Relation::kHierarchy, &c));
}
#endif // ONAIR_OKUI_HAS_NATIVE_APPLICATION

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION
TEST(View, messagePosting) {
    View a, b, c;

    int received = 0;

    b.listen([&] (const int& message) {
        EXPECT_EQ(message, 7);
        ++received;
    }, Relation::kAncestor);

    c.listen([&] (const int& message, View* sender) {
        EXPECT_EQ(sender, &a);
        EXPECT_EQ(message, 7);
        ++received;
    }, Relation::kAncestor);

    TestApplication application;
    okui::Window window(&application);
    window.open();
    window.contentView()->addSubview(&a);
    a.addSubview(&b);
    a.addSubview(&c);

    int message = 7;
    a.post(message, Relation::kDescendant);
    EXPECT_EQ(received, 2);

    a.listen([&] (const int& message, View* sender) {
        EXPECT_EQ(sender, &b);
        EXPECT_EQ(message, 5);
        ++received;
    }, Relation::kDescendant);

    b.post(5, Relation::kAncestor);
    EXPECT_EQ(received, 3);
}

TEST(View, messagePostingFromApplication) {
    View a, b;

    int received = 0;

    b.listen([&] (const int& message) {
        EXPECT_EQ(message, 7);
        ++received;
    });

    TestApplication application;
    okui::Window window(&application);
    window.open();
    window.contentView()->addSubview(&a);
    a.addSubview(&b);

    application.post(7);
    EXPECT_EQ(received, 1);
}
#endif // ONAIR_OKUI_HAS_NATIVE_APPLICATION

TEST(View, provisions) {
    View a, b, c;

    int n = 0;

    a.set(n);
    a.addSubview(&b);
    b.addSubview(&c);

    EXPECT_TRUE(a.get<int>());
    EXPECT_TRUE(b.inherit<int>());
    EXPECT_TRUE(c.inherit<int>());

    struct State {
        int x = 7;
    } state;

    a.set(state);

    auto attachedState = a.get<State>();
    ASSERT_TRUE(attachedState);
    EXPECT_EQ(attachedState->x, 7);

    attachedState->x = 8;
    auto attachedState2 = a.get<State>();
    ASSERT_TRUE(attachedState2);
    EXPECT_EQ(attachedState2->x, 8);
}

TEST(View, mouseMovement) {
    struct MouseMoveView : View {
        virtual void mouseMovement(double x, double y) override { mouseMoveHandled = true; }
        bool mouseMoveHandled = false;
    };

    MouseMoveView a, b, c;
    a.addSubview(&b);
    b.addSubview(&c);

    a.setBounds(0, 0, 100, 100);
    b.setBounds(20, 20, 20, 20);
    b.setClipsToBounds(false);
    c.setBounds(25, 25, 10, 10);

    a.dispatchMouseMovement(10, 10);
    EXPECT_TRUE(a.mouseMoveHandled);
    a.mouseMoveHandled = false;

    a.dispatchMouseMovement(20, 20);
    EXPECT_FALSE(a.mouseMoveHandled);
    EXPECT_TRUE(b.mouseMoveHandled);
    b.mouseMoveHandled = false;

    a.dispatchMouseMovement(45, 45);
    EXPECT_FALSE(a.mouseMoveHandled);
    EXPECT_FALSE(b.mouseMoveHandled);
    EXPECT_TRUE(c.mouseMoveHandled);
}

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION
TEST(View, focusOnRemoval) {
    struct FocusView : okui::View {
        virtual bool canBecomeFocus() override { return true; }
    };

    View a, b;
    FocusView c;

    TestApplication application;
    okui::Window window(&application);
    window.open();
    window.contentView()->addSubview(&a);

    a.addSubview(&b);
    a.setPreferredFocus(&b);
    b.addSubview(&c);
    c.focus();

    b.removeSubviews();

    EXPECT_EQ(window.focus(), nullptr);
}
#endif
