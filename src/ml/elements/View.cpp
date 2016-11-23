#include <okui/ml/elements/View.h>

#include <okui/ml/Context.h>

namespace okui::ml::elements {

void View::ElementBase::initialize(const Context& context, const pugi::xml_node& xml) {
    _text = xml.text().get();

    for (auto& attribute : xml.attributes()) {
        _attributes[attribute.name()] = attribute.value();
    }

    for (auto& child : xml.children()) {
        if (child.type() == pugi::node_element) {
            auto element = context.load(child);
            if (!element) { continue; }
            if (view() && element->view()) {
                view()->addSubview(element->view());
            }
            _children.emplace_back(std::move(element));
        }
    }

    update(context);
}

void View::ElementBase::update(const Context& context) {
    for (auto& child : _children) {
        child->update(context);
    }

    for (auto& kv : _attributes) {
        setAttribute(context, kv.first, context.render(kv.second));
    }

    setText(context, context.render(_text));
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

} // namespace okui::ml::elements
