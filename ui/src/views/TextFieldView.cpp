#include "bittorrent/ui/views/TextFieldView.h"
#include "bittorrent/ui/Application.h"
#include "bittorrent/ui/Platform.h"
#include "bittorrent/ui/shapes/Rectangle.h"
#include <algorithm>

namespace bittorrent {
namespace ui {
namespace views {

TextFieldView::TextFieldView() {
    _textView.setAlignment(TextView::HorizontalAlignment::kLeft, TextView::VerticalAlignment::kCenter);
    _textView.setTextColor(1, 1, 1, 1);
    _textView.setMultiLine(false);

    addSubview(&_textViewClip);
    _textViewClip.addSubview(&_textView);
}

void TextFieldView::mouseDown(MouseButton button, int x, int y) {
    focus();
}

void TextFieldView::focusGained() {
    application()->platform()->startTextInput();
}

void TextFieldView::focusLost() {
    application()->platform()->stopTextInput();
}

void TextFieldView::keyDown(Keycode key, KeyModifiers mod, bool repeat) {
    if (key == Keycode::kBackspace) {
        _text.pop_back();
        _textView.setText(_text.c_str());
        layout();
    } else if (key == Keycode::kReturn && _enterHandler) {
        _enterHandler(_text);
    } else if (mod & application()->platform()->defaultShortcutModifier()) {
        if (key == Keycode::kV) {
            textInput(application()->platform()->getClipboardText());
        } else if (key == Keycode::kC) {
            application()->platform()->setClipboardText(_text.c_str());
        }
    }
}

void TextFieldView::textInput(const std::string& text) {
    _text += text;
    _textView.setText(_text.c_str());
    layout();
}

void TextFieldView::layout() {
    _textViewClip.setBounds(0, 0, bounds().width, bounds().height);

    auto textWidth = static_cast<int>(std::ceil(_textView.textWidth()));
    _textView.setBounds(std::min(_textViewClip.bounds().width - textWidth, 0), 0, textWidth, _textViewClip.bounds().height);
}

}}}
