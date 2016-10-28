#pragma once

#include <okui/config.h>

#include <okui/ml/ElementInterface.h>
#include <okui/ml/ElementTypeInterface.h>

#include <stdts/string_view.h>

#include <unordered_map>

namespace okui {
namespace ml {

class Environment {
public:
    Environment();

    template <typename Interface>
    void defineElementType(stdts::string_view tag) {
        _elementTypes[tag.to_string()] = std::make_unique<Interface>();
    }

    std::unique_ptr<ElementInterface> createElement(stdts::string_view tag) {
        auto it = _elementTypes.find(tag.to_string());
        return it == _elementTypes.end() ? nullptr : it->second->create();
    }

private:
    std::unordered_map<std::string, std::unique_ptr<ElementTypeInterface>> _elementTypes;
};

}} // namespace okui::ml
