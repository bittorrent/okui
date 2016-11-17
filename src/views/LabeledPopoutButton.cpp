#include <okui/views/LabeledPopoutButton.h>

#include <okui/Application.h>
#include <okui/shapes/Rectangle.h>

using namespace std::literals;

namespace okui {
namespace views {

LabeledPopoutButton::LabeledPopoutButton() {
    // in case the user wants text to extend beyond the bounds
    setClipsToBounds(false);

    _body.setScaling(_scaling);
    setPreferredFocus(&_body);

    _label.setClipsToBounds(false);
    _label.setAlignment(TextView::Style::HorizontalAlignment::kCenter);
    _label.setOverflowBehavior(TextView::Style::OverflowBehavior::kWrap);

    addSubview(&_body);
    addSubview(&_label);
}

void LabeledPopoutButton::setLabelStyle(TextView::Style focused, TextView::Style unfocused) {
    _focusedLabelStyle = std::move(focused);
    _unfocusedLabelStyle = std::move(unfocused);
    _label.setStyle(isFocus() ? _focusedLabelStyle : _unfocusedLabelStyle);
    layout();
}

void LabeledPopoutButton::focusGained() {
    _label.setStyle(_focusedLabelStyle);
    addUpdateHook("popout animation tracking", [this]{ layout(); });
}

void LabeledPopoutButton::focusLost() {
    _label.setStyle(_unfocusedLabelStyle);
    addUpdateHook("popout animation tracking", [this]{ layout(); });
}

void LabeledPopoutButton::layout() {
    auto bodyWidth = bounds().width / (1.0 + _scaling);
    auto bodyHeight = bodyWidth / _bodyAspectRatio;
    _body.setSize(bodyWidth, bodyHeight);
    auto focusedBodyHeight = bodyHeight * (1.0 + _scaling);
    _body.setAnchor({bounds().width * 0.5, focusedBodyHeight * 0.5}, {0.5, 0.5});

    auto bodyBottom = _body.bounds().y + _body.bounds().height;
    _label.setBounds(0, bodyBottom, bounds().width, bounds().height - bodyBottom);
    if (_body.popoutAnimation().current() == _body.popoutAnimation().target()) {
        removeUpdateHook("popout animation tracking");
    }
}

void LabeledPopoutButton::Body::setBackgroundColor(Color backgroundColor) {
    _backgroundColor = std::move(backgroundColor);
    invalidateRenderCache();
}

void LabeledPopoutButton::Body::setImage(std::string resourceOrURL, std::string placeholder) {
    _imageView.setTexture(std::move(resourceOrURL));
    _imageView.setTexturePlaceholder(std::move(placeholder));
    addSubview(&_imageView);
    _imageView.sendToBack();
}

void LabeledPopoutButton::Body::setIcon(std::string sdf) {
    _iconView.setTexture(std::move(sdf));
    _iconView.setTextureDistanceField();
    _iconView.setTextureColor(Color::kWhite.withAlphaF(0.8));
    addSubview(&_iconView);
    _iconView.bringToFront();
}

void LabeledPopoutButton::Body::render() {
    auto colorShader = this->colorShader();
    colorShader->setColor(_backgroundColor);
    shapes::Rectangle(0, 0, bounds().width, bounds().height).round(3).draw(colorShader);
    colorShader->flush();
}

void LabeledPopoutButton::Body::layout() {
    _imageView.setBounds(0, 0, bounds().width, bounds().height);
    _iconView.setBounds(0, 0, bounds().width, bounds().height);
}

} } // namespace okui::views
