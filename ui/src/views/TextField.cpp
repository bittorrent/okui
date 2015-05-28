#include "bittorrent/ui/views/TextField.h"

#include "bittorrent/ui/Application.h"
#include "bittorrent/ui/Platform.h"
#include "bittorrent/ui/shapes/Rectangle.h"

#include <algorithm>

namespace bittorrent {
namespace ui {
namespace views {

TextField::TextField() {
    _textView.setAlignment(TextView::HorizontalAlignment::kLeft, TextView::VerticalAlignment::kCenter);
    _textView.setTextColor(1, 1, 1, 1);
    _textView.setMultiLine(false);

    addSubview(&_textViewClip);
    _textViewClip.addSubview(&_textView);
}

void TextField::setText(const char* text, bool invokeAction) {
    _text = text;
    _textView.setText(_concealsText ? std::string(_text.size(), '*').c_str() : text);
    _updateTextLayout();
    if (invokeAction && _changeAction) {
        _changeAction();
    }
}

void TextField::setConcealsText(bool concealsText) {
    if (_concealsText == concealsText) { return; }

    _concealsText = concealsText;
    setText(_text.c_str());
}

void TextField::setEnabled(bool enabled) {
    _isEnabled = enabled;
    
    if (!enabled) {
        unfocus();
    }
}

void TextField::setPadding(double left, double right, double top, double bottom) {
    _paddingLeft = left;
    _paddingRight = right;
    _paddingTop = top;
    _paddingBottom = bottom;
    _updateTextLayout();
}

void TextField::mouseDown(MouseButton button, int x, int y) {
    if (_isEnabled) {
        focus();
    }
}

void TextField::focusGained() {
    application()->platform()->startTextInput();
}

void TextField::focusLost() {
    application()->platform()->stopTextInput();
    if (_blurAction) {
        _blurAction();
    }
}

void TextField::keyDown(Keycode key, KeyModifiers mod, bool repeat) {
    if (key == Keycode::kBackspace) {
        if (!_text.empty()) {
            setText(_text.substr(0, _text.size() - 1).c_str(), true);
        }
    } else if (key == Keycode::kReturn && _returnAction) {
        _returnAction();
    } else if (key == Keycode::kV && (mod & application()->platform()->defaultShortcutModifier())) {
        textInput(application()->platform()->getClipboardText());
    } else if (key == Keycode::kC && (mod & application()->platform()->defaultShortcutModifier())) {
        application()->platform()->setClipboardText(_text.c_str());
    } else {
        // pass the event up the chain
        View::keyDown(key, mod, repeat);
    }
}

void TextField::textInput(const std::string& text) {
    setText((_text + text).c_str(), true);
}

void TextField::layout() {
    _updateTextLayout();
}

void TextField::_updateTextLayout() {
    _textViewClip.setBounds(_paddingLeft, _paddingBottom, bounds().width - _paddingLeft - _paddingRight, bounds().height - _paddingTop - _paddingBottom);

    auto textWidth = static_cast<int>(std::ceil(_textView.textWidth()));
    _textView.setBounds(std::min(_textViewClip.bounds().width - textWidth, 0), 0, textWidth, _textViewClip.bounds().height);
}

}}}
