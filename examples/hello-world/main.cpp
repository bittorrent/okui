#include <okui/applications/Native.h>
#include <okui/applications/SDL.h>
#include <okui/FileResourceManager.h>

#include <okui/views/TextView.h>

#include <scraps/loggers.h>

class Application : public okui::applications::Native<okui::applications::SDL> {
public:
    Application() {
        setResourceManager(&_resourceManager);
    }

    virtual std::string name() const override { return "Hello, world!"; }
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

    window.contentView()->setBackgroundColor(okui::Color::kAqua);

    okui::views::TextView textView;
    textView.setFont("OpenSans-regular.png", "OpenSans-regular.fnt");
    textView.setTextSize(100);
    textView.setTextColor(okui::Color::kWhite);
    textView.setOutline(20);
    textView.setOutlineColor(okui::Color::kBlack);
    textView.setAlignment(okui::views::TextView::Style::HorizontalAlignment::kCenter);
    textView.setText("Hello, world!");
    window.contentView()->addSubview(&textView);
    textView.setBoundsRelative(0, 0, 1, 1);

    application.run();
    return 0;
}
