/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   encode.h
 * Author: kamyar
 * Email: <u>kamyarnemati at gmail.com</u>
 *
 * Created on 20 February 2017, 20:13
 * C++ 14
 */

#ifndef ENCODE_H
#define ENCODE_H

#include "base.h"

namespace libecg {                                        //ecg data compression
    
    /**
     * @description This is a derived class from <b>Base</b> class. It provides 
     * algorithms to perform hybrid data compression on ECG signals. Lossless 
     * compression is the first compression layer. A threshold is required for 
     * that matter. However, an extra layer of lossy compression is applied by 
     * specifying an aperture value greater than <i>zero</i>. Signal distortion 
     * will be introduced and signal quality is affected by aperture directly.
     * 
     * @author Kamyar Nemati
     * @Email <u>kamyarnemati at gmail.com</u>
     */
    class Encode : public Base {                             //derived from Base
        //private section------------------------------------------------------/
    private:
        Encode() = delete;                     //default constructor is disabled
        Encode(const Encode& that) = delete;      //copy constructor is disabled
        Encode& operator=(const Encode&) = delete;   //assignment is not allowed
        
        int aperture;        //the aperture value used to ignore adjacent errors
        volatile pList compressed; //compressed data that is processed gradually
        std::string sequence;     //holds binary string of final compressed data
        
        const bool read(const std::string& dataset_path);            //read ECGs
        
        /* COMPRESSION ALGORITHMS */
        const bool losslessCompression();             //#1: Lossless Compression
        const bool lossyCompressionFilter();    //#2-1: Lossy Compression Filter
        const bool redundantErrorEliminator();//#2-2: Redundant Error Eliminator
        const bool errorCountController();        //#2-3: Error Count Controller
        const bool sampleScaleDown();                    //#3: Sample Scale Down
        /* ----- */
        
        void writeInfo() const;                  //additinal data to be ambedded
        const bool translate();//translation of compressed data to binary string
        
        /* find absolute minimum & maximum value in the original dataset */
        const bool originalMinMax(int& min, int& max) const;
        /* find absolute minimum & maximum value in the compressed dataset */
        const bool compressedMinMax(int& min, int& max) const;
        
        const bool writeAsBin(
            const std::string& path) const;    //write compressed data as binary
        const bool writeAsDec(
            const std::string& path) const;   //write compressed data as decimal
        //protected section----------------------------------------------------/
    protected:
        //public section-------------------------------------------------------/
    public:
        /**
         * @description The <b>Encode</b> class needs to be initialised by the 
         * following parameters:
         * 
         * @param <i>const unsigned int&</i> <b>dataset_len</b> Number of 
         * samples to be read from the dataset file.
         * @param <i>const string&</i> <b>dataset_path</b> Relative path to the 
         * dataset file.
         * @param <i>const unsigned int&</i> <b>bitsThreshold</b> The number of 
         * bits to form threshold.
         * @param <i>const unsigned int&</i> <b>aperture</b> The aperture size.
         * @param <i>bool&</i> <b>stat</b> Returns initialisation status.
         */
        explicit Encode(
            const unsigned int& dataset_len, 
            const std::string& dataset_path, 
            const unsigned int& bitsThreshold, 
            const unsigned int& aperture, 
            bool& stat
        );                                                         //constructor
        ~Encode();                                                  //destructor
        
        /**
         * @description Perform the compression process.
         * @return <i>const bool</i> Returns the status of encoding process.
         */
        const bool encode();                               //compression process
        /**
         * @description This function is not supported.
         * @return <i>const bool</i> Returns false.
         */
        inline const bool decode() {
            return false;
        }                                       //decompression is not supported
        
        /**
         * @description Use this function to get PRD from <b>Decode</b> class.
         * @param <i>pList& lst</i> <b>lst</b> Returns the original dataset as 
         * List.
         */
        void getOriginal(pList& lst) const;        //return the original dataset
        /**
         * @description This function is meant to initialise the <b>Decode</b> 
         * class by the compressed data list without the need to write to file.
         * @param <i>pList& lst</i> <b>lst</b> Returns the compressed dataset 
         * as List.
         */
        void getCompressed(pList& lst) const;    //return the compressed dataset
        /**
         * @description Calculates the compression ratio of the compressed data.
         * This function finds decimal-based CR.
         * @return <i>const float</i> Returns the compression ratio (CR).
         */
        const float getCompressionRatio() const;   //calculate compression ratio
        /**
         * @description This function is meant to initialise the <b>Decode</b> 
         * class by the binary string of compressed data without the need to 
         * write to file.
         * @return <i>const string</i> Returns the compressed dataset as a 
         * string of binaries.
         */
        const std::string getBinarySequeneCompressed() const;  //return binaries
        /**
         * @description Calculates the compression ratio of the compressed data.
         * This function finds binary-based CR. Meaning that it finds the total 
         * number of bits required to store the original dataset first. Then it 
         * compare it against the compressed binary string.
         * @return <i>const float</i> Returns the compression ratio (CR).
         */
        const float getBinarySequeneCompressionRatio() const;  //binary-based CR
        
        /**
         * @description Write compressed data into file.
         * @param <i>const string&</i> <b>path</b> The path to the output file.
         * File will be created if not exists. Will be overwritten otherwise.
         * @param <i>STREAM_TYPE</i> <b>t</b> Specify the type of write.
         * @return <i>const bool</i> Returns the process status.
         */
        const bool write(
            const std::string& path, 
            STREAM_TYPE t
        ) const;                              //stream compressed data to a file
    };
}

#endif /* ENCODE_H */

