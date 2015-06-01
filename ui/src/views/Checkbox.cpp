#include "bittorrent/ui/views/Checkbox.h"

namespace bittorrent {
namespace ui {
namespace views {

using namespace bittorrent;

void Checkbox::setState(State state, bool invokeAction) {
    _state = state;
    if (invokeAction && _action) {
        _action();
    }
}

void Checkbox::mouseDown(MouseButton button, int x, int y) {
    setState(_state == State::kChecked ? State::kUnchecked : State::kChecked, true);
}

void Checkbox::keyDown(KeyCode key, KeyModifiers mod, bool repeat) {
    if (key == KeyCode::kSpace) {
        setState(_state == State::kChecked ? State::kUnchecked : State::kChecked, true);
        return;
    }
    
    bittorrent::ui::View::keyDown(key, mod, repeat);
}

}}}