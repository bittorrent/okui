#include <okui/applications/Native.h>
#include <okui/applications/SDL.h>
#include <okui/StateMachine.h>

class Application : public okui::applications::Native<okui::applications::SDL> {
public:
    virtual std::string name() const override { return "StateMachine Example"; }
    virtual std::string organization() const override { return "BitTorrent Inc."; }
};

struct State {
    double x, y, opacity;
    constexpr auto members() { return std::tie(x, y, opacity); }
};

class View : public okui::View, public okui::StateMachine<State> {
public:
    View() : okui::StateMachine<State>("Above") {
        addUpdateHook("StateMachine", [&]{ drive(); });
        setTransition("Above", "Normal", 200ms, okui::interpolation::Quadratic::EaseOut);
        setTransition("Normal", "Left", 200ms, okui::interpolation::Quadratic::EaseIn);
    }

private:
    virtual void update(stdts::string_view id, State& state, bool& isAnimated) override {
        state.opacity = id == "Normal" ? 1.0 : 0.0;
        state.x = id == "Left" ? -100.0 : 300.0;
        state.y = id == "Above" ? -100.0 : 300.0;
    }

    virtual void apply(const State& state) override {
        setOpacity(state.opacity);
        setBounds(state.x, state.y, 100.0, 100.0);
    }
};

int main() {
    Application application;

    okui::Window window{&application};
    window.open();

    View view;
    view.setBackgroundColor(okui::Color::kBlue);
    window.contentView()->addSubview(&view);

    view.asyncAfter(2s, [&] { view.setState("Normal"); });
    view.asyncAfter(4s, [&] { view.setState("Left"); });

    application.run();
    return 0;
}
