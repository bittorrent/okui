#include "onair/okui/views/TextField.h"

#include "onair/okui/Application.h"
#include "onair/okui/Platform.h"
#include "onair/okui/shapes/Rectangle.h"

#include <algorithm>

namespace onair {
namespace okui {
namespace views {

TextField::TextField() {
    _textView.setAlignment(TextView::HorizontalAlignment::kLeft, TextView::VerticalAlignment::kCenter);
    _textView.setTextColor(1, 1, 1, 1);
    _textView.setMultiLine(false);

    addSubview(&_scrollView);
    _scrollView.contentView()->addSubview(&_selectionHighlight);
    _selectionHighlight.setIsVisible(false);
    _scrollView.contentView()->addSubview(&_textView);
    _textView.addSubview(&_cursor);
    _cursor.setIsVisible(false);
}

void TextField::setText(const std::string& text) {
    _text = text;
    _textChanged();
}

void TextField::setColors(const Color& highlightColor, const Color& cursorColor) {
    _selectionHighlight.color = highlightColor;
    _cursor.color = cursorColor;
}

void TextField::setConcealsText(bool concealsText) {
    if (_concealsText == concealsText) { return; }

    _concealsText = concealsText;
    _textChanged(false);
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

void TextField::selectRange(size_t index, size_t length) {
    _selectionStartIndex = index;
    _moveCursor(index + length, true);
}

TextField::SelectionRange TextField::selectionRange() const {
    if (_selectionStartIndex  < _cursorIndex) {
        return {_selectionStartIndex, _cursorIndex-_selectionStartIndex};
    }
    return {_cursorIndex, _selectionStartIndex-_cursorIndex};
}

void TextField::mouseDown(MouseButton button, int x, int y) {
    if (_isEnabled && button == MouseButton::kLeft) {
        focus();
        auto localPt = _scrollView.contentView()->superviewToLocal(_scrollView.superviewToLocal(x, y));
        _moveCursor(_textView.lineColumnAtPosition(localPt.x, localPt.y).second);
    }
}

void TextField::mouseDrag(int x, int y) {
    if (_isEnabled) {
        auto localPt = _scrollView.contentView()->superviewToLocal(_scrollView.superviewToLocal(x, y));
        _moveCursor(_textView.lineColumnAtPosition(localPt.x, localPt.y).second, true);
    }
}

void TextField::mouseEnter() {
    application()->platform()->setCursorType(CursorType::kText);
}

void TextField::mouseExit() {
    application()->platform()->setCursorType(CursorType::kArrow);
}

void TextField::focusGained() {
    application()->platform()->startTextInput();
    _cursor.setIsVisible(true);
}

void TextField::focusLost() {
    application()->platform()->stopTextInput();
    _selectionHighlight.setIsVisible(false);
    _cursor.setIsVisible(false);
    if (_blurAction) {
        _blurAction();
    }
}

void TextField::keyDown(KeyCode key, KeyModifiers mod, bool repeat) {
    switch(key) {
        case KeyCode::kBackspace: {
            auto range = selectionRange();
            if (range.length > 0) {
                _text.erase(range.index, range.length);
                _textChanged();
                _moveCursor(range.index);
            } else if (range.index > 0) {
                _text.erase(range.index-1, 1);
                _textChanged();
                _moveCursor(range.index-1);
            }
            break;
        }
        case KeyCode::kReturn: {
            if (_returnAction) {
                _returnAction();
            }
            break;
        }
        case KeyCode::kLeft: {
            bool shortcut = mod & application()->platform()->defaultShortcutModifier();
            bool dragging = mod & KeyModifier::kShift;
            auto range    = selectionRange();
            if      (shortcut)                      { _moveCursor(0, dragging); }
            else if (range.length > 0 && !dragging) { _moveCursor(range.index); }
            else if (_cursorIndex > 0)              { _moveCursor(_cursorIndex-1, dragging); }
            break;
        }
        case KeyCode::kRight: {
            bool shortcut = mod & application()->platform()->defaultShortcutModifier();
            bool dragging = mod & KeyModifier::kShift;
            auto range    = selectionRange();
            if      (shortcut)                      { _moveCursor(_text.size(), dragging); }
            else if (range.length > 0 && !dragging) { _moveCursor(range.index + range.length); }
            else if (_cursorIndex < _text.size())   { _moveCursor(_cursorIndex+1, dragging); }
            break;
        }
        default: {
            // pass the event up the chain if it's not input
            if (key < KeyCode::kSpace || key > KeyCode::kZ) {
                View::keyDown(key, mod, repeat);
            }
            break;
        }
    }
}

bool TextField::canHandleCommand(Command command) {
    return command == kCommandCopy ||
           command == kCommandPaste ||
           command == kCommandSelectAll ||
           View::canHandleCommand(command);
}

void TextField::handleCommand(Command command, CommandContext context) {
    switch(command) {
        case kCommandCopy: {
            auto range = selectionRange();
            if (range.length > 0) {
                application()->platform()->setClipboardText(_text.substr(range.index, range.length).c_str());
            }
            break;
        }
        case kCommandPaste: {
            textInput(application()->platform()->getClipboardText());
            break;
        }
        case kCommandSelectAll: {
            _selectionStartIndex = 0;
            _moveCursor(_text.length(), true);
            break;
        }
        default: {
            View::handleCommand(command, context);
            break;
        }
    }
}

void TextField::textInput(const std::string& text) {
    auto range = selectionRange();
    _text.replace(range.index, range.length, text);
    _textChanged();
    _moveCursor(range.index + text.length());
}

void TextField::layout() {
    _updateTextLayout();
    _updateCursorLayout();
}

void TextField::_moveCursor(size_t pos, bool dragging) {
    if (!dragging) {
        _selectionStartIndex = pos;
    }
    _cursorIndex = pos;
    _cursor.timer.restart();
    _updateCursorLayout();
}

void TextField::_textChanged(bool invokeAction) {
    _textView.setText(_concealsText ? std::string(_text.size(), '*').c_str() : _text);
    _updateTextLayout();
    if (invokeAction && _changeAction) {
        _changeAction();
    }
}

void TextField::_updateTextLayout() {
    _scrollView.setBounds(_paddingLeft, _paddingTop, bounds().width - _paddingLeft - _paddingRight, bounds().height - _paddingTop - _paddingBottom);

    auto textWidth = std::ceil(_textView.textWidth());

    _textView.setBounds(0, 0, textWidth+1, _scrollView.bounds().height);
    _scrollView.contentView()->setBounds(_scrollView.contentView()->bounds().x, 0, textWidth+1, _scrollView.bounds().height);
}

void TextField::_updateCursorLayout() {
    auto lineHeight     = std::ceil(_textView.lineHeight());
    auto range          = selectionRange();
    auto cursorPosition = _textView.lineColumnPosition(0, range.index);

    //  Display the selection highlight if there is a selection:
    if (range.length > 0) {
        auto start = cursorPosition;
        auto end   = _textView.lineColumnPosition(0, range.index + range.length);
        _selectionHighlight.setBounds(start.x, start.y, end.x-start.x+1, lineHeight);
        _selectionHighlight.setIsVisible(true);

        if (_cursorIndex != range.index) {
            cursorPosition = end;
        }
    } else {
        _selectionHighlight.setIsVisible(false);
    }

    //  Update the cursor position:
    _cursor.setBounds(cursorPosition.x, cursorPosition.y, 1, lineHeight);

    //  Shift the text view to keep the cursor in view:
    auto scrollContentX = _scrollView.contentView()->bounds().x;
    cursorPosition.x += scrollContentX;

    if (cursorPosition.x < 0) {
        _scrollView.setContentOffset(scrollContentX - cursorPosition.x, 0);
    } else if (cursorPosition.x >= _scrollView.bounds().width) {
        _scrollView.setContentOffset(scrollContentX - cursorPosition.x + _scrollView.bounds().width - 1, 0);
    }
}

void TextField::SelectionHighlight::render() {
    auto colorShader = this->colorShader();

    colorShader->setColor(color.r, color.g, color.b, color.a);

    okui::shapes::Rectangle(0, 0, bounds().width, bounds().height).round(3).draw(colorShader);

    colorShader->flush();
}

void TextField::Cursor::render() {
    auto colorShader = this->colorShader();

    auto s     = std::fmod(std::chrono::duration<double>(timer.elapsed()).count(), 1.1);
    auto alpha = 1.0;

         if (s < 0.55) { alpha = (0.10 - (s - 0.45))/0.10; }
    else if (s < 0.95) { alpha = 0; }
    else if (s < 1.10) { alpha = (s - 0.95)/0.15; }

    colorShader->setColor(color.r, color.g, color.b, alpha);

    okui::shapes::Rectangle(0, 0, bounds().width, bounds().height).draw(colorShader);

    colorShader->flush();
}

}}}
