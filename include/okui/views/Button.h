#pragma once

#include <okui/config.h>

#include <okui/View.h>

#include <functional>

namespace okui {
namespace views {

template <typename BaseView>
class Button : public BaseView {
public:
    enum class State {
        kNormal,
        kDepressed
    };

    State state() const;

    void setAction(std::function<void()> action);
    void setAction(Command command, CommandContext context = {});

    void press();

    void setCanBecomeDirectFocus(bool canBecomeDirectFocus);
    virtual bool canBecomeDirectFocus() override;

    virtual void mouseDown(MouseButton button, double x, double y) override;
    virtual void mouseUp(MouseButton button, double startX, double startY, double x, double y) override;
    virtual void mouseDrag(double startX, double startY, double x, double y) override;
    virtual void mouseExit() override;
    virtual void keyDown(KeyCode key, KeyModifiers mod, bool repeat) override;

protected:
    /**
    * Subclasses can use this to update when their state changes.
    */
    virtual void stateChanged() {}

private:
    void _changeState(State state) {
        _state = state;
        stateChanged();
    }

    static constexpr auto kMouseThreshold = 10;

    std::function<void()> _action;
    bool                  _mouseDown = false;
    State                 _state = State::kNormal;
    bool                  _canBecomeDirectFocus = true;
};

template <typename BaseView>
typename Button<BaseView>::State Button<BaseView>::state() const {
    return _state;
}

template <typename BaseView>
void Button<BaseView>::setAction(std::function<void()> action) {
    _action = std::move(action);
}

template <typename BaseView>
void Button<BaseView>::setAction(Command command, CommandContext context) {
    _action = [=] {
        this->application()->command(command, context);
    };
}

template <typename BaseView>
void Button<BaseView>::press() {
    if (_action) {
        _action();
    }
}

template <typename BaseView>
void Button<BaseView>::setCanBecomeDirectFocus(bool canBecomeDirectFocus) {
    _canBecomeDirectFocus = canBecomeDirectFocus;
}

template <typename BaseView>
bool Button<BaseView>::canBecomeDirectFocus() {
    return _canBecomeDirectFocus;
}

template <typename BaseView>
void Button<BaseView>::mouseDown(MouseButton button, double x, double y) {
    _changeState(State::kDepressed);
    _mouseDown = true;
}

template <typename BaseView>
void Button<BaseView>::mouseUp(MouseButton button, double startX, double startY, double x, double y) {
    if (_state != State::kNormal) {
        _changeState(State::kNormal);
    }

    if (_mouseDown) {
        _mouseDown = false;
        if (_action) { _action(); }
    } else {
        BaseView::mouseUp(button, startX, startY, x, y);
    }
}

template <typename BaseView>
void Button<BaseView>::mouseDrag(double startX, double startY, double x, double y) {
    auto dx = std::abs(x - startX);
    auto dy = std::abs(y - startY);

    if (dx > kMouseThreshold || dy > kMouseThreshold) {
        _mouseDown = false;

        if (_state != State::kNormal) {
            _changeState(State::kNormal);
        }
    }

    BaseView::mouseDrag(startX, startY, x, y);
}

template <typename BaseView>
void Button<BaseView>::mouseExit() {
    _changeState(State::kNormal);
    _mouseDown = false;
}

template <typename BaseView>
void Button<BaseView>::keyDown(KeyCode key, KeyModifiers mod, bool repeat) {
    if (key == KeyCode::kSpace || key == KeyCode::kReturn || key == KeyCode::kSelect) {
        if (_action) { _action(); }
        return;
    }

    BaseView::keyDown(key, mod, repeat);
}

} } // namespace okui::views
