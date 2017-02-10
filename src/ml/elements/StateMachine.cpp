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
#include <okui/ml/elements/StateMachine.h>

#include <okui/ml/Context.h>
#include <okui/ml/parsing.h>

namespace okui::ml::elements {

void StateMachine::Element::initialize(const Context* context, const pugi::xml_node& xml) {
    _context = context;

    for (auto& attribute : xml.attributes()) {
        if (attribute.name() == "id"s) {
            _idAttribute = attribute.value();
            _id = _context->render(_idAttribute);
        } else if (attribute.name() == "state"s) {
            _stateAttribute = attribute.value();
            _stateMachine = std::make_unique<StateMachine>(_context->render(*_stateAttribute));
        }
    }

    bool triggerStateMachine = !_stateMachine;

    for (auto& child : xml.children()) {
        if (child.type() != pugi::node_element) { continue; }

        if (child.name() == "state"s) {
            std::string id;
            std::unordered_map<std::string, std::string> attributes;
            for (auto& attribute : child.attributes()) {
                if (attribute.name() == "id"s) {
                    if (triggerStateMachine) {
                        _stateMachine.reset();
                        return;
                    }
                    id = attribute.value();
                } else if (attribute.name() == "trigger"s) {
                    if (!triggerStateMachine) {
                        _stateMachine.reset();
                        return;
                    }
                    id = attribute.value();
                } else {
                    attributes[attribute.name()] = attribute.value();
                }
            }

            if (!_stateMachine) {
                _stateMachine = std::make_unique<StateMachine>(id, true);
            }

            _stateMachine->stateDefinitions[std::move(id)].attributes = std::move(attributes);
        } else if (child.name() == "transition"s) {
            auto fromAttribute = child.attribute("from");
            auto toAttribute = child.attribute("to");
            auto durationAttribute = child.attribute("duration");
            auto interpolationAttribute = child.attribute("interpolation");
            if (toAttribute && fromAttribute && durationAttribute && interpolationAttribute) {
                auto duration = ParseDuration(durationAttribute.value());
                auto interpolation = ParseInterpolator<double>(interpolationAttribute.value());
                if (duration && interpolation) {
                    auto delayAttribute = child.attribute("delay");
                    auto delay = delayAttribute ? ParseDuration(delayAttribute.value()).value_or(0s) : 0s;

                    if (!_stateMachine) {
                        _stateMachine = std::make_unique<StateMachine>(fromAttribute.value(), true);
                    }

                    _stateMachine->setTransition(
                        fromAttribute.value(),
                        toAttribute.value(),
                        std::chrono::duration_cast<Animation<double>::Duration>(*duration),
                        interpolation,
                        std::chrono::duration_cast<Animation<double>::Duration>(delay)
                    );
                }
            }
        }
    }
}

void StateMachine::Element::update() {
    if (_stateMachine && _stateAttribute) {
        _stateMachine->setState(_context->render(*_stateAttribute));
    }
}

void StateMachine::Element::StateMachine::update(stdts::string_view id, State& state, bool& isAnimated) {
    auto& def = stateDefinitions[id.to_string()];
    for (auto& kv : def.attributes) {
        auto& v = state.attributes.values[kv.first];
        v.clear();
        StateMachineDelegate::ValueComponent component;
        component.value = kv.second;
        component.coefficient = 1.0;
        v.emplace_back(std::move(component));
    }
}

void StateMachine::Element::StateMachine::apply(State state) {
    if (!delegate) { return; }
    for (auto& kv : state.attributes.values) {
        if (!kv.second.empty()) {
            delegate->apply(kv.first, std::move(kv.second));
        }
    }
}

} // namespace okui::ml::elements
