#pragma once

#include "TestApplication.h"
#include "onair/okui/Window.h"

#if ONAIR_OKUI_HAS_NATIVE_APPLICATION

inline void RenderOnce(std::function<void(onair::okui::View* view)> init, std::function<void(onair::okui::View* view)> render) {
    TestApplication application;

    bool didRender = false;

    struct RenderView : onair::okui::View {
        RenderView(bool* didRender, std::function<void(onair::okui::View* view)> init, std::function<void(onair::okui::View* view)> render)
            : didRender(didRender), initFunction(init), renderFunction(render) {}

        virtual void render() override {
            if (!*didRender) {
                renderFunction(this);
            }

            *didRender = true;
            application()->quit();
        }

        virtual void windowChanged() override {
            EXPECT_TRUE(window());

            if (initFunction) {
                initFunction(this);
            }
        }

        bool* didRender = nullptr;
        std::function<void(onair::okui::View* view)> initFunction;
        std::function<void(onair::okui::View* view)> renderFunction;
    };

    struct RenderWindow : onair::okui::Window {
        RenderWindow(onair::okui::Application* application, bool* didRender, std::function<void(onair::okui::View* view)> init, std::function<void(onair::okui::View* view)> render)
            : Window(application), view(didRender, init, render)
        {}

        virtual void layout() override {
            contentView()->addSubview(&view);
            view.setBounds(0, 0, contentView()->bounds().width, contentView()->bounds().height);
        }

        RenderView view;
    } window(&application, &didRender, init, render);

    window.open();

    application.run();

    EXPECT_TRUE(didRender);
}

inline void RenderOnce(std::function<void(onair::okui::View* view)> render) {
    return RenderOnce(std::function<void(onair::okui::View* view)>(), render);
}

#endif // ONAIR_OKUI_HAS_NATIVE_APPLICATION
