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

    /**
    * @param context must outlive the element
    */
    virtual void initialize(const Context* context, const pugi::xml_node& xml) {}
    virtual void update() {}

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

    class StateMachineDelegate {
    public:
        virtual ~StateMachineDelegate() {}

        struct ValueComponent {
            stdts::string_view value;
            double coefficient = 0.0;
        };

        virtual void apply(stdts::string_view attribute, std::vector<ValueComponent> components) = 0;
    };

    class StateMachineInterface {
    public:
        virtual void setDelegate(StateMachineDelegate* delegate) = 0;
        virtual bool drive() = 0;
        virtual std::string state() const = 0;
        virtual void setState(std::string id) = 0;
    };

    /**
    * If the element is a state machine, this will return an interface for it.
    */
    virtual StateMachineInterface* stateMachine() { return nullptr; }
};

} // namespace okui::ml
