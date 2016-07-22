#pragma once

#include "onair/okui/View.h"
#include "onair/okui/views/Button.h"
#include "onair/okui/views/ImageView.h"
#include "onair/okui/views/PopoutFocus.h"
#include "onair/okui/views/TextView.h"

namespace onair {
namespace okui {
namespace views {

/**
* A popout button with a graphic and a label underneath.
*
* The bounds of this view will fully contain the graphic and the label in the focused state.
*/
class LabeledPopoutButton : public Button<View> {
public:
    LabeledPopoutButton();
    ~LabeledPopoutButton() { removeSubviews(); }

    void setScaling(double scaling) { _scaling = scaling; _body.setScaling(scaling); layout(); }
    void setBackgroundColor(Color backgroundColor) { _body.setBackgroundColor(std::move(backgroundColor)); }
    void setIcon(std::string sdf) { _body.setIcon(std::move(sdf)); }
    void setImageURL(std::string url, std::string placeholder) { _body.setImageURL(std::move(url), std::move(placeholder)); }
    void setLabelStyle(TextView::Style focused) { setLabelStyle(focused, focused); }
    void setLabelStyle(TextView::Style focused, TextView::Style unfocused);
    void setLabelText(std::string labelText) { _label.setText(std::move(labelText)); }
    void setBodyAspectRatio(double aspectRatio) { _bodyAspectRatio = aspectRatio; layout(); }

    virtual void focusGained() override;
    virtual void focusLost() override;
    virtual void layout() override;

private:
    double _scaling = 0.2;
    double _bodyAspectRatio = 16.0 / 9.0;

    class Body : public PopoutFocus<View> {
    public:
        ~Body() { removeSubviews(); }

        void setBackgroundColor(Color backgroundColor);
        void setIcon(std::string sdf);
        void setImageURL(std::string url, std::string placeholder);

        virtual void render() override;
        virtual void layout() override;

    private:
        Color _backgroundColor = Color::kTransparentBlack;
        ImageView _imageView;
        ImageView _iconView;
    } _body;

    TextView::Style _focusedLabelStyle;
    TextView::Style _unfocusedLabelStyle;
    TextView _label;
};

}}}
