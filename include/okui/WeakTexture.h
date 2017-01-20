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

#include <okui/TextureInterface.h>

namespace okui {

class WeakTexture : public TextureInterface {
public:
    void set(GLuint id = 0, int width = 0, int height = 0, bool hasPremultipliedAlpha = false) {
        _id = id;
        _width = width;
        _height = height;
        _hasPremultipliedAlpha = hasPremultipliedAlpha;
    }

    virtual int width() const override { return _width; }
    virtual int height() const override { return _height; }

    virtual GLuint id() const override { return _id; }

    virtual bool hasPremultipliedAlpha() const override { return _hasPremultipliedAlpha; }

private:
    int    _width = 0;
    int    _height = 0;
    GLuint _id = 0;
    bool   _hasPremultipliedAlpha{false};
};

} // namespace okui
