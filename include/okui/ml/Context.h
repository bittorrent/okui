#pragma once

#include <okui/config.h>

#include <okui/ml/Environment.h>

#include <stdts/string_view.h>

#include <unordered_map>

namespace okui {
namespace ml {

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
    void define(std::string name, T value) {
        auto it = _fmtArgs.find(name);
        if (it != _fmtArgs.end()) {
            it->second = fmt::internal::NamedArg<char>(fmt::BasicStringRef<char>(name.data(), name.size()), value);
            return;
        }
        _fmtArgs.emplace(name, fmt::internal::NamedArg<char>(fmt::BasicStringRef<char>(name.data(), name.size()), value));
    }

    /**
    * Render a string, substituting defined variables where needed.
    */
    std::string render(stdts::string_view str) const;

private:
    Environment* const _environment;
    std::unordered_map<std::string, fmt::internal::NamedArg<char>> _fmtArgs;
};

}} // namespace okui::ml
