#pragma once

#include <okui/config.h>

#include <okui/ml/Environment.h>

#include <stdts/any.h>
#include <stdts/optional.h>
#include <stdts/string_view.h>

#include <unordered_map>

namespace okui::ml {

/**
* A context contains the essentials for loading and updating views. For example, one of its
* responsibilities is storing the values of the variables used in the markup.
*/
class Context {
public:
    explicit Context(Environment* environment) : _environment{environment} {}

    /**
    * Loads the given markup, or returns null on error.
    *
    * Element construction is extremely permissive, following the HTML spirit of forward-compatibility.
    * If the markup is valid XML, an element is *always* returned, even if there are unrecognized
    * attributes or tags.
    */
    std::unique_ptr<ElementInterface> load(stdts::string_view markup) const;
    std::unique_ptr<ElementInterface> load(const pugi::xml_node& element) const;

    /**
    * Defines a variable. Variables can be used in attribute values and text via named fmtlib arguments.
    */
    template <typename T>
    void define(std::string name, T&& value) {
        _fmtArgs[name] = std::make_unique<FormatArgStorage>(std::move(name), std::forward<T>(value));
    }

    const stdts::any* get(const std::string& name) const {
        auto it = _fmtArgs.find(name);
        if (it == _fmtArgs.end()) { return nullptr; }
        return &it->second->value;
    }

    /**
    * Render a string, substituting defined variables where needed.
    */
    std::string render(stdts::string_view str) const;

private:
    Environment* const _environment;

    struct FormatArgStorage {
        FormatArgStorage(std::string name, const char* value)
            : name(std::move(name)), value(std::string(value)), arg(fmt::BasicStringRef<char>(this->name.data(), this->name.size()), *stdts::any_cast<std::string>(&this->value)) {}

        template <typename T>
        FormatArgStorage(std::string name, T&& value)
            : name(std::move(name)), value(std::forward<T>(value)), arg(fmt::BasicStringRef<char>(this->name.data(), this->name.size()), *stdts::any_cast<std::decay_t<T>>(&this->value)) {}

        std::string name;
        stdts::any value;
        fmt::internal::NamedArg<char> arg;
    };

    std::unordered_map<std::string, std::unique_ptr<FormatArgStorage>> _fmtArgs;
};

} // namespace okui::ml
