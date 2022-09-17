#pragma once

#include <vector>
#include <string>

#include "Vec.h"

class BinIO {
    BinIO()=delete;
    BinIO(const BinIO&)=delete;
    BinIO(BinIO&&)=delete;
public:

	/// Writes a value of trivial type T (no pointers) to data as bytes
	template<typename T>
	static void writeSimple(std::vector<std::uint8_t>& data, const T& val) {
		union {
			T original;
			std::uint8_t bytes[sizeof(T)];
		} o2b = {};
		o2b.original = val;
		for (std::size_t i = 0; i < sizeof(T); ++i) {
			data.push_back(o2b.bytes[i]);
		}
	}

	/// Reads a value of trivial type T (no pointers) from data as bytes
	template<typename T>
	static T readSimple(const std::vector<std::uint8_t>& data, size_t& at) {
		union {
			T original;
			std::uint8_t bytes[sizeof(T)];
		} b2o = {};
		for (std::size_t i = 0; i < sizeof(T); ++i) {
			assert(at < data.size());
			b2o.bytes[i] = data[at];
			++at;
		}
		return b2o.original;
	}
    
	static void writeString(std::vector<std::uint8_t>& data, const std::string& val) {
        for (const char& c : val) {
            writeSimple<char>(data, c);
        }
        writeSimple<char>(data, '\0');
    }
    
	static std::string readString(const std::vector<std::uint8_t>& data, size_t& at) {
        std::string s = "";
        assert(at < data.size());
        while (at < data.size()) {
            char c = readSimple<char>(data, at);
            if (c == '\0') break;
            else s += c;
        }
        return s;
    }
    
	template<int N, typename T>
	static void writeVec (std::vector<std::uint8_t>& data, const Vec<N, T>& val) {
		for (int i = 0; i < N; ++i) {
			writeSimple<T>(data, val[i]);
		}
	}
    
	template<int N, typename T>
	static Vec<N, T> readVec (const std::vector<std::uint8_t>& data, size_t& at) {
		Vec<N, T> r;
		for (int i = 0; i < N; ++i) {
			r.set(i, readSimple<T>(data, at));
		}
		return r;
	}

};
