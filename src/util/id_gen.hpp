#pragma once

#include <cstdint>
#include <string>
#include <chrono>

namespace hyperlog::idgen {

class IDGen {
private:
    uint64_t state_;
    
    static constexpr uint64_t wyrand_mix(uint64_t a, uint64_t b) {
        __uint128_t r = (__uint128_t)a * b;
        return (r >> 64) ^ r;
    }

public:
    IDGen(uint64_t seed = 0) {
        state_ = seed ? seed : std::chrono::steady_clock::now().time_since_epoch().count();
    }
    
    uint64_t next() {
        state_ += 0xa0761d6478bd642full;
        return wyrand_mix(state_, state_ ^ 0xe7037ed1a0b428dbull);
    }
    
    void generate_hex(char* buf, size_t len) {
        static constexpr char hex[] = "0123456789abcdef";
        for (size_t i = 0; i < len; i += 16) {
            uint64_t val = next();
            for (size_t j = 0; j < 16 && i + j < len; ++j) {
                buf[i + j] = hex[(val >> (j * 4)) & 0xF];
            }
        }
    }
    
    std::string hex(size_t len = 16) {
        std::string result(len, '\0');
        generate_hex(&result[0], len);
        return result;
    }
    
    void generate_alphanum(char* buf, size_t len) {
        static constexpr char chars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        uint64_t val = next();
        int bits = 0;
        for (size_t i = 0; i < len; ++i) {
            if (bits < 6) {
                val = next();
                bits = 64;
            }
            buf[i] = chars[val & 0x3F];
            val >>= 6;
            bits -= 6;
        }
    }
    
    std::string alphanum(size_t len = 16) {
        std::string result(len, '\0');
        generate_alphanum(&result[0], len);
        return result;
    }
};

}