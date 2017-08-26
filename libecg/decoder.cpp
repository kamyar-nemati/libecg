/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "decoder.h"

libecg::Decoder::Decoder(
        const unsigned int& dataset_len, 
        const pList& dataset_lst, 
        bool& stat) : Base() {
    //-------------------------------------------------------------------------/
    std::printf("Initialising Decoder...\n"); //Debugging
    //Base class' attributes
    this->initialiseDataset(dataset_len);                    //re-initialisation
//    this->set_bitsOriginal(0);                  //will be re-initialised later
//    this->set_bitsScaled(0);                    //will be re-initialised later
//    this->set_bitsThreshold(0);                 //will be re-initialised later
//    this->set_lossyEnabled(false);              //will be re-initialised later
    //Derived class' attributes
    this->reconstructed = new std::list<int>();
    stat = this->read(dataset_lst);
    if (stat) {
        std::printf("Decoder object initialised by %d items.\n", this->getDatasetLen()); //Debugging
    } else {
        std::printf("Decoder object initialisation failure.\n"); //Debugging
    }
    //-------------------------------------------------------------------------/
}

libecg::Decoder::Decoder(
        const std::string& sequence, 
        bool& stat) : Base() {
    //-------------------------------------------------------------------------/
    std::printf("Initialising Decoder...\n"); //Debugging
    //Base class' attributes
//    this->initialiseDataset(dataset_len);                  //re-initialisation
//    this->set_bitsOriginal(0);                  //will be re-initialised later
//    this->set_bitsScaled(0);                    //will be re-initialised later
//    this->set_bitsThreshold(0);                 //will be re-initialised later
//    this->set_lossyEnabled(false);              //will be re-initialised later
    //Derived class' attributes
    this->reconstructed = new std::list<int>();
    stat = this->translate(sequence);
    if (stat) {
        std::printf("Decoder object initialised by %d bits.\n", (int)sequence.length()); //Debugging
    } else {
        std::printf("Decoder object initialisation failure.\n"); //Debugging
    }
    //-------------------------------------------------------------------------/
}

libecg::Decoder::Decoder(
        const std::string& path, 
        STREAM_TYPE t, 
        bool& stat) : Base() {
    //-------------------------------------------------------------------------/
    std::printf("Initialising Decoder...\n"); //Debugging
    //Base class' attributes
//    this->initialiseDataset(dataset_len);                  //re-initialisation
//    this->set_bitsOriginal(0);                  //will be re-initialised later
//    this->set_bitsScaled(0);                    //will be re-initialised later
//    this->set_bitsThreshold(0);                 //will be re-initialised later
//    this->set_lossyEnabled(false);              //will be re-initialised later
    //Derived class' attributes
    this->reconstructed = new std::list<int>();
    stat = this->read(path, t);
    if (stat) {
        std::printf("Decoder object initialised.\n"); //Debugging
    } else {
        std::printf("Decoder object initialisation failure.\n"); //Debugging
    }
    //-------------------------------------------------------------------------/
}

libecg::Decoder::~Decoder() {
    this->deleteDataset();
    delete this->reconstructed;
}

const bool libecg::Decoder::read(const pList& dataset_lst) {
    std::printf("Reading data...\n"); //Debugging
    try {
        int i = 0;
        for (std::list<int>::const_iterator it = dataset_lst->begin(); it != dataset_lst->end(); ++it) {
            this->setDatasetAt(i, *it);
            ++i;
        }
        if (i > this->getDatasetLen()) {
            return false;
        }
    } catch(std::exception& ex) {
        std::printf("Exception: %s .\n", ex.what()); //Debugging
        return false;
    }
    return true;
}

const bool libecg::Decoder::read(const std::string path, STREAM_TYPE t) {
    if (t == STREAM_TYPE::BINARY) {
        return this->readAsBinary(path);
    }
    if (t == STREAM_TYPE::DECIMAL) {
        return this->readAsDecimal(path);
    }
    return false;
}

const bool libecg::Decoder::readAsBinary(const std::string& path) {
    bool stat = false;
    try {
        std::ifstream infile(path);
        std::string line;
        if (std::getline(infile, line)) {
            stat = this->translate(line);
        }
        infile.close();
    } catch(std::exception& ex) {
        std::printf("Exception: %s .\n", ex.what()); //Debugging
        return false;
    }
    return stat;
}

