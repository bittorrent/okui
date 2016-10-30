#include <okui/ml/elements/View.h>

#include <okui/ml/Context.h>

namespace okui {
namespace ml {
namespace elements {

void View::ElementBase::initialize(const Context& context, const pugi::xml_node& xml) {
    _text = xml.text().get();

    for (auto& attribute : xml.attributes()) {
        _attributes[attribute.name()] = attribute.value();
    }

    update(context);

    for (auto& child : xml.children()) {
        if (child.type() == pugi::node_element) {
            auto element = context.load(child);
            if (view() && element->view()) {
                view()->addSubview(element->view());
            }
            _children.emplace_back(std::move(element));
        }
    }
}

void View::ElementBase::update(const Context& context) {
    for (auto& kv : _attributes) {
        setAttribute(kv.first, context.render(kv.second));
    }

    setText(context.render(_text));

    for (auto& child : _children) {
        child->update(context);
    }
}

}}} // namespace okui::ml::elements
