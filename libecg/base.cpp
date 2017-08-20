/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "base.h"


libecg::Base::Base() { //attributes initialisation
    this->utils                     = new Utils();
    this->dataset                   = new Dataset;
    this->dataset->built            = false;
    this->dataset->dataset_arr      = nullptr;
    this->dataset->dataset_len      = 0;
    this->threshold                 = new Threshold;
    this->threshold->built          = false;
    this->threshold->range          = new ThresholdRange;
    this->threshold->range->min     = 0;
    this->threshold->range->max     = 0;
    this->threshold->range->flag    = 0;
    this->_bitsOriginal             = 0;
    this->_bitsScaled               = 0;
    this->_bitsThreshold            = 0;
    this->_lossyEnabled             = false;
}

libecg::Base::~Base() {
    delete this->threshold->range;
    delete this->threshold;
    delete this->dataset;
    delete this->utils;
}

const bool libecg::Base::validateThreshold() const {
    if (this->_bitsThreshold >= 3 && this->_bitsThreshold <= 16) {
        return true;
    }
    return false;
}

const bool libecg::Base::shapeThreshold() {
    try {
        if (!this->validateThreshold()) {
            throw std::logic_error("invalid_threshold_bit_length"); //Debugging
        }
        int e = (int)std::pow(2, this->_bitsThreshold);
        this->threshold->range->flag = e / 2 * (-1);
        this->threshold->range->min = this->threshold->range->flag + 1;
        this->threshold->range->max = e / 2 - 1;
        this->threshold->built = true;
    } catch (std::exception& ex) {
        std::printf("Exception: %s .\n", ex.what()); //Debugging
        return false;
    }
    return true;
}

void libecg::Base::initialiseDataset(const int& dataset_len) {
    assert(!this->dataset->built);
    this->dataset->dataset_len = dataset_len;
    this->dataset->dataset_arr = new int[dataset_len];
    this->dataset->built = true;
}

const int libecg::Base::getDatasetAt(const int& idx) const {
    assert(this->dataset->built);
    return this->dataset->dataset_arr[idx];
}

void libecg::Base::setDatasetAt(const int& idx, const int& val) const {
    assert(this->dataset->built);
    this->dataset->dataset_arr[idx] = val;
}

const int libecg::Base::getDatasetLen() const {
    return this->dataset->dataset_len;
}

void libecg::Base::setDatasetLen(const int& dataset_len) {
    this->dataset->dataset_len = dataset_len;
}

void libecg::Base::deleteDataset() {
    assert(this->dataset->built);
    delete[] this->dataset->dataset_arr;
    this->dataset->dataset_len = 0;
    this->dataset->built = false;
}

const libecg::Base::ThresholdRange* libecg::Base::getThreshold() const {
    assert(this->threshold->built);
    return this->threshold->range;
}

const int libecg::Base::get_bitsOriginal() const {
    return this->_bitsOriginal;
}

const int libecg::Base::get_bitsScaled() const {
    return this->_bitsScaled;
}

const int libecg::Base::get_bitsThreshold() const {
    return this->_bitsThreshold;
}

const int libecg::Base::get_lossyEnabled() const {
    return this->_lossyEnabled;
}

void libecg::Base::set_bitsOriginal(const int& _bitsOriginal) {
    this->_bitsOriginal = _bitsOriginal;
}

void libecg::Base::set_bitsScaled(const int& _bitsScaled) {
    this->_bitsScaled = _bitsScaled;
}

void libecg::Base::set_bitsThreshold(const int& _bitsThreshold) {
    this->_bitsThreshold = _bitsThreshold;
}

void libecg::Base::set_lossyEnabled(const int& _lossyEnabled) {
    this->_lossyEnabled = _lossyEnabled;
}