const bool libecg::Decoder::readAsDecimal(const std::string& path) {
    bool stat = false;
    try {
        std::list<int>* tmp = new std::list<int>();
        std::ifstream infile(path);
        std::string line;
        int val;
        while (std::getline(infile, line)) {
            val = std::stoi(line);
            tmp->push_back(val);
        }
        infile.close();
        this->initialiseDataset((int)tmp->size()); //Base class' attribute initialisation
        stat = this->read(tmp);
        delete tmp;
    } catch(std::exception& ex) {
        std::printf("Exception: %s .\n", ex.what()); //Debugging
        return false;
    }
    return stat;
}

const bool libecg::Decoder::sampleScaleUp() {
    //Sample Scale Up algorithm------------------------------------------------/
    try {
        //-start-
        int designatedSample = this->getDatasetAt(0);
        int actualSample;
        for (int i = 1; i < this->getDatasetLen(); ++i) {
            if (i == 1) {
                actualSample = this->getDatasetAt(i) + designatedSample;
                this->reconstructed->push_back(actualSample);
            } else {
                if (this->getDatasetAt(i) == this->getThreshold()->flag) {
                    this->reconstructed->push_back(this->getDatasetAt(i));
                    ++i;
                    actualSample = this->getDatasetAt(i) + designatedSample;
                    this->reconstructed->push_back(actualSample);
                } else {
                    this->reconstructed->push_back(this->getDatasetAt(i));
                }
            }
        }
        //-end-
    } catch(std::exception& ex) {
        std::printf("Exception: %s .\n", ex.what()); //Debugging
        return false;
    }
    //Return-------------------------------------------------------------------/
    std::printf("Phase 1: Sample Scale Up (%d items)\n", this->getDatasetLen()); //Debugging
    return true;
}

const bool libecg::Decoder::redundantErrorRecovery() {
    //Redundant Error Recovery algorithm---------------------------------------/
    try {
        pList tmp = new std::list<int>();
        //-start-
        std::list<int>::const_iterator it = this->reconstructed->begin();
        int currentItem = *it;
        int count;
        tmp->push_back(currentItem);
        ++it;
        for (; it != this->reconstructed->end(); ++it) {
            currentItem = *it;
            if (currentItem == this->getThreshold()->flag) {
                tmp->push_back(currentItem);
                ++it;
                currentItem = *it;
                tmp->push_back(currentItem);
            } else {
                ++it;
                count = *it;
                for (int j = 0; j < count; ++j) {
                    tmp->push_back(currentItem);
                }
            }
        }
        //-end-
        this->reconstructed->clear();
        for (std::list<int>::const_iterator it = tmp->begin(); it != tmp->end(); ++it) {
            this->reconstructed->push_back(*it);
        }
        delete tmp;
    } catch(std::exception& ex) {
        std::printf("Exception: %s .\n", ex.what()); //Debugging
        return false;
    }
    //Return-------------------------------------------------------------------/
    std::printf("Phase 2: Redundant Error Recovery (%d items)\n", (int)this->reconstructed->size()); //Debugging
    return true;
}

const bool libecg::Decoder::decompression() {
    //Decompression algorithm--------------------------------------------------/
    try {
        pList tmp = new std::list<int>();
        //-start-
        int reference = -1;
        for (std::list<int>::const_iterator it = this->reconstructed->begin(); it != this->reconstructed->end(); ++it) { //Goes through all the items in the compressed list.
            if (it == this->reconstructed->begin()) { //In case of the first item:
                tmp->push_back(*it); //The first item is always the original sample.
                reference = *it; //It is always considered as reference as well.
            } else {
                if (*it == this->getThreshold()->flag) { //In case of any Flag:
                    ++it; //Skip one item which is the Flag.
                    tmp->push_back(*it); //After every Flag, there is an original sample. Therefore, it is added.
                    reference = *it; //Then, make the original sample as reference.
                } else { //In case of any value rather than the Flag:
                    tmp->push_back(*it + reference); /* It is an Error. Therefore, the original sample of this point
                                                      * can be calculated by having the reference sample and the Error
                                                      * that has been produced with respect to the reference sample. */
                }
            }
        }
        //-end-
        this->reconstructed->clear();
        for (std::list<int>::const_iterator it = tmp->begin(); it != tmp->end(); ++it) {
            this->reconstructed->push_back(*it);
        }
        delete tmp;
    } catch(std::exception& ex) {
        std::printf("Exception: %s .\n", ex.what()); //Debugging
        return false;
    }
    //Return-------------------------------------------------------------------/
    std::printf("Phase 3: Decompression (%d items)\n", (int)this->reconstructed->size()); //Debugging
    return true;
}

