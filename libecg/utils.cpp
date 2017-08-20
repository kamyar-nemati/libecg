/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "utils.h"

libecg::Utils::Utils() {
    
}

libecg::Utils::~Utils() {
    
}

const unsigned int libecg::Utils::minBits(const int& min, const int& max) {
    int bitsMin, bitsMax;
    if (min == 0) {
        bitsMin = 1;
    } else {
        bitsMin = (int)std::ceil(std::log2(std::abs(min)));
        if (std::signbit((float)min) == 1) {
            ++bitsMin;
        }
    }
    if (max == 0) {
        bitsMax = 1;
    } else {
        bitsMax = (int)std::ceil(std::log2(std::abs(max)));
        if (std::signbit((float)max) == 1) {
            ++bitsMax;
        }
    }
    return bitsMin > bitsMax ? bitsMin : bitsMax;
}

const bool libecg::Utils::intToBin(std::string& str, const signed int& val, const unsigned int& bit) {
    switch (bit) {
        case 1:
            str = std::bitset<1>(val).to_string();
            break;
        case 2:
            str = std::bitset<2>(val).to_string();
            break;
        case 3:
            str = std::bitset<3>(val).to_string();
            break;
        case 4:
            str = std::bitset<4>(val).to_string();
            break;
        case 5:
            str = std::bitset<5>(val).to_string();
            break;
        case 6:
            str = std::bitset<6>(val).to_string();
            break;
        case 7:
            str = std::bitset<7>(val).to_string();
            break;
        case 8:
            str = std::bitset<8>(val).to_string();
            break;
        case 9:
            str = std::bitset<9>(val).to_string();
            break;
        case 10:
            str = std::bitset<10>(val).to_string();
            break;
        case 11:
            str = std::bitset<11>(val).to_string();
            break;
        case 12:
            str = std::bitset<12>(val).to_string();
            break;
        case 13:
            str = std::bitset<13>(val).to_string();
            break;
        case 14:
            str = std::bitset<14>(val).to_string();
            break;
        case 15:
            str = std::bitset<15>(val).to_string();
            break;
        case 16:
            str = std::bitset<16>(val).to_string();
            break;
        default:
            return false;
    }
    return true;
}

const std::string libecg::Utils::cutSubStr(std::string& str, const unsigned int& pos, const unsigned int& len) {
    std::string s = str.substr(pos, len);
    std::string f = "";
    if (pos > 0) {
        f += str.substr(0, pos);
    }
    if (pos + len < str.length() - 1) {
        f += str.substr(pos + len);
    }
    str = f;
    return s;
}

const bool libecg::Utils::binToInt(signed int& val, const std::string& str, const unsigned int& bit, const bool& sign) {
    switch (bit) {
        case 1:
            val = (int)std::bitset<1>(str).to_ulong();
            break;
        case 2:
            val = (int)std::bitset<2>(str).to_ulong();
            break;
        case 3:
            val = (int)std::bitset<3>(str).to_ulong();
            break;
        case 4:
            val = (int)std::bitset<4>(str).to_ulong();
            break;
        case 5:
            val = (int)std::bitset<5>(str).to_ulong();
            break;
        case 6:
            val = (int)std::bitset<6>(str).to_ulong();
            break;
        case 7:
            val = (int)std::bitset<7>(str).to_ulong();
            break;
        case 8:
            val = (int)std::bitset<8>(str).to_ulong();
            break;
        case 9:
            val = (int)std::bitset<9>(str).to_ulong();
            break;
        case 10:
            val = (int)std::bitset<10>(str).to_ulong();
            break;
        case 11:
            val = (int)std::bitset<11>(str).to_ulong();
            break;
        case 12:
            val = (int)std::bitset<12>(str).to_ulong();
            break;
        case 13:
            val = (int)std::bitset<13>(str).to_ulong();
            break;
        case 14:
            val = (int)std::bitset<14>(str).to_ulong();
            break;
        case 15:
            val = (int)std::bitset<15>(str).to_ulong();
            break;
        case 16:
            val = (int)std::bitset<16>(str).to_ulong();
            break;
        default:
            return false;
    }
    if (sign) {
        int x = (int)std::pow(2, bit - 1);
        if (val >= x) {
            val = (val + x) % x - x;
        }
    }
    return true;
}
