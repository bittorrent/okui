#pragma once

#include "okui/config.h"

#include <algorithm>
#include <cmath>

namespace okui {
namespace math {

template <typename T, size_t R, size_t C> struct Matrix;

template <typename T, size_t R, size_t C>
class MatrixBase {
public:
    MatrixBase() {}

    MatrixBase(std::initializer_list<T> l) {
        size_t r{0}, c{0};
        for (auto& v : l) {
            (*this)(r, c) = v;
            if (++c >= R) {
                c = 0;
                ++r;
            }
        }
    }

    T& operator()(size_t r, size_t c) { return _columnMajor[c * C + r]; }
    const T& operator()(size_t r, size_t c) const { return _columnMajor[c * C + r]; }

    template <size_t C2>
    Matrix<T, R, C2> operator*(const Matrix<T, C, C2>& right) const {
        Matrix<T, R, C2> ret;
        for (size_t i = 0; i < R; ++i) {
            for (size_t j = 0; j < C2; ++j) {
                T sum = 0;
                for (size_t k = 0; k < C; ++k) {
                    sum += (*this)(i, k) * right(k, j);
                }
                ret(i, j) = sum;
            }
        }
        return ret;
    }

    bool operator==(const Matrix<T, R, C>& other) const {
        for (size_t i = 0; i < R * C; ++i) {
            if (_columnMajor[i] != other._columnMajor[i]) {
                return false;
            }
        }
        return true;
    }

    const T* columnMajor() const {
        return _columnMajor;
    }

protected:
    T _columnMajor[R * C]{0};
};

#define ONAIR_OKUI_MATH_MATRIX_INHERITANCE(rows, cols) \
    Matrix() : MatrixBase<T, rows, cols>{} {} \
    template <typename... Args> \
    Matrix(Args&&... args) : MatrixBase<T, rows, cols>{std::forward<Args>(args)...} {}

template <typename T, size_t R, size_t C>
struct Matrix : MatrixBase<T, R, C> {
    ONAIR_OKUI_MATH_MATRIX_INHERITANCE(R, C)
};

template <typename T>
struct Matrix<T, 4, 4> : MatrixBase<T, 4, 4> {
    ONAIR_OKUI_MATH_MATRIX_INHERITANCE(4, 4)

    static Matrix Frustum(T left, T right, T bottom, T top, T near, T far) {
        Matrix ret;
        ret._columnMajor[ 0] = (2 * near) / (right - left);
        ret._columnMajor[ 5] = (2 * near) / (top - bottom);
        ret._columnMajor[ 8] = (right + left) / (right - left);
        ret._columnMajor[ 9] = (top + bottom) / (top - bottom);
        ret._columnMajor[10] = (far + near) / (far - near);
        ret._columnMajor[11] = -1;
        ret._columnMajor[14] = (2 * far * near) / (far - near);
        return ret;
    }

    static Matrix Perspective(T fovy, T aspect, T near, T far) {
        auto top = near * tan(fovy / 360.0 * M_PI);
        auto right = top * aspect;
        return Frustum(-right, right, -top, top, near, far);
    }

    static Matrix Translation(T x, T y, T z) {
        Matrix ret;
        ret._columnMajor[ 0] = 1;
        ret._columnMajor[ 5] = 1;
        ret._columnMajor[10] = 1;
        ret._columnMajor[12] = x;
        ret._columnMajor[13] = y;
        ret._columnMajor[14] = z;
        ret._columnMajor[15] = 1;
        return ret;
    }
};

} } // namespace okui::math
