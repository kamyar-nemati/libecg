/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   decoder.h
 * Author: kamyar
 * Email: <u>kamyarnemati at gmail.com</u>
 *
 * Created on 20 February 2017, 20:13
 * C++ 14
 */

#ifndef DECODER_H
#define DECODER_H

#include "base.h"

namespace libecg {                                        //ecg data compression
    
    /**
     * @description This is a derived class from <b>Base</b> class. It provides 
     * decompression algorithms to decode any compressed ECG signal dataset by 
     * the <b>Encoder</b> class.
     * This class accepts either a string of binaries, a list of compressed 
     * data in decimal form, or a path to file that contains the compressed 
     * data.
     * 
     * @author Kamyar Nemati
     * @Email <u>kamyarnemati at gmail.com</u>
     */
    class Decoder : public Base {                            //derived from Base
        //private section------------------------------------------------------/
    private:
        Decoder() = delete;                    //default constructor is disabled
        Decoder(const Decoder& that) = delete;    //copy constructor is disabled
        Decoder& operator=(const Decoder&) = delete; //assignment is not allowed
        
        volatile pList reconstructed;           //holds the decompressed dataset
        
        const bool read(const pList& dataset_lst);        //read compressed data
        const bool read(
            const std::string path, 
            STREAM_TYPE t
        );                                      //read compressed data from file
        const bool readAsBinary(const std::string& path);  //read file as binary
        const bool readAsDecimal(const std::string& path);//read file as decimal
        
        /* DECOMPRESSION ALGORITHMS */
        const bool sampleScaleUp();                        //#1: Sample Scale Up
        const bool redundantErrorRecovery();      //#2: Redundant Error Recovery
        const bool decompression();                          //#3: Decompression
        
        const bool readInfo();                              //read embedded data
        const bool translate(const std::string& sequence); /* binary string 
                                                            * translation to 
                                                            * decimal numbers */
        //protected section----------------------------------------------------/
    protected:
        //public section-------------------------------------------------------/
    public:
        /**
         * @description <b>Decoder</b> class initialisation by the list of 
         * compressed dataset in decimal form. The following parameters needed.
         * 
         * @param <i>const unsigned int&</i> <b>dataset_len</b> Length of the 
         * compressed dataset list directly.
         * @param <i>const list(int)*& dataset_lst</i> <b>dataset_lst</b> The 
         * compressed dataset list in decimal form.
         * @param <i>bool& stat</i> <b>stat</b> Returns initialisation status.
         */
        explicit Decoder(
            const unsigned int& dataset_len, 
            const pList& dataset_lst, 
            bool& stat
        );                                              //overloaded constructor
        /**
         * @description <b>Decoder</b> class initialisation by the binary 
         * string of compressed data directly. The following parameters are 
         * required.
         * 
         * @param <i>const string&</i> <b>sequence</b> The binary string that 
         * represents the compressed ECG signal.
         * @param <i>bool& stat</i> <b>stat</b> Returns initialisation status.
         */
        explicit Decoder(
            const std::string& sequence, 
            bool& stat
        );                                              //overloaded constructor
        /**
         * @description <b>Decoder</b> class initialisation by the file that 
         * represents the compressed data. The following parameters are needed.
         * 
         * @param <i>const string&</i> <b>path</b> The path to the file.
         * @param <i>STREAM_TYPE</i> <b>t</b> Read type.
         * @param <i>bool& stat</i> <b>stat</b> Returns initialisation status.
         */
        explicit Decoder(
            const std::string& path, 
            STREAM_TYPE t, 
            bool& stat
        );                                                         //constructor
        ~Decoder();                                                 //destructor
        
        /**
         * @description This function is not supported.
         * @return <i>const bool</i> Returns false.
         */
        inline const bool encode() {
            return false;
        }                                         //compression is not supported
        /**
         * @description Perform the decompression process.
         * @return <i>const bool</i> Returns the status of decoding process.
         */
        const bool decode();                             //decompression process
        
        /**
         * @description This function is for the purpose of debugging.
         * @param <i>pList& lst</i> <b>lst</b> Returns the reconstructed 
         * dataset as List.
         */
        void getReconstructed(pList& lst) const;   //return decompressed dataset
        /**
         * @description Calculates the Percent Root-Mean-Square Difference 
         * (PRD) of the reconstructed data.
         * @param <i>const pList&</i> <b>originalSet</b> The Original dataset 
         * is required to calculate PRD.
         * @return <i>const float</i> Returns the calculated PRD.
         */
        const float getPercentRootMeanSquareDifference(
            const pList& originalSet
        ) const;                                                 //calculate PRD
        
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
        ) const;                           //stream reconstructed data to a file
    };
}

#endif /* DECODER_H */

