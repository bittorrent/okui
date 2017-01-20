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
#include <okui/views/MarkupView.h>

#include <scraps/net/HTTPRequest.h>

#include <cassert>

namespace okui::views {

void MarkupView::load(stdts::string_view markup) {
    setPreferredFocus(nullptr);
    _element = _context.load(markup);
    if (_element && _element->view()) {
        addSubview(_element->view());
        setPreferredFocus(_element->view());
        layout();
    }
}

void MarkupView::stream(stdts::string_view uri) {
    _lastStreamResponse = {};
    _streamURI = std::string(uri);
    _sendStreamRequest();
    addUpdateHook("MarkupView::_streamUpdateHook", [&] { _streamUpdateHook(); });
}

void MarkupView::layout() {
    if (_element && _element->view()) {
        _element->view()->setBounds(0, 0, bounds().width, bounds().height);
    }
}

View* MarkupView::descendantViewWithId(stdts::string_view id) {
    if (_element) {
        auto e = _element->descendantWithId(id);
        if (e) {
            return e->view();
        }
    }

    return nullptr;
}

void MarkupView::_sendStreamRequest() {
    assert(_streamURI);
    _lastStreamRequestTime = std::chrono::steady_clock::now();
    _pendingStreamResponse = std::async(std::launch::async, [=, uri = _streamURI, lastStreamResponse = _lastStreamResponse] {
        std::vector<std::string> headers;
        if (lastStreamResponse && lastStreamResponse->etag) {
            headers.emplace_back("If-None-Match: "s + *lastStreamResponse->etag);
            headers.emplace_back("X-Wait-If-Match: true");
        }
        scraps::net::HTTPRequest request(*uri, nullptr, 0, headers);
        request.wait();

        StreamResponse ret;
        if (!request.error()) {
            auto etags = request.responseHeaders("ETag");
            if (etags.size() == 1) {
                ret.etag = etags[0];
            }
            if (request.responseStatus() == 200) {
                ret.markup = request.responseBody();
            }
        }
        return ret;
    });
}

void MarkupView::_streamUpdateHook() {
    if (!_pendingStreamResponse.valid()) {
        if (_streamURI && (!_lastStreamRequestTime || *_lastStreamRequestTime + 5s < std::chrono::steady_clock::now())) {
            _sendStreamRequest();
        }
        return;
    }

    if (_pendingStreamResponse.wait_for(0s) == std::future_status::ready) {
        auto response = _pendingStreamResponse.get();
        if (response.markup) {
            if (!_lastStreamResponse || !(response.markup == _lastStreamResponse->markup)) {
                load(*response.markup);
                if (response.etag) {
                    // we want to reconnect immediately
                    _lastStreamRequestTime = {};
                }
            }
            _lastStreamResponse = response;
        }
    }
}

} // namespace okui::views
