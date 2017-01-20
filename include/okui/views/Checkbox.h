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

#include <okui/View.h>

namespace okui::views {

class Checkbox : public View {
public:
    enum class State {
        kChecked,
        kUnchecked,
    };

    State state() const { return _state; }
    void setState(State state, bool invokeAction = false);

    void setAction(std::function<void()> action) { _action = action; }

    virtual void render() override = 0;

    virtual bool canBecomeDirectFocus() override { return true; }
    virtual void mouseDown(MouseButton button, double x, double y) override;
    virtual void keyDown(KeyCode key, KeyModifiers mod, bool repeat) override;

private:
    State _state = State::kUnchecked;
    std::function<void()> _action;
};

} // namespace okui::views
