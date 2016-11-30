#pragma once

#include <okui/config.h>

#include <okui/View.h>
#include <okui/views/Button.h>
#include <okui/views/ImageView.h>
#include <okui/views/PopoutFocus.h>
#include <okui/views/TextView.h>

namespace okui::views {

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
    void setImage(std::string resourceOrURL, std::string placeholder = {}) { _body.setImage(std::move(resourceOrURL), std::move(placeholder)); }
    void setLabelStyle(TextView::Style focused) { setLabelStyle(focused, focused); }
    void setLabelStyle(TextView::Style focused, TextView::Style unfocused);
    void setLabelText(std::string labelText) { _label.setText(std::move(labelText)); }
    void setBodyAspectRatio(double aspectRatio) { _bodyAspectRatio = aspectRatio; layout(); }
    void setIconColor(Color c) { _body.setIconColor(std::move(c)); }

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
        void setImage(std::string resourceOrURL, std::string placeholder);
        void setIconColor(Color c) { _iconView.setTextureColor(std::move(c)); }

        virtual bool canBecomeDirectFocus() override { return true; }
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

} // namespace okui::views
