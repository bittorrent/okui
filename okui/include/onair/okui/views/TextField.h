#pragma once

#include "onair/okui/config.h"

#include "onair/okui/views/TextView.h"
#include "onair/okui/views/ScrollView.h"
#include "onair/okui/View.h"
#include "onair/okui/Application.h"
#include "onair/okui/Color.h"

#include "onair/Timer.h"

namespace onair {
namespace okui {
namespace views {

class TextField : public onair::okui::View {
public:
    struct SelectionRange {
        size_t index;
        size_t length;
    };

    using Action = std::function<void()>;

    TextField();

    /**
    * The change action is invoked any time the field's text is changed.
    */
    void setChangeAction(Action action) { _changeAction = std::move(action); }

    /**
    * The blur action is invoked any time the field loses focus.
    */
    void setBlurAction(Action action) { _blurAction = std::move(action); }

    /**
    * The return action is invoked any time the return key is hit while the field has focus.
    */
    void setReturnAction(Action action) { _returnAction = std::move(action); }

    const std::string& text() const { return _text; }
    void setText(const std::string& text);

    void setColors(const Color& highlightColor, const Color& cursorColor);

    /**
    * If the text is concealed, all characters are displayed as dots or some other uniform character.
    */
    void setConcealsText(bool concealsText = true);

    void setEnabled(bool enabled = true);
    bool isEnabled() const { return _isEnabled; }

    /**
    * Sets the padding for the text display.
    */
    void setPadding(double padding) { setPadding(padding, padding, padding, padding); }
    void setPadding(double left, double right, double top, double bottom);

    TextView& textView() { return _textView; }

    void selectRange(size_t index, size_t length);
    SelectionRange selectionRange() const;

    /**
    * Implement this to render the text field.
    *
    * Your implementation should render a background that fills the view's bounds.
    */
    virtual void render() override = 0;

    virtual void layout() override;

    virtual bool canBecomeFocus() override { return _isEnabled; }

    virtual void focusGained() override;
    virtual void focusLost() override;

    virtual void mouseDown(MouseButton button, double x, double y) override;
    virtual void mouseDrag(double startX, double startY, double x, double y) override;
    virtual void mouseEnter() override;
    virtual void mouseExit() override;
    virtual void textInput(const std::string& text) override;
    virtual void keyDown(KeyCode key, KeyModifiers mod, bool repeat) override;

    virtual bool canHandleCommand(Command command) override;
    virtual void handleCommand(Command command, CommandContext context) override;

private:
    struct SelectionHighlight : public onair::okui::View {
        virtual void render() override;
        Color color{1, 1, 1, 0.35};
    };

    struct Cursor : public onair::okui::View {
        virtual void render() override;
        virtual void appeared() override { timer.restart(); }
        Color color{1, 1, 1, 1};
        SteadyTimer timer;
    };

    void _textChanged(bool invokeAction = true);
    void _moveCursor(size_t pos, bool dragging = false);
    void _updateTextLayout();
    void _updateCursorLayout();

    std::string        _text;
    ScrollView         _scrollView;
    TextView           _textView;
    SelectionHighlight _selectionHighlight;
    Cursor             _cursor;
    Action             _returnAction;
    Action             _blurAction;
    Action             _changeAction;
    bool               _concealsText = false;
    bool               _isEnabled = true;
    double             _paddingLeft{0.0}, _paddingRight{0.0}, _paddingTop{0.0}, _paddingBottom{0.0};
    size_t             _cursorIndex = 0;
    size_t             _selectionStartIndex = 0;
};

}}}
