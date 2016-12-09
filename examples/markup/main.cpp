#include <okui/applications/Native.h>
#include <okui/applications/SDL.h>
#include <okui/FileResourceManager.h>

#include <okui/views/MarkupView.h>

#include <scraps/loggers.h>

class Application : public okui::applications::Native<okui::applications::SDL> {
public:
    Application() {
        setResourceManager(&_resourceManager);
    }

    virtual std::string name() const override { return "Markup"; }
    virtual std::string organization() const override { return "BitTorrent Inc."; }

private:
    #define INNER_C_STR(x) #x
    #define C_STR(x) INNER_C_STR(x)
    okui::FileResourceManager _resourceManager{C_STR(RESOURCE_PATH)};
};

int main() {
    scraps::SetLogger(std::make_shared<scraps::StandardLogger>());

    Application application;

    okui::Window window{&application};
    window.open();

    window.contentView()->setBackgroundColor(okui::Color::kBlack);

    okui::ml::Environment environment;

    okui::views::MarkupView view(&environment);
    view.define("state", "standby");

    if (auto markup = application.loadResource("View.xml")) {
        view.load(*markup);
    } else {
        SCRAPS_LOG_ERROR("unable to load markup xml");
    }
    window.contentView()->addSubview(&view);
    view.setBoundsRelative(0, 0, 1, 1);

    std::function<void()> cycle;
    cycle = [&] {
        view.asyncAfter(1s, [&] {
            view.define("state", "standby");
            view.define("state", "present");
        });
        view.asyncAfter(5s, [&] {
            view.define("state", "gone");
        });
        view.asyncAfter(6s, cycle);
    };
    view.async(cycle);

    application.run();

    return 0;
}