const bool libecg::Decoder::readInfo() {
    std::printf("Reading embedded data...\n"); //Debugging
    int i = this->getDatasetLen() - 1;
    this->set_lossyEnabled((this->getDatasetAt(i) == 1 ? true : false)); // Read the Lossy Compression Filter indicator bit from the compressed ECG.
    --i;
    this->set_bitsThreshold(this->getDatasetAt(i)); // Read the size of the Error Threshold from the compressed ECG.
    --i;
    this->set_bitsScaled(this->getDatasetAt(i)); // Read the number of bits required to represent a scaled original sample from the compressed ECG.
    --i;
    this->set_bitsOriginal(this->getDatasetAt(i)); // Read the number of bits required to represent an original sample from the compressed ECG.
    this->setDatasetLen(i);
    return this->shapeThreshold();
}

const bool libecg::Decoder::translate(const std::string& sequence) {
    std::printf("Translating %d bits...\n", (int)sequence.length()); //Debugging
    this->setDatasetLen(0); //Base class' attribute initialisation
    std::string buffer = sequence;
    std::string subseq;
    pList tmp = new std::list<int>();
    int item;
    try {
        int _lossyEnabled;
        int _bitsThreshold;
        int _bitsScaled;
        int _bitsOriginal;
        int _designatedSample;
        //Embedded bits #1
        subseq = this->utils->take_cutSubStr(buffer, buffer.length() - this->BITS_EMBEDDED_INDICATOR, this->BITS_EMBEDDED_INDICATOR);
        if (!this->utils->perform_binToInt(_lossyEnabled, subseq, this->BITS_EMBEDDED_INDICATOR, false)) {
            return false;
        }
        this->set_lossyEnabled((_lossyEnabled == 1 ? true : false)); //Base class' attribute initialisation
        //Embedded bits #2
        subseq = this->utils->take_cutSubStr(buffer, buffer.length() - this->BITS_EMBEDDED, this->BITS_EMBEDDED);
        if (!this->utils->perform_binToInt(_bitsThreshold, subseq, this->BITS_EMBEDDED, false)) {
            return false;
        }
        this->set_bitsThreshold(_bitsThreshold); //Base class' attribute initialisation
        if (!this->shapeThreshold()) {
            return false;
        }
        //Embedded bits #3
        subseq = this->utils->take_cutSubStr(buffer, buffer.length() - this->BITS_EMBEDDED, this->BITS_EMBEDDED);
        if (!this->utils->perform_binToInt(_bitsScaled, subseq, this->BITS_EMBEDDED, false)) {
            return false;
        }
        this->set_bitsScaled(_bitsScaled); //Base class' attribute initialisation
        //Embedded bits #4
        subseq = this->utils->take_cutSubStr(buffer, buffer.length() - this->BITS_EMBEDDED, this->BITS_EMBEDDED);
        if (!this->utils->perform_binToInt(_bitsOriginal, subseq, this->BITS_EMBEDDED, false)) {
            return false;
        }
        this->set_bitsOriginal(_bitsOriginal); //Base class' attribute initialisation
        //Embedded bits #5
        subseq = this->utils->take_cutSubStr(buffer, 0, _bitsOriginal);
        if (!this->utils->perform_binToInt(_designatedSample, subseq, _bitsOriginal, false)) {
            return false;
        }
        //Remaining bits
        //First item is always a scaled sample
        subseq = this->utils->take_cutSubStr(buffer, 0, _bitsScaled);
        if (!this->utils->perform_binToInt(item, subseq, _bitsScaled, false)) {
            return false;
        }
        tmp->push_back(item);
        this->setDatasetLen(this->getDatasetLen() + 1);
        while (buffer.length() > 0) {
            subseq = this->utils->take_cutSubStr(buffer, 0, _bitsThreshold); //after every scaled sample is either a (flag) or an (error). there would never be a (count) after a scaled sample
            if (!this->utils->perform_binToInt(item, subseq, _bitsThreshold, true)) { //errors and flags of courser might be negative numbers. therefore signed bit is set to (true)
                return false;
            }
            tmp->push_back(item);
            this->setDatasetLen(this->getDatasetLen() + 1);
            if (item == this->getThreshold()->flag) {
                subseq = this->utils->take_cutSubStr(buffer, 0, _bitsScaled);
                if (!this->utils->perform_binToInt(item, subseq, _bitsScaled, false)) { //after every flag there is always a scaled sample
                    return false;
                }
                tmp->push_back(item);
                this->setDatasetLen(this->getDatasetLen() + 1);
            } else {
                if (!this->get_lossyEnabled()) {//IF lossy not applied then errors followed by either a flag or another error
                    continue; //no count is present if lossy compression is not applied
                }
                subseq = this->utils->take_cutSubStr(buffer, 0, _bitsThreshold); //if not a flag, therefore it's an error
                if (!this->utils->perform_binToInt(item, subseq, _bitsThreshold, false)) { //an error is followed by a count IF lossy compression is applied (counts are always positive numbers)
                    return false;
                }
                if (item == 0) {
                    item = (int)std::pow(2, _bitsThreshold);
                }
                tmp->push_back(item);
                this->setDatasetLen(this->getDatasetLen() + 1);
            }
        }
        tmp->push_back(_bitsOriginal);
        this->setDatasetLen(this->getDatasetLen() + 1);
        tmp->push_back(_bitsScaled);
        this->setDatasetLen(this->getDatasetLen() + 1);
        tmp->push_back(_bitsThreshold);
        this->setDatasetLen(this->getDatasetLen() + 1);
        tmp->push_back(_lossyEnabled);
        this->setDatasetLen(this->getDatasetLen() + 1);
        tmp->push_front(_designatedSample);
        this->setDatasetLen(this->getDatasetLen() + 1);
    } catch(std::exception& ex) {
        std::printf("Exception: %s .\n", ex.what()); //Debugging
        return false;
    }
    this->initialiseDataset(this->getDatasetLen()); //Base class' attribute initialisation
    int i = 0;
    for (std::list<int>::const_iterator it = tmp->begin(); it != tmp->end(); ++it) {
        this->setDatasetAt(i, *it);
        ++i;
    }
    if (i > this->getDatasetLen()) {
        return false;
    }
    delete tmp;
    std::printf("Translated into %d decimal numbers.\n", i); //Debugging
    return true;
}

