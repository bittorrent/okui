#pragma once
#include "onair/okui/config.h"
#include "onair/okui/View.h"
#include "onair/okui/views/TextView.h"
#include "onair/okui/Color.h"

namespace onair {
namespace okui {
namespace views {

class TextField : public onair::okui::View {
public:
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
    void setText(const char* text, bool invokeAction = false);
    
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

    virtual void mouseDown(MouseButton button, int x, int y) override;
    virtual void textInput(const std::string& text) override;
    virtual void keyDown(KeyCode key, KeyModifiers mod, bool repeat) override;

    virtual bool canHandleCommand(Command command) override;
    virtual void handleCommand(Command command, CommandContext context) override;

private:
    std::string  _text;
    TextView     _textView;
    View         _textViewClip;
    Action       _returnAction;
    Action       _blurAction;
    Action       _changeAction;
    bool         _concealsText = false;
    bool         _isEnabled = true;
    double       _paddingLeft{0.0}, _paddingRight{0.0}, _paddingTop{0.0}, _paddingBottom{0.0};
        
    void _updateTextLayout();
};

}}}
