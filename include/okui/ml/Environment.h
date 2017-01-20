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

#include <okui/config.h>

#include <okui/ml/ElementInterface.h>
#include <okui/ml/ElementTypeInterface.h>

#include <stdts/string_view.h>

#include <unordered_map>

namespace okui::ml {

class Environment {
public:
    Environment();

    template <typename Interface>
    void defineElementType(stdts::string_view tag) {
        _elementTypes[std::string(tag)] = std::make_unique<Interface>();
    }

    std::unique_ptr<ElementInterface> createElement(stdts::string_view tag) {
        auto it = _elementTypes.find(std::string(tag));
        return it == _elementTypes.end() ? nullptr : it->second->create();
    }

private:
    std::unordered_map<std::string, std::unique_ptr<ElementTypeInterface>> _elementTypes;
};

} // namespace okui::ml
