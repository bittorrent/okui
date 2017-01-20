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
#include <okui/ml/Context.h>
#include <okui/ml/Environment.h>

#include <cassert>

namespace okui::ml {

std::unique_ptr<ElementInterface> Context::load(stdts::string_view markup) const {
    pugi::xml_document document;
    auto result = document.load_buffer(markup.data(), markup.size());
    if (result) {
        return load(document.first_child());
    }
    SCRAPS_LOG_ERROR("error parsing markup: {}", result.description());
    return {};
}

std::unique_ptr<ElementInterface> Context::load(const pugi::xml_node& xml) const {
    auto element = _environment->createElement(xml.name());
    if (element) {
        element->initialize(this, xml);
    }
    return element;
}

std::string Context::render(stdts::string_view str) const {
    // based on http://stackoverflow.com/questions/39493542/building-a-dynamic-list-of-named-arguments-for-fmtlib
    std::vector<fmt::internal::NamedArg<char>> av;
    fmt::ULongLong types = 0;

    uint32_t i = 0;
    for (const auto& item : _fmtArgs) {
        // intentional pointer comparison - the FormatArgStorage objects can't be allowed to move around in memory after arg has been created
        assert(item.second->name.data() == item.second->arg.name.data());
        av.emplace_back(item.second->arg);
        if (i < fmt::ArgList::MAX_PACKED_ARGS) {
            types |= static_cast<uint64_t>(fmt::internal::Arg::NAMED_ARG) << (i * 4);
        }
        ++i;
    }

    try {
        if (_fmtArgs.size() >= fmt::ArgList::MAX_PACKED_ARGS) {
            std::vector<fmt::internal::Arg> avdata;
            avdata.resize(_fmtArgs.size() + 1);
            for (size_t i = 0; i < av.size(); ++i) {
                avdata[i].type = fmt::internal::Arg::NAMED_ARG;
                avdata[i].pointer = &av[i];
            }
            return fmt::format(str.data(), fmt::ArgList(types, &avdata[0]));
        }

        std::vector<fmt::internal::Value> avdata;
        avdata.resize(_fmtArgs.size());
        for (size_t i = 0; i < av.size(); ++i) {
            avdata[i].pointer = &av[i];
        }
        return fmt::format(str.data(), fmt::ArgList(types, avdata.data()));
    } catch (const fmt::FormatError& error) {
        return std::string(str);
    }
}

} // namespace okui::ml
