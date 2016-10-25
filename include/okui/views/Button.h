#pragma once

#include "okui/config.h"

#include "okui/View.h"

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

    State state() const                                          { return _state; }

    void setAction(std::function<void()> action)                 { _action = std::move(action); }
    void setAction(Command command, CommandContext context = {}) { _action = [=] { this->application()->command(command, context); }; }

    void press()                                                 { if (_action) { _action(); } }

    void setCanBecomeFocused(bool canBecomeFocused)              { _canBecomeFocused = canBecomeFocused; }
    virtual bool canBecomeFocus() override                       { return _canBecomeFocused; }

    virtual void mouseDown(MouseButton button, double x, double y) override;
    virtual void mouseUp(MouseButton button, double startX, double startY, double x, double y) override;
    virtual void mouseExit() override;
    virtual void keyDown(KeyCode key, KeyModifiers mod, bool repeat) override;

protected:
    /**
    * Subclasses can use this to update when their state changes.
    */
    virtual void stateChanged() {}

private:
    std::function<void()> _action;
    bool                  _mouseDown = false;
    State                 _state = State::kNormal;
    bool                  _canBecomeFocused = true;

    void _changeState(State state) {
        _state = state;
        stateChanged();
    }
};

template <typename BaseView>
void Button<BaseView>::mouseDown(MouseButton button, double x, double y) {
    _changeState(State::kDepressed);
    _mouseDown = true;
}

template <typename BaseView>
void Button<BaseView>::mouseUp(MouseButton button, double startX, double startY, double x, double y) {
    auto dx = std::abs(x - startX);
    auto dy = std::abs(y - startY);

    if (_mouseDown && dx < this->size().x/2 && dy < this->size().y/2) {
        _mouseDown = false;
        _changeState(State::kNormal);
        if (_action) { _action(); }
    } else {
        BaseView::mouseUp(button, startX, startY, x, y);
    }
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