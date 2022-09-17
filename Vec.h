#pragma once

#include <cmath>
#include <cassert>
#include <cstdarg>
#include <string>

#ifndef PI
    #define PI 3.141592653589793238462643383279502884f
#endif


/// Utility class to represent an n-component vector
template<int N, typename T = float>
class Vec {

	T components[N];

	// Variadic component setter - base case
	template<typename T0>
	void setComponents(int idx, T0 first) {
		set(idx, (T)first);
		// Any missing arguments are 0-initialized
		for (int i = idx + 1; i < N; ++i) {
			set(i, (T)0);
		}
	}

	// Variadic component setter - recursive case
	template<typename T0, typename... Ts>
	void setComponents(int idx, T0 first, Ts... args) {
		set(idx, (T)first);
		++idx;
		if (idx < N) {
			setComponents(idx, args...);
		}
	}

public:


	/// Default constructor. Does not initialize any values.
	inline Vec() {}

	// Component constructors
	// Variadic params for ease of use - should pass at most N components, rest will be discarded.
	// Any components not passed are initialized to 0.
	template<typename T0, typename... Ts>
	inline Vec(T0 x, Ts... args) {
		setComponents(0, x, args...);
	}

	static inline Vec<N, T> Zero() {
		Vec<N, T> zero;
		for (int i = 0; i < N; ++i) zero.set(i, (T)0);
		return zero;
	}

	static inline Vec<N, T> One() {
		Vec<N, T> one;
		for (int i = 0; i < N; ++i) one.set(i, (T)1);
		return one;
	}



	// Getters - includes standards for X, Y, Z, W
	inline T get (const int& i) const { assert(N > i); return components[i]; }
	inline T operator[](const int& i) const { return get(i); }
	inline T X () const { return get(0); }
	inline T Y () const { return get(1); }
	inline T Z () const { return get(2); }
	inline T W () const { return get(3); }

	// Swizzles (2- and 3-components only; all possible permutations of x, y, z, w)
#define SWIZ2(a, b)			inline Vec<2, T> a ## b				() const { return Vec<2, T>( a(), b() ); }
#define SWIZ3(a, b, c)		inline Vec<3, T> a ## b ## c		() const { return Vec<3, T>(a(), b(), c()); }
	// .x, .xx, .xxx
#define SWIZZLE(a) SWIZ2(a, a) SWIZ3(a, a, a)
	SWIZZLE(X) SWIZZLE(Y) SWIZZLE(Z) SWIZZLE(W)
#undef SWIZZLE
	// .xy, .yx, .xxy, .xyx, .yxx, .xyy, .yxy, .yyx
#define SWIZZLE(a, b) SWIZ2(a, b) SWIZ2(b, a) SWIZ3(a, a, b) SWIZ3(a, b, a) SWIZ3(b, a, a) SWIZ3(a, b, b) SWIZ3(b, a, b) SWIZ3(b, b, a)
	SWIZZLE(X, Y) SWIZZLE(X, Z) SWIZZLE(X, W) SWIZZLE(Y, Z) SWIZZLE(Y, W) SWIZZLE(Z, W)
#undef SWIZZLE
	// .xyz, .xzy, .yxz, .zxy, .yzx, .zyx
#define SWIZZLE(a, b, c) SWIZ3(a, b, c) SWIZ3(a, c, b) SWIZ3(b, a, c) SWIZ3(c, a, b) SWIZ3(b, c, a) SWIZ3(c, b, a)
	SWIZZLE(X, Y, Z) SWIZZLE(X, Y, W) SWIZZLE(X, Z, W) SWIZZLE(Y, Z, W)
#undef SWIZZLE
#undef SWIZ2
#undef SWIZ3

	// Setters
	inline T set (const int& i, const T& val) { assert(N > i); return components[i] = val; }
	inline T setX (const T& val) { return set(0, val); }
	inline T setY (const T& val) { return set(1, val); }
	inline T setZ (const T& val) { return set(2, val); }
	inline T setW (const T& val) { return set(3, val); }

	// Swizzle setters (2- and 3-components only; all possible permutations of x, y, z, w)
#define SWIZ2(a, b)			inline void set ## a ## b		(const Vec<2, T>& v)	{ set ## a (v[0]); set ## b (v[1]); }
#define SWIZ3(a, b, c)		inline void set ## a ## b ## c	(const Vec<3, T>& v)	{ set ## a (v[0]); set ## b (v[1]); set ## c (v[2]); }
	// .x, .xx, .xxx
#define SWIZZLE(a) SWIZ2(a, a) SWIZ3(a, a, a)
	SWIZZLE(X) SWIZZLE(Y) SWIZZLE(Z) SWIZZLE(W)
#undef SWIZZLE
	// .xy, .yx, .xxy, .xyx, .yxx, .xyy, .yxy, .yyx
#define SWIZZLE(a, b) SWIZ2(a, b) SWIZ2(b, a) SWIZ3(a, a, b) SWIZ3(a, b, a) SWIZ3(b, a, a) SWIZ3(a, b, b) SWIZ3(b, a, b) SWIZ3(b, b, a)
	SWIZZLE(X, Y) SWIZZLE(X, Z) SWIZZLE(X, W) SWIZZLE(Y, Z) SWIZZLE(Y, W) SWIZZLE(Z, W)
#undef SWIZZLE
	// .xyz, .xzy, .yxz, .zxy, .yzx, .zyx
#define SWIZZLE(a, b, c) SWIZ3(a, b, c) SWIZ3(a, c, b) SWIZ3(b, a, c) SWIZ3(c, a, b) SWIZ3(b, c, a) SWIZ3(c, b, a)
	SWIZZLE(X, Y, Z) SWIZZLE(X, Y, W) SWIZZLE(X, Z, W) SWIZZLE(Y, Z, W)
#undef SWIZZLE
#undef SWIZ2
#undef SWIZ3


