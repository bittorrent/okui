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

#include <okui/opengl/opengl.h>
#include <okui/opengl/TextureCache.h>

namespace okui {

class TextureInterface {
public:
    virtual ~TextureInterface() {}

    virtual bool hasMetadata() const { return isLoaded(); }

    /**
    * Return the dimensions of the texture. Available only when hasMetadata returns true.
    *
    * Return 0 if the image data could not be parsed.
    */
    virtual int width() const = 0;
    virtual int height() const = 0;

    /**
    * Returns the aspect ratio (width / height) of the texture. Available only when hasMetadata returns true.
    */
    double aspectRatio() const { return (double)width() / height(); }

    /**
    * Requires the render context to be active.
    *
    * Loads the texture to the GPU if necessary.
    *
    * This object can be deleted at any time, regardless of whether the render context is still active.
    */
    virtual void load() {}

    virtual bool isLoaded() const { return id(); }

    virtual bool hasPremultipliedAlpha() const { return false; }

    virtual int allocatedWidth() const { return width(); }
    virtual int allocatedHeight() const { return height(); }

    /**
    * If loaded, returns the id of the GPU texture. Otherwise, returns 0.
    */
    virtual GLuint id() const = 0;
};

} // namespace okui
