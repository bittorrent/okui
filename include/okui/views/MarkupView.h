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

#include <okui/View.h>
#include <okui/ml/Context.h>
#include <okui/ml/Environment.h>

namespace okui::views {

class MarkupView : public View {
public:
    MarkupView(ml::Environment* environment) : _environment{environment} {}

    void load(stdts::string_view markup);

    /**
    * Loads and updates markup provided by the given URI.
    *
    * For example, if an HTTP URL is given, the view will continuously poll the endpoint for updated
    * markup.
    *
    * The request will include an X-Wait-If-Match header set to true. This is to indicate to the
    * server that if the ETag matches, it can hold the connection open until it becomes outdated,
    * facilitating real-time streaming.
    */
    void stream(stdts::string_view uri);

    template <typename T>
    void define(std::string name, T&& value) {
        _context.define(std::move(name), std::forward<T>(value));
        if (_element) {
            _element->update();
        }
    }

    virtual void layout() override;

    ml::ElementInterface* element() const { return _element.get(); }

    View* descendantViewWithId(stdts::string_view id);

private:
    ml::Environment* const _environment;
    ml::Context _context{_environment};

    struct StreamResponse {
        stdts::optional<std::string> etag;
        stdts::optional<std::string> markup;
    };

    stdts::optional<std::string> _streamURI;
    stdts::optional<StreamResponse> _lastStreamResponse;
    std::future<StreamResponse> _pendingStreamResponse;
    stdts::optional<std::chrono::steady_clock::time_point> _lastStreamRequestTime;

    std::unique_ptr<ml::ElementInterface> _element;

    void _sendStreamRequest();
    void _streamUpdateHook();
};

} // namespace okui::views
