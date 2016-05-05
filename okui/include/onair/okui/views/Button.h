#pragma once

#include "onair/okui/config.h"

#include "onair/okui/views/ImageView.h"

#include <functional>
#include <unordered_map>

namespace onair {
namespace okui {
namespace views {

class Button : public View {
public:
    enum class State {
        kNormal,
        kDepressed
    };

    State state() const { return _state; }
    TextureHandle& texture(State state = State::kNormal) { return _imageViews[state].texture(); }

    void setAction(std::function<void()> action);
    void setAction(Command command, CommandContext context);

    void setTextureResource(std::string resource, State state = State::kNormal);
    void setTextureFromURL(std::string url, State state = State::kNormal);
    void setTextureFromURL(std::string url, std::string placeholderResource, State state = State::kNormal);
    void setTextureColor(Color color, State state = State::kNormal);
    void setTextureDistanceField(double edge = 0.5, State state = State::kNormal);

    void press();

    virtual bool canBecomeFocus() override { return true; }
    virtual void mouseDown(MouseButton button, double x, double y) override;
    virtual void mouseUp(MouseButton button, double startX, double startY, double x, double y) override;
    virtual void mouseExit() override;
    virtual void keyDown(KeyCode key, KeyModifiers mod, bool repeat) override;
    virtual void layout() override;

private:
    ImageView& _stateImageView(State state);
    void _changeState(State state);

    State                                _state = State::kNormal;
    std::unordered_map<State, ImageView> _imageViews;
    Color                                _normalColor = {1, 1, 1, 1};
    std::function<void()>                _action;
    bool                                 _mouseDown = false;
};

}}}
