#pragma once

#include "onair/okui/config.h"

#include <cmath>

namespace onair {
namespace okui {
namespace math {

// arrays are column major

template <typename T, int R, int C> struct Matrix;

template <typename T, int R, int C>
struct MatrixBase {
	T m[R * C]{0};
		
	T& operator()(int r, int c) { return m[c * C + r]; }
	const T& operator()(int r, int c) const { return m[c * C + r]; }

	template <int C2>
	Matrix<T, R, C2> operator*(const Matrix<T, C, C2>& right) const {
		Matrix<T, R, C2> ret;
		for (int i = 0; i < R; ++i) {
			for (int j = 0; j < C2; ++j) {
				T sum = 0;
				for (int k = 0; k < C; ++k) {
					sum += (*this)(i, k) * right(k, j);
				}
				ret(i, j) = sum;
			}
		}
		return ret;
	}
};

template <typename T, int R, int C>
struct Matrix : MatrixBase<T, R, C> {};

template <typename T>
struct Matrix<T, 4, 4> : MatrixBase<T, 4, 4> {
	using MatrixBase<T, 4, 4>::m;
	
	static Matrix Frustum(T left, T right, T bottom, T top, T near, T far) {
		Matrix ret;
		ret.m[ 0] = (2 * near) / (right - left);
		ret.m[ 5] = (2 * near) / (top - bottom);
		ret.m[ 8] = (right + left) / (right - left);
		ret.m[ 9] = (top + bottom) / (top - bottom);
		ret.m[10] = (far + near) / (far - near);
		ret.m[11] = -1;
		ret.m[14] = (2 * far * near) / (far - near);
		return ret;
	}

	static Matrix Perspective(T fovy, T aspect, T near, T far) {
		auto top = near * tan(fovy / 360.0 * M_PI);
		auto right = top * aspect;
		return Frustum(-right, right, -top, top, near, far);
	}

	static Matrix Translation(T x, T y, T z) {
		Matrix ret;
		ret.m[ 0] = 1;
		ret.m[ 5] = 1;
		ret.m[10] = 1;
		ret.m[12] = x;
		ret.m[13] = y;
		ret.m[14] = z;
		ret.m[15] = 1;
		return ret;
	}
};

}}}
