#include <okui/views/Checkbox.h>

namespace okui {
namespace views {

void Checkbox::setState(State state, bool invokeAction) {
    _state = state;
    if (invokeAction && _action) {
        _action();
    }
}

void Checkbox::mouseDown(MouseButton button, double x, double y) {
    setState(_state == State::kChecked ? State::kUnchecked : State::kChecked, true);
}

void Checkbox::keyDown(KeyCode key, KeyModifiers mod, bool repeat) {
    if (key == KeyCode::kSpace) {
        setState(_state == State::kChecked ? State::kUnchecked : State::kChecked, true);
        return;
    }

    okui::View::keyDown(key, mod, repeat);
}

} } // namespace okui::views
