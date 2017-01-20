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

#include <cmath>

namespace okui {

class AffineTransformation {
public:
    /**
    * @param tx|ty translations to make after rotation and scaling
    * @param txT|tyT translations to make before rotation and scaling
    * @param sx|sy scale factors
    * @param r rotation in radians
    */
    AffineTransformation(double tx = 0.0, double ty = 0.0, double txT = 0.0, double tyT = 0.0, double sx = 1.0, double sy = 1.0, double r = 0.0)
        : _sxCosR(sx * cos(r)), _sySinR(sy * sin(r)), _sxSinR(sx * sin(r)), _syCosR(sy * cos(r))
        , _txF(tx + txT * _sxCosR - tyT * _sySinR), _tyF(ty + txT * _sxSinR + tyT * _syCosR)
    {}

    template <typename T>
    void transform(T x, T y, T* xOut, T* yOut) const {
        *xOut = x * _sxCosR - y * _sySinR + _txF;
        *yOut = x * _sxSinR + y * _syCosR + _tyF;
    }

    static AffineTransformation Translation(double x, double y) {
        return AffineTransformation(x, y);
    }

    static AffineTransformation Rotation(double r) {
        return AffineTransformation(0.0, 0.0, 0.0, 0.0, 1.0, 1.0, r);
    }

    static AffineTransformation Scale(double x, double y) {
        return AffineTransformation(0.0, 0.0, 0.0, 0.0, x, y);
    }

private:
    double _sxCosR, _sySinR, _sxSinR, _syCosR, _txF, _tyF;
};

} // namespace okui
