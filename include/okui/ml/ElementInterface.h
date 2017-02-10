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
        virtual void reset(std::string id) = 0;
        virtual bool hasState(stdts::string_view id) const = 0;
        virtual bool hasTrigger(stdts::string_view id) const = 0;
    };

    /**
    * If the element is a state machine, this will return an interface for it.
    */
    virtual StateMachineInterface* stateMachine() { return nullptr; }
};

} // namespace okui::ml
