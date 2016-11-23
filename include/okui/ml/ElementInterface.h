#pragma once

#include <pugixml.hpp>

#include <stdts/string_view.h>

namespace okui {
class View;
} // namespace okui

namespace okui::ml {
class Context;

class ElementInterface {
public:
    virtual ~ElementInterface() {}

    virtual void initialize(const Context& context, const pugi::xml_node& xml) {}
    virtual void update(const Context& context) {}

    /**
    * Returns the element's id if it has one.
    */
    virtual const char* id() const = 0;

    /**
    * Returns the child with the given id if it exists.
    */
    virtual ElementInterface* descendantWithId(stdts::string_view id) const { return nullptr; }

    /**
    * If the element represents a view, this will return it.
    */
    virtual View* view() { return nullptr; }
};

} // namespace okui::ml
