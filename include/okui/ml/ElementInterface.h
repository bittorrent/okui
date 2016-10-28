#pragma once

#include <pugixml.hpp>

namespace okui {
class View;

namespace ml {
class Context;

class ElementInterface {
public:
    virtual ~ElementInterface() {}

    virtual void initialize(const Context& context, const pugi::xml_node& xml) {}
    virtual void update(const Context& context) {}

    /**
    * If the element represents a view, this will return it.
    */
    virtual View* view() { return nullptr; }
};

}} // namespace okui::ml
