/**
* Copyright 2017 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
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
