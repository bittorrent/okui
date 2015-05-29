#pragma once

#include "TestApplication.h"
#include "bittorrent/ui/Window.h"

inline void RenderOnce(std::function<void(bittorrent::ui::View* view)> init, std::function<void(bittorrent::ui::View* view)> render) {
    TestApplication application;

    bool didRender = false;

    struct RenderView : bittorrent::ui::View {
        RenderView(bool* didRender, std::function<void(bittorrent::ui::View* view)> init, std::function<void(bittorrent::ui::View* view)> render)
            : didRender(didRender), initFunction(init), renderFunction(render) {}
        
        virtual void render() override {
            renderFunction(this);
            
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
        std::function<void(bittorrent::ui::View* view)> initFunction;
        std::function<void(bittorrent::ui::View* view)> renderFunction;
    };

    struct RenderWindow : bittorrent::ui::Window {
        RenderWindow(bittorrent::ui::Application* application, bool* didRender, std::function<void(bittorrent::ui::View* view)> init, std::function<void(bittorrent::ui::View* view)> render)
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

inline void RenderOnce(std::function<void(bittorrent::ui::View* view)> render) {
    return RenderOnce(std::function<void(bittorrent::ui::View* view)>(), render);
}