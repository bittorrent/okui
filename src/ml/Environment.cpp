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
#include <okui/ml/Environment.h>

#include <okui/ml/elements/Button.h>
#include <okui/ml/elements/FocusBorder.h>
#include <okui/ml/elements/Image.h>
#include <okui/ml/elements/LabeledPopoutButton.h>
#include <okui/ml/elements/Popout.h>
#include <okui/ml/elements/StateMachine.h>
#include <okui/ml/elements/Text.h>
#include <okui/ml/elements/View.h>

namespace okui::ml {

Environment::Environment() {
    defineElementType<elements::Button>("button");
    defineElementType<elements::FocusBorder>("focusborder");
    defineElementType<elements::Image>("image");
    defineElementType<elements::LabeledPopoutButton>("labeledpopoutbutton");
    defineElementType<elements::Popout>("popout");
    defineElementType<elements::StateMachine>("statemachine");
    defineElementType<elements::Text>("text");
    defineElementType<elements::View>("view");
}

} // namespace okui::ml