const bool libecg::Decoder::decode() {
    std::printf("Decompressing...\n"); //Debugging
    if (!this->readInfo()) {
        return false;
    }
    if (!this->sampleScaleUp()) {
        return false;
    }
    if (this->get_lossyEnabled()) {
        if(!this->redundantErrorRecovery()) {
            return false;
        }
    }
    if (!this->decompression()) {
        return false;
    }
    std::printf("Decompression is done.\n"); //Debugging
    return true;
}

void libecg::Decoder::getReconstructed(pList& lst) const {
    for (std::list<int>::const_iterator it = this->reconstructed->begin(); it != this->reconstructed->end(); ++it) {
        lst->push_back(*it);
    }
}

const float libecg::Decoder::getPercentRootMeanSquareDifference(const pList& originalSet) const {
    float prd = std::numeric_limits<float>::quiet_NaN();
    float num = 0, den = 0;
    try {
        if (originalSet->size() < 1 || this->reconstructed->size() < 1) {
            throw std::logic_error("dataset_size_error"); //Debugging
        }
        if (originalSet->size() != this->reconstructed->size()) {
            throw std::logic_error("dataset_size_mismatch"); //Debugging
        }
        std::list<int>::const_iterator ot = originalSet->begin();
        std::list<int>::const_iterator rt = this->reconstructed->begin();
        do {
            num += (int)std::pow(*ot - *rt, 2);
            den += (int)std::pow(*ot, 2);
            ++ot;
            ++rt;
        } while (ot != originalSet->end() && rt != this->reconstructed->end());
        prd = std::sqrt(num / den) * 100;
    } catch(std::exception& ex) {
        std::printf("Exception: %s .\n", ex.what()); //Debugging
    }
    return prd;
}

const bool libecg::Decoder::write(const std::string& path, STREAM_TYPE t) const {
    std::string str = "";
    std::string line;
    try {
        std::ofstream outfile;
        outfile.open(path);
        for (std::list<int>::const_iterator it = this->reconstructed->begin(); it != this->reconstructed->end(); ++it) {
            line = std::to_string(*it);
            line += "\n";
            str += line;
        }
        outfile << str;
        outfile.close();
    } catch(std::exception& ex) {
        std::printf("Exception: %s .\n", ex.what()); //Debugging
        return false;
    }
    return true;
}
