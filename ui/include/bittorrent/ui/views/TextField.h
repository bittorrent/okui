#pragma once
#include "bittorrent/ui/config.h"
#include "bittorrent/ui/View.h"
#include "bittorrent/ui/views/TextView.h"
#include "bittorrent/ui/Color.h"

namespace bittorrent {
namespace ui {
namespace views {

class TextField : public bittorrent::ui::View {
public:
    using Action = std::function<void()>;

    TextField();

    void setChangeAction(Action action) { _changeAction = std::move(action); }
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
    virtual void keyDown(Keycode key, KeyModifiers mod, bool repeat) override;

private:
    std::string  _text;
    TextView     _textView;
    View         _textViewClip;
    Action       _returnAction;
    Action       _changeAction;
    bool         _concealsText = false;
    bool         _isEnabled = true;
    double       _paddingLeft{0.0}, _paddingRight{0.0}, _paddingTop{0.0}, _paddingBottom{0.0};
        
    void _updateTextLayout();
};

}}}
