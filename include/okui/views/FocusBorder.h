#pragma once

#include <okui/config.h>

#include <okui/Animation.h>
#include <okui/Point.h>
#include <okui/shapes/Rectangle.h>

namespace okui::views {

/**
* FocusBorder renders a border around its bounds of a specified color and thickness when
* focused. It can be used as a subview of a class which should be focused.
*/
template <typename BaseView>
class FocusBorder : public BaseView {
public:
    template <typename... Args>
    FocusBorder(Args&&... args);

    virtual void focusGained() override;
    virtual void focusLost() override;
    virtual void disappeared() override;
    virtual void layout() override;

    void setColor(okui::Color color)    { _borderView.color = color;}
    void setThickness(double thickness) { _borderView.thickness = thickness; }

private:
    struct BorderView : public View {
        virtual void render() override;

        Animation<double> anim{0.0};
        okui::Color       color = okui::Color::kWhite;
        double            thickness = 1;
    };

    BorderView _borderView;
};

template <typename BaseView>
template <typename... Args>
FocusBorder<BaseView>::FocusBorder(Args&&... args)
    : BaseView{std::forward<Args>(args)...}
{
    this->addSubview(&_borderView);
    _borderView.bringToFront();
}

template <typename BaseView>
void FocusBorder<BaseView>::focusGained() {
    _borderView.anim.target(1.0, 100ms, interpolation::Quadratic::EaseOut);
    _borderView.bringToFront();
    BaseView::focusGained();
}

template <typename BaseView>
void FocusBorder<BaseView>::focusLost() {
    _borderView.anim.target(0.0, 300ms, interpolation::Quadratic::EaseOut);
    BaseView::focusLost();
}

template <typename BaseView>
void FocusBorder<BaseView>::disappeared() {
    _borderView.anim.reset(0.0);
    BaseView::disappeared();
}

template <typename BaseView>
void FocusBorder<BaseView>::layout() {
    _borderView.setBoundsRelative(0, 0, 1, 1);
    BaseView::layout();
}

template <typename BaseView>
void FocusBorder<BaseView>::BorderView::render() {
    auto colorShader = this->colorShader();

    colorShader->setColor(color.withAlphaF(color.alphaF() * anim.current()));
    okui::shapes::Rectangle(0, 0, this->bounds().width, thickness).draw(colorShader);
    okui::shapes::Rectangle(0, thickness, thickness, this->bounds().height).draw(colorShader);
    okui::shapes::Rectangle(this->bounds().width-thickness, thickness, thickness, this->bounds().height).draw(colorShader);
    okui::shapes::Rectangle(0, this->bounds().height-thickness, this->bounds().width, thickness).draw(colorShader);

    colorShader->flush();
}

} // namespace okui::views