	/// Vector length squared
	inline T lengthSqr() const {
		T val = 0;
		for (int i = 0; i < N; ++i) val += get(i) * get(i);
		return val;
	}

	/// Normalized vector
	inline Vec<N, T> normalized() const {
		T length = (T)sqrt((T)lengthSqr());
		Vec<N, T> ret;
		for (int i = 0; i < N; ++i) ret.set(i, length > (T) 0 ? get(i) / length : (T)0);
		return ret;
	}
	inline void normalize() {
		T length = (T)sqrt((T)lengthSqr());
		if (length > (T)0) {
			for (int i = 0; i < N; ++i) set(i, get(i) / length);
		}
	}



	/// Standard operations

	/// Component-wise addition & subtraction
	inline Vec<N, T> operator+(const Vec<N, T>& v) const {
		Vec<N, T> ret;
		for (int i = 0; i < N; ++i) ret.set(i, v[i] + get(i));
		return ret;
	}

	inline Vec<N, T> operator+=(const Vec<N, T>& v) {
		Vec<N, T> n = (*this) + v;
		for (int i = 0; i < N; ++i) set(i, n[i]);
		return *this;
	}

	inline void add(const T& f) {
		for (int i = 0; i < N; ++i) set(i, get(i) + f);
	}

	inline void add(const Vec<N, T>& v) {
		for (int i = 0; i < N; ++i) set(i, get(i) + v[i]);
	}

	inline Vec<N, T> operator-(const Vec<N, T>& v) const {
		Vec<N, T> ret;
		for (int i = 0; i < N; ++i) ret.set(i, get(i) - v[i]);
		return ret;
	}

	inline Vec<N, T> operator-=(const Vec<N, T>& v) {
		Vec<N, T> n = (*this) - v;
		for (int i = 0; i < N; ++i) set(i, n[i]);
		return *this;
	}

	/// Product
	inline Vec<N, T> operator*(const T& f) const {
		Vec<N, T> ret;
		for (int i = 0; i < N; ++i) ret.set(i, get(i) * f);
		return ret;
	}

	inline void multiply(const T& f) {
		for (int i = 0; i < N; ++i) set(i, get(i) * f);
	}

	/// Dot product
	inline T dot(const Vec<N, T>& v) const {
		T result = 0;
		for (int i = 0; i < N; ++i) {
			result += get(i) * v.get(i);
		}
		return result;
	}
	
	/// Hadamard product
	inline Vec<N, T> hadamard(const Vec<N, T>& v) const {
		Vec<N, T> result;
		for (int i = 0; i < N; ++i) {
			result.set(i, get(i) * v.get(i));
		}
		return result;
	}


	/// Cast to int vector
	inline Vec<N, int> floor() const {
		Vec<N, int> ret;
		for (int i = 0; i < N; ++i) ret.set(i, (int)get(i));
		return ret;
	}

	
	/// Convert to contiguous index
	/// i.e. [ 0, 0, 0 ] -> 0, [ 1, 1, 1 ] -> 1 + width + width * height, [ 0, 2 ] -> 2 * width, etc.
	inline int index(const int& range) const {
		int idx = 0;
		for (int i = 0; i < N; ++i) {
			idx += int(get(i) * std::pow(range, i));
		}
		return idx;
	}


	/// Linear interpolation
	inline static Vec<N, T> Lerp(const Vec<N, T>& a, const Vec<N, T>& b, float t) {
		Vec<N, T> ret;
		for (int i = 0; i < N; ++i) ret.set(i, (T)(a.get(i) * (1.0 - t) + b.get(i) * t));
		return ret;
	}


	/// Component-wise clamp
	inline void clamp(const T& a, const T& b) {
		for (int i = 0; i < N; ++i) {
			if (get(i) < a) set(i, a);
			else if (get(i) > b) set(i, b);
		}
	}


	/// Assuming two Vecs representing positions in space, moves this vec towards the position of the other one, with a clamp on how far the vec may move at once
	inline void moveTowards(const Vec<N, T>& target, const T& max) {
		Vec<N, T> towards = target - (*this);
		if (towards.lengthSqr() > max * max) {
			towards.normalize();
			towards.multiply(max);
		}
		this->operator+=(towards);
	}


	/// Comparisons
	inline bool operator< (const T& v) const {
		for (int i = 0; i < N; ++i) if (get(i) < v) return true;
		return false;
	}
	inline bool operator<= (const T& v) const {
		for (int i = 0; i < N; ++i) if (get(i) <= v) return true;
		return false;
	}
	inline bool operator> (const T& v) const {
		for (int i = 0; i < N; ++i) if (get(i) > v) return true;
		return false;
	}
	inline bool operator>= (const T& v) const {
		for (int i = 0; i < N; ++i) if (get(i) >= v) return true;
		return false;
	}
	inline bool operator== (const T& v) const {
		for (int i = 0; i < N; ++i) if (get(i) == v) return true;
		return false;
	}
	inline bool operator!= (const T& v) const {
		for (int i = 0; i < N; ++i) if (get(i) != v) return true;
		return false;
	}
	

	bool isNaN() const {
		for (int i = 0; i < N; ++i) if (std::isnan(get(i))) return true;
		return false;
	}


	// String conversion
	inline std::string toString() const {
		std::string ret = "[ ";
		for (int i = 0; i < N; ++i) {
			ret += std::to_string(components[i]);
			if (i < N - 1) ret += ", \t";
		}
		return ret + " ]";
	}

};
