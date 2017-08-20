/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   utils.h
 * Author: kamyar
 * Email: <u>kamyarnemati at gmail.com</u>
 *
 * Created on 03 March 2017, 10:55
 */

#ifndef UTILS_H
#define UTILS_H

#include "lib.h"

namespace libecg {                                        //ecg data compression
    
    /**
     * @description This is a utility class. It provides some required 
     * functionalities to cater compression and decompression process needs for 
     * <b>Encode</b> and <b>Decode</b>.
     * 
     * @author Kamyar Nemati
     * @Email <u>kamyarnemati at gmail.com</u>
     */
    class Utils {
        //private section------------------------------------------------------/
    private:
        Utils(const Utils& that) = delete;
        Utils& operator=(const Utils&) = delete;
        
        static const unsigned int minBits(
            const int& min, 
            const int& max
        );
        static const bool intToBin(
            std::string& str, 
            const signed int& val, 
            const unsigned int& bit
        );
        static const std::string cutSubStr(
            std::string& str, 
            const unsigned int& pos, 
            const unsigned int& len
        );
        static const bool binToInt(
            signed int& val, 
            const std::string& str, 
            const unsigned int& bit, 
            const bool& sign
        );
        //protected section----------------------------------------------------/
    protected:
        //public section-------------------------------------------------------/
    public:
        Utils();
        ~Utils();
        
        /**
         * @description Find the minimum number of bits required to represent 
         * a range of numbers.
         * @param <i>const int&</i> <b>min</b> The smallest number in the range.
         * @param <i>const int&</i> <b>max</b> The largest number in the range.
         * @return <i>const int</i> Returns the number of bits.
         */
        static inline const int find_minBits(
                const int& min, 
                const int& max) {
            return minBits(min, max);
        }                     //the minimum number of bits to represent a number
        /**
         * @description Converts a decimal-based number into the corresponding 
         * binary-based number.
         * @param <i>string&</i> <b>str</b> The string of 0's & 1's 
         * to be returned after conversion.
         * @param <i>const signed int&</i> <b>val</b> The decimal number to be 
         * converted, ranging from negative to positive.
         * <b>ONLY INTEGER VALUES</b>
         * @param <i>const unsigned int&</i> <b>bit</b> The number of bits to 
         * represent the integer decimal value in binary.
         * @return <i>const bool</i> Returns the process status.
         */
        static inline const bool perform_intToBin(
                std::string& str, 
                const signed int& val, 
                const unsigned int& bit) {
            if (minBits(0, val) > bit) {
                return false;
            }
            return intToBin(str, val, bit);
        }                               //conversion of decimal to binary string
        /**
         * @description Find, remove, and then return a portion of a string.
         * @param <i>string&</i> <b>str</b> The main string in which to search 
         * and to be modified.
         * @param <i>const unsigned int&</i> <b>pos</b> The location of the 
         * sub-string to be found.
         * @param <i>const unsigned int&</i> <b>len</b> The length of the sub-
         * string to be found.
         * @return <i>const string</i> Returns the found sub-string.
         */
        static inline const std::string take_cutSubStr(
                std::string& str, 
                const unsigned int& pos, 
                const unsigned int& len) {
            return cutSubStr(str, pos, len);
        }                                  //read and remove portion of a string
        /**
         * @description Converts a binary-based number into the corresponding 
         * decimal-based number.
         * @param <i>signed int&</i> <b>val</b> The integer value to be 
         * returned after conversion.
         * @param <i>const string&</i> <b>str</b> The string of 0's & 1's to be 
         * converted.
         * @param <i>const unsigned int&</i> <b>bit</b> The number of bits by 
         * which the binary string is already represented.
         * @param <i>const bool&</i> <b>sign</b> Determines whether or not the 
         * binary string is a signed binary.
         * @return <i>const bool</i> Returns the process status.
         */
        static inline const bool perform_binToInt(
                signed int& val, 
                const std::string& str, 
                const unsigned int& bit, 
                const bool& sign) {
            if ((int)str.length() != bit) {
                return false;
            }
            return binToInt(val, str, bit, sign);
        }                               //conversion of binary string to decimal
    };
}

#endif /* UTILS_H */

