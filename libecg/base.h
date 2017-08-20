/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   base.h
 * Author: kamyar
 * Email: <u>kamyarnemati at gmail.com</u>
 *
 * Created on 20 February 2017, 20:13
 * C++ 14
 */

#ifndef BASE_H
#define BASE_H

#include "lib.h"
#include "utils.h"

namespace libecg {                                        //ecg data compression
    
    class Utils;                                           //forward declaration
    
    /**
     * @description This is a 'Mixin' abstract calss which provides basic 
     * structure to classes <b>Encode</b> and <b>Decode</b>. This class 
     * therefore is not meant to be instantiated. Functional behaviour for 
     * functions <b>encode()</b> and <b>decode()</b> must be defined in derived 
     * classes.
     * 
     * @author Kamyar Nemati
     * @Email <u>kamyarnemati at gmail.com</u>
     */
    class Base {
        //private section------------------------------------------------------/
    private:
        Base(const Base& that) = delete;          //copy constructor is disabled
        Base& operator=(const Base&) = delete;       //assignment is not allowed
        
        struct ThresholdRange {                          //theoretical threshold
            int min;                           //threshold range (minimum value)
            int max;                           //threshold range (maximum value)
            int flag;                    //the flag that is used to find samples
        };
        struct Threshold {
            bool built;                            //indication of success built
            ThresholdRange* range;
        };
        struct Dataset {
            bool built;                                //one-time initialisation
            int* dataset_arr;    //to store initial data to be further processed
            int dataset_len;                     //length of the initial dataset
        };
        
        Dataset* dataset;                                      //initial dataset
        Threshold* threshold;                              //threshold structure
        /* EMBEDDED DATA */
        int _bitsOriginal;      //bits to store original samples (embedded data)
        int _bitsScaled;          //bits to store scaled samples (embedded data)
        int _bitsThreshold;    //bits to construct the threshold (embedded data)
        bool _lossyEnabled; //lossy compression filter indicator (embedded data)
        /* ---- */
        //protected section----------------------------------------------------/
    protected:
        typedef std::list<int>* pList;  //the main ADT that holds processed data
        
        const Utils* utils; //the class that provides some extra functionalities
        
        /* NUMBER OF BITS TO STORE EMBEDDED DATA */
        static const int BITS_EMBEDDED = 5;        //bits to store embedded data
        static const int BITS_EMBEDDED_INDICATOR = 1;  //a bit to keep indicator
        /* -- */
        
        const bool validateThreshold() const;  //threshold bit length validation
        const bool shapeThreshold();    //construct the threshold range and flag
        /* DATASET ACCESSORS */
        void initialiseDataset(const int& dataset_len);     //initialise dataset
        const int getDatasetAt(const int& idx) const;     //get dataset by index
        void setDatasetAt(const int& idx, const int& val) const;  //set by index
        const int getDatasetLen() const;                 //return dataset length
        void setDatasetLen(const int& dataset_len);         //set dataset length
        void deleteDataset();                               //deallocate dataset
        /* THRESHOLD GET ACCESSOR */
        const ThresholdRange* getThreshold() const; //get threshold range & flag
        /* EMBEDDED DATA GET ACCESSOR */
        const int get_bitsOriginal() const;
        const int get_bitsScaled() const;
        const int get_bitsThreshold() const;
        const int get_lossyEnabled() const;
        /* EMBEDDED DATA SET ACCESSOR */
        void set_bitsOriginal(const int& _bitsOriginal);
        void set_bitsScaled(const int& _bitsScaled);
        void set_bitsThreshold(const int& _bitsThreshold);
        void set_lossyEnabled(const int& _lossyEnabled);
        //public section-------------------------------------------------------/
    public:
        enum STREAM_TYPE {BINARY, DECIMAL};                   //read-write types
        
        /**
         * @description This constructor initialises attributes to <i>null</i> 
         * and <i>zero</i>. Therefore, it is necessary for derived class to 
         * re-initialise attributes. In order to construct the <b>threshold</b> 
         * use <b>set_bitsThreshold()</b> first to specify the threshold bit 
         * length, then call <b>shapeThreshold()</b> to make the threshold 
         * range. Once <b>threshold</b> is constructed, it cannot be altered.
         */
        Base();                                            //default constructor
        virtual ~Base() = 0;                       //destructor to be overridden
        
        virtual const bool encode() = 0;                           //compression
        virtual const bool decode() = 0;                         //decompression
        
        virtual const bool write(
            const std::string& path, 
            STREAM_TYPE t
        ) const = 0;                                     //stream data to a file
    };
}

#endif /* BASE_H */
