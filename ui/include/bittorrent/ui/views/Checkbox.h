#pragma once

#include "bittorrent/ui/View.h"

namespace bittorrent {
namespace ui {
namespace views {

class Checkbox : public View {
public:
    enum class State {
        kChecked,
        kUnchecked,
    };

    State state() const { return _state; }
    void setState(State state, bool invokeAction = false);

    void setAction(std::function<void()> action) { _action = action; }

    virtual void render() = 0;

    virtual bool canBecomeFocus() override { return true; }
    virtual void mouseDown(MouseButton button, int x, int y) override;
    virtual void keyDown(KeyCode key, KeyModifiers mod, bool repeat) override;

private:
    State _state = State::kUnchecked;
    std::function<void()> _action;
};

}}}