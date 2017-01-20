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
#include <okui/ml/elements/View.h>

#include <okui/ml/Context.h>

namespace okui::ml::elements {

void View::ElementBase::initialize(const Context* context, const pugi::xml_node& xml) {
    _context = context;
    _text = xml.text().get();

    for (auto& attribute : xml.attributes()) {
        _attributes[attribute.name()] = attribute.value();
    }

    for (auto& child : xml.children()) {
        if (child.type() == pugi::node_element) {
            auto element = _context->load(child);
            if (!element) { continue; }
            if (view() && element->view()) {
                view()->addSubview(element->view());
            } else if (auto stateMachine = element->stateMachine()) {
                stateMachine->setDelegate(this);
                _stateMachines.emplace_back(stateMachine);
            }
            _children.emplace_back(std::move(element));
        }
    }

    update();
}

void View::ElementBase::update() {
    for (auto& kv : _attributes) {
        setAttribute(kv.first, _context->render(kv.second));
    }

    for (auto& child : _children) {
        child->update();
    }

    if (!_stateMachines.empty() && view()) {
        view()->addUpdateHook("ElementBase", [&]{
            bool done = true;
            for (auto& stateMachine : _stateMachines) {
                if (stateMachine->drive()) {
                    done = false;
                }
            }
            if (done) {
                view()->removeUpdateHook("ElementBase");
            }
        });
    }

    setText(_context->render(_text));
}

const char* View::ElementBase::id() const {
    auto it = _attributes.find("id");
    return it == _attributes.end() ? nullptr : it->second.c_str();
}

ElementInterface* View::ElementBase::descendantWithId(stdts::string_view id) const {
    for (auto& child : _children) {
        if (child->id() && id == child->id()) {
            return child.get();
        } else if (auto descendant = child->descendantWithId(id)) {
            return descendant;
        }
    }
    return nullptr;
}

void View::ElementBase::apply(stdts::string_view attribute, std::vector<View::ElementBase::ValueComponent> components) {
    std::vector<std::string> strings;
    strings.resize(components.size());
    size_t i = 0;
    for (auto& component : components) {
        component.value = strings[i++] = _context->render(component.value);
    }
    if (components.size() == 1) {
        setAttribute(attribute, std::move(components[0].value));
    } else {
        setAttribute(attribute, std::move(components));
    }
}

stdts::optional<Color> View::ElementBase::SumColorComponents(const std::vector<ValueComponent>& components) {
    if (components.size() == 1) { return ParseColor(components[0].value); }

    auto sum = Color::kTransparentBlack;
    for (auto& c : components) {
        auto v = ParseColor(c.value);
        if (!v) { return {}; }
        sum += *v * c.coefficient;
    }
    return sum;
}

stdts::optional<std::string> View::ElementBase::SumExpressionComponents(const std::vector<ValueComponent>& components) {
    if (components.size() == 1) { return std::string{components[0].value}; }

    std::string sum = "";
    for (auto& c : components) {
        if (!sum.empty()) {
            sum += '+';
        }
        sum += '(';
        sum.append(c.value.data(), c.value.size());
        sum += ")*";
        sum += std::to_string(c.coefficient);
    }
    return sum;
}

stdts::optional<double> View::ElementBase::SumNumberComponents(const std::vector<ValueComponent>& components) {
    if (components.size() == 1) { return ParseNumber(components[0].value); }

    double sum = 0.0;
    for (auto& c : components) {
        auto v = ParseNumber(c.value);
        if (!v) { return {}; }
        sum += *v * c.coefficient;
    }
    return sum;
}

} // namespace okui::ml::elements
