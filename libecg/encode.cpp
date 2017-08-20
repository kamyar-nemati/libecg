/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "encode.h"

libecg::Encode::Encode(
        const unsigned int& dataset_len, 
        const std::string& dataset_path, 
        const unsigned int& bitsThreshold, 
        const unsigned int& aperture, 
        bool& stat) : Base() {
    //-------------------------------------------------------------------------/
    std::printf("Received parameters:\n"); //Debugging
    std::printf("- ECG signal dataset: %s\n", dataset_path.c_str()); //Debugging
    std::printf("- Dataset length: %u\n", dataset_len); //Debugging
    std::printf("- Threshold: %u\n", bitsThreshold); //Debugging
    std::printf("- Aperture: %u\n", aperture); //Debugging
    std::printf("Initialising Encode...\n"); //Debugging
    //Base class' attributes
    this->initialiseDataset(dataset_len);                    //re-initialisation
//    this->set_bitsOriginal(0);                  //will be re-initialised later
//    this->set_bitsScaled(0);                    //will be re-initialised later
    this->set_bitsThreshold(bitsThreshold);                  //re-initialisation
    this->set_lossyEnabled((aperture > 0 ? true : false));   //re-initialisation
    //Derived class' attributes
    this->aperture = aperture;
    this->compressed = new std::list<int>();
    this->sequence = "";
    stat = this->shapeThreshold() && this->read(dataset_path);
    if (stat) {
        std::printf("Encode object initialised by %d samples.\n", this->getDatasetLen()); //Debugging
        //Analysing the original dataset
        std::printf("Further analysis:\n"); //Debugging
        int min, max, bit;
        stat = this->originalMinMax(min, max);
        bit = this->utils->find_minBits(min, max);
        std::printf("- Samples ranging from %d to %d. Hence, %d bits per sample needed.\n", min, max, bit); //Debugging
        std::printf("- The total number of %d bits required to represent the entire dataset.\n", bit * this->getDatasetLen()); //Debugging
        //-end-
    } else {
        std::printf("Encode object initialisation failure.\n"); //Debugging
    }
    //-------------------------------------------------------------------------/
}

libecg::Encode::~Encode() {
    this->deleteDataset();
    delete this->compressed;
}

const bool libecg::Encode::read(const std::string& dataset_path) {
    std::printf("Reading data...\n"); //Debugging
    try {
        std::ifstream infile(dataset_path);
        std::string line;
        while (getline(infile, line)) {
            std::istringstream iss(line);
            int a, b, c;
            if (!(iss >> a >> b >> c)) {
                continue;
            }
            if (a >= this->getDatasetLen()) {
                throw std::logic_error("dataset_length_exceeded"); //Debugging
            }
            this->setDatasetAt(a, b);
        }
        infile.close();
    } catch(std::exception& ex) {
        std::printf("Exception: %s .\n", ex.what()); //Debugging
        return false;
    }
    return true;
}

const bool libecg::Encode::losslessCompression() {
    //derive embedded data (original sample bits)------------------------------/
    int sampleMin = 0, sampleMax = 0;
    if(!this->originalMinMax(sampleMin, sampleMax)) { //Find the smallest and the largest sample in the original dataset.
        return false;
    }
    this->set_bitsOriginal(this->utils->find_minBits(sampleMin, sampleMax)); //Calculat the minimum number of bits needed to represent each sample in the dataset.
    //Get required data--------------------------------------------------------/
    int error_max = this->getThreshold()->max;
    int flag = this->getThreshold()->flag;
    //Lossless Compression algorithm-------------------------------------------/
    try {
        this->compressed->clear(); //Clean up the list that holds compressed data.
        //-start-
        int currentSample = this->getDatasetAt(0); //Read the first sample.
        int referenceSample = currentSample; //The first sample is also considered as the Reference Sample.
        this->compressed->push_back(currentSample); //The first sample is always saved.
        int e = 0; //It holds the difference between the current sample and the Reference Sample.
        for (int i = 1; i < this->getDatasetLen(); ++i) { //Iteration starts from the second sample as the first one has already been saved.
            currentSample = this->getDatasetAt(i); //Update the 'currentSamples' as the loop goes by.
            e = currentSample - referenceSample; //Calculate the error.
            if (std::abs(e) <= error_max) { //Determine if the error falls into the Error Threshold.
                this->compressed->push_back(e); //Save the calculated error instead of the original current sample if its error doesn't exceed the Error Threashold.
            } else { //Otherwise; If the error exceeds the Error Threashold:
                this->compressed->push_back(flag); //Put a Flag first.
                this->compressed->push_back(currentSample); //Then, save the original sample.
                referenceSample = currentSample; //Finally, set the saved original sample as the Reference Sample.
            }
        }
        //-end-
    } catch(std::exception& ex) {
        std::printf("Exception: %s .\n", ex.what()); //Debugging
        return false;
    }
    //Return-------------------------------------------------------------------/
    std::printf("Phase 1: Lossless Compression (%d items)\n", (int)this->compressed->size()); //Debugging
    return true;
}

const bool libecg::Encode::lossyCompressionFilter() {
    //Lossy Compression Filter algorithm---------------------------------------/
    try {
        //-start-
        int current = 0, previous = 0; //Adjacent errors
        for (std::list<int>::iterator it = this->compressed->begin(); it != this->compressed->end(); ++it) { //Process the compressed dataset.
            if (it == this->compressed->begin()) {
                ++it; //Start from the second item since the first one is always an original sample at this point.
                ++it; //Move one more index forward as the pointer 'previous' will point to the second index.
            }
            current = *it;
            std::list<int>::const_iterator ot = it;
            --ot;
            previous = *ot;
            if (current == this->getThreshold()->flag) { //Then next is an original sample (let's call it foo).
                std::advance(it, 2); //To make 'previous' point to one index after foo for the next round. 3 jumps needed.
            } else {
                if (previous == this->getThreshold()->flag) { //Then 'current' is an original sample.
                    ++it; //In this case 2 jumps needed.
                } else { //Otherwise:
                    if (std::abs(current - previous) <= this->aperture) { //Do the comparison against the Aperture.
                        *it = previous;
                    }
                }
            }
        }
        //-end-
    } catch(std::exception& ex) {
        std::printf("Exception: %s .\n", ex.what()); //Debugging
        return false;
    }
    //Return-------------------------------------------------------------------/
    std::printf("Phase 2-1: Lossy Compression Filter (%d items)\n", (int)this->compressed->size()); //Debugging
    return this->redundantErrorEliminator();
}

const bool libecg::Encode::redundantErrorEliminator() {
    //Redundant Error Eliminator algorithm-------------------------------------/
    try {
        //-start-
        pList tmp = new std::list<int>(); //Temporary list to hold compressed data being processed.
        int currentItem = *this->compressed->begin(); //Start processing compressed data from beginning.
        int buffer = INT_MAX; //Hold an error to do comparison against adjacent errors.
        int count = 1; //Hold the number of equal consecutive errors.
        tmp->push_back(currentItem); //First compressed data is always saved.
        for (std::list<int>::const_iterator it = this->compressed->begin(); it != this->compressed->end(); ++it) { //Start iteration.
            if (it == this->compressed->begin()) {
                ++it; //Iteration starts from the second item.
            }
            currentItem = *it; //Update the 'currentItem'.
            if (currentItem == this->getThreshold()->flag) { //A flag is separation between consecutive errors.
                if (buffer != INT_MAX) { //This means 'buffer' is set. If 'buffer' is not set, it means a flag happened to fall in between two original samples.
                    tmp->push_back(buffer); //Add the error held by 'buffer'.
                    tmp->push_back(count); //Then add the number of occurrence of that error.
                } //Following actions always take place after coming across a flag.
                tmp->push_back(currentItem); //Finally, add the flag.
                ++it; //Move one index forward.
                currentItem = *it; //Update 'currentItem' again. This points to an original sample.
                tmp->push_back(currentItem); //Add the original sample.
                count = 1; //Reset the counter.
                buffer = INT_MAX; //Reset the buffer.
            } else { //Following actions are to process consecutive errors between flags.
                if (buffer == INT_MAX) { //If 'buffer' is not set, then:
                    buffer = currentItem; //Set the buffer.
                    count = 1; //Set the counter.
                } else { //If 'buffer' is already set, then:
                    if (buffer == currentItem) { //Check if adjacent errors are equal.
                        ++count; //Increase the counter by one if adjacent errors are equal.
                    } else { //Otherwise:
                        tmp->push_back(buffer); //Add the error held by 'buffer'.
                        tmp->push_back(count); //Then add the number of occurrence of that error.
                        count = 1; //Finally, reset the counter.
                        buffer = currentItem; //And then, update the buffer.
                    }
                }
            }
            std::list<int>::const_iterator ot = this->compressed->end();
            --ot;
            if (it == ot) { //If reached the end, save 'buffer' and the counter before terminating the loop.
                tmp->push_back(buffer);
                tmp->push_back(count);
            }
        }
        this->compressed->clear(); //Proceed to update the compressed data.
        for (std::list<int>::const_iterator it = tmp->begin(); it != tmp->end(); ++it) {
            this->compressed->push_back(*it);
        }
        delete tmp;
        //-end-
    } catch(std::exception& ex) {
        std::printf("Exception: %s .\n", ex.what()); //Debugging
        return false;
    }
    //Return-------------------------------------------------------------------/
    std::printf("Phase 2-2: Redundant Error Eliminator (%d items)\n", (int)this->compressed->size()); //Debugging
    return this->errorCountController();
}

const bool libecg::Encode::errorCountController() {
    //Get required data--------------------------------------------------------/
    int maxErrorNumber = (int)std::pow(2, this->get_bitsThreshold());
    //Error Count Controller algorithm-----------------------------------------/
    try {
        //-start-
        int errorNumber = 0, errorValue = 0;
        for (std::list<int>::iterator it = this->compressed->begin(); it != this->compressed->end(); ++it) {
            if (it == this->compressed->begin()) {
                continue;
            }
            if (*it == this->getThreshold()->flag) {
                ++it;
            } else {
                errorValue = *it;
                std::list<int>::iterator ot = it;
                ++ot;
                 errorNumber= *ot;
                if (errorNumber > maxErrorNumber) {
                    *ot = errorNumber - maxErrorNumber;
                    this->compressed->insert(it, errorValue);
                    this->compressed->insert(it, maxErrorNumber);
                    --it;
                } else {
                    ++it;
                }
            }
        }
        //-end-
    } catch(std::exception& ex) {
        std::printf("Exception: %s .\n", ex.what()); //Debugging
        return false;
    }
    //Return-------------------------------------------------------------------/
    std::printf("Phase 2-3: Error Count Controller (%d items)\n", (int)this->compressed->size()); //Debugging
    return true;
}

const bool libecg::Encode::sampleScaleDown() {
    //Sample Scale Down algorithm----------------------------------------------/
    try {
        //-start-
        int compressedSampleMin, compressedSampleMax;
        if (!this->compressedMinMax(compressedSampleMin, compressedSampleMax)) {
            return false;
        }
        int designatedSample = compressedSampleMin;
        int difference;
        for (std::list<int>::iterator it = this->compressed->begin(); it != this->compressed->end(); ++it) {
            if (it == this->compressed->begin()) { //There's always an original sample at this index.
                difference = *it - compressedSampleMin; //Calculate the difference.
                *it = difference;
            } else {
                if (*it == this->getThreshold()->flag) {
                    ++it;
                    difference = *it - compressedSampleMin; //Calculate the difference.
                    *it = difference;
                }
            }
        }
        if (!this->compressedMinMax(compressedSampleMin, compressedSampleMax)) {
            return false;
        }
        this->set_bitsScaled(this->utils->find_minBits(compressedSampleMin, compressedSampleMax));
        this->compressed->push_front(designatedSample); /* To add the smallest sample at beginning of the list.
                                                         * It'll be used to scale other samples throughout the list. */
        //-end-
    } catch(std::exception& ex) {
        std::printf("Exception: %s .\n", ex.what()); //Debugging
        return false;
    }
    //Return-------------------------------------------------------------------/
    std::printf("Phase 3: Sample Scale Down (%d items)\n", (int)this->compressed->size()); //Debugging
    return true;
}

void libecg::Encode::writeInfo() const {
    std::printf("Embedding additional data...\n"); //Debugging
    this->compressed->push_back(this->get_bitsOriginal()); // Write the number of bits required to represent an original sample to the compressed ECG.
    this->compressed->push_back(this->get_bitsScaled()); // Write the number of bits required to represent a scaled original sample to the compressed ECG.
    this->compressed->push_back(this->get_bitsThreshold()); // Write the Error Threshold size to the compressed ECG.
    this->compressed->push_back(this->get_lossyEnabled()); // Write the Lossy Compression Filter indicator bit to the compressed ECG.
}

const bool libecg::Encode::translate() {
    std::printf("Translating %d decimal items into bits...\n", (int)this->compressed->size()); //Debugging
    std::string str_designatedSample = "";
    std::string str_bitsOriginal = "";
    std::string str_bitsScaled = "";
    std::string str_bitsThreshold = "";
    std::string str_lossyEnabled = "";
    try {
        std::list<int>::const_iterator ot = this->compressed->end();
        --ot;
        if (!this->utils->perform_intToBin(str_lossyEnabled, *ot, this->BITS_EMBEDDED_INDICATOR)) {
            return false;
        }
        --ot;
        if (!this->utils->perform_intToBin(str_bitsThreshold, *ot, this->BITS_EMBEDDED)) {
            return false;
        }
        --ot;
        if (!this->utils->perform_intToBin(str_bitsScaled, *ot, this->BITS_EMBEDDED)) {
            return false;
        }
        --ot;
        if (!this->utils->perform_intToBin(str_bitsOriginal, *ot, this->BITS_EMBEDDED)) {
            return false;
        }
        std::list<int>::const_iterator it = this->compressed->begin();
        if (!this->utils->perform_intToBin(str_designatedSample, *it, this->get_bitsOriginal())) {
            return false;
        }
        ++it;
        this->sequence += str_designatedSample; //embedd designate sample
        std::string tmp;
        if (!this->utils->perform_intToBin(tmp, *it, this->get_bitsScaled())) { //item after designated sample (this item) is always a scaled sample
            return false;
        }
        this->sequence += tmp;
        ++it;
        for (; it != ot; ++it) {
            if (*it == this->getThreshold()->flag) { //if a flag
                if (!this->utils->perform_intToBin(tmp, *it, this->get_bitsThreshold())) {
                    return false;
                }
                this->sequence += tmp; //add flag
                ++it;
                if (!this->utils->perform_intToBin(tmp, *it, this->get_bitsScaled())) { //after a flag is always a scaled sample
                    return false;
                }
                this->sequence += tmp; //add scaled sample
            } else { //if not a flag, then error (either error or count)
                if (!this->utils->perform_intToBin(tmp, (*it == std::pow(2, this->get_bitsThreshold()) ? 0 : *it), this->get_bitsThreshold())) { //no error will ever be equal to 2 to the power of the threshold length, therefore it's a count
                    return false;
                }
                this->sequence += tmp;
            }
        }
    } catch(std::exception& ex) {
        std::printf("Exception: %s .\n", ex.what()); //Debugging
        return false;
    }
    this->sequence += str_bitsOriginal; //embedd data
    this->sequence += str_bitsScaled; //embedd data
    this->sequence += str_bitsThreshold; //embedd data
    this->sequence += str_lossyEnabled; //embedd data
    std::printf("Translated into %d bits.\n", (int)this->sequence.length()); //Debugging
    return true;
}

const bool libecg::Encode::encode() {
    std::printf("Compressing...\n"); //Debugging
    if (!this->losslessCompression()) {
        return false;
    }
    if (this->get_lossyEnabled()) {
        if (!this->lossyCompressionFilter()) {
            return false;
        }
    }
    if (!this->sampleScaleDown()) {
        return false;
    }
    this->writeInfo();
    if (!this->translate()) {
        return false;
    }
    std::printf("Compression is done.\n"); //Debugging
    return true;
}

const bool libecg::Encode::originalMinMax(int& min, int& max) const {
    try {
        min = INT_MAX;
        max = INT_MIN;
        for (int i = 0; i < this->getDatasetLen(); ++i) {
            if (this->getDatasetAt(i) < min) {
                min = this->getDatasetAt(i);
            }
            if (this->getDatasetAt(i) > max) {
                max = this->getDatasetAt(i);
            }
        }
    } catch(std::exception& ex) {
        std::printf("Exception: %s .\n", ex.what()); //Debugging
        return false;
    }
    return true;
}

const bool libecg::Encode::compressedMinMax(int& min, int& max) const {
    try {
        pList tmp = new std::list<int>();
        int currentSample = *this->compressed->begin();
        tmp->push_back(currentSample);
        for (std::list<int>::const_iterator it = this->compressed->begin(); it != this->compressed->end(); ++it) {
            currentSample = *it;
            if (currentSample == this->getThreshold()->flag) {
                ++it;
                currentSample = *it;
                tmp->push_back(currentSample);
            }
        }
        min = *std::min_element(tmp->begin(), tmp->end());
        max = *std::max_element(tmp->begin(), tmp->end());
        delete tmp;
    } catch(std::exception& ex) {
        std::printf("Exception: %s .\n", ex.what()); //Debugging
        return false;
    }
    return true;
}

const bool libecg::Encode::writeAsBin(const std::string& path) const {
    try {
        std::ofstream outfile;
        outfile.open(path);
        outfile << this->sequence;
        outfile.close();
    } catch(std::exception& ex) {
        std::printf("Exception: %s .\n", ex.what()); //Debugging
        return false;
    }
    return true;
}

const bool libecg::Encode::writeAsDec(const std::string& path) const {
    std::string str = "";
    std::string line;
    try {
        std::ofstream outfile;
        outfile.open(path);
        for (std::list<int>::const_iterator it = this->compressed->begin(); it != this->compressed->end(); ++it) {
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

void libecg::Encode::getOriginal(pList& lst) const {
    for (int i = 0; i < this->getDatasetLen(); ++i) {
        lst->push_back(this->getDatasetAt(i));
    }
}

void libecg::Encode::getCompressed(pList& lst) const {
    for (std::list<int>::const_iterator it = this->compressed->begin(); it != this->compressed->end(); ++it) {
        lst->push_back(*it);
    }
}

const float libecg::Encode::getCompressionRatio() const {
    return float(this->getDatasetLen()) / (float)this->compressed->size();
}

const std::string libecg::Encode::getBinarySequeneCompressed() const {
    return this->sequence;
}

const float libecg::Encode::getBinarySequeneCompressionRatio() const {
    int originalMin, originalMax;
    if (!this->originalMinMax(originalMin, originalMax)) {
        return std::numeric_limits<float>::quiet_NaN();
    }
    int find_minBitsOriginal = this->utils->find_minBits(originalMin, originalMax);
    int totalBits_original = this->getDatasetLen() * find_minBitsOriginal;
    int totalBits_compressed = this->sequence.length();
    float ratio = float(totalBits_original) / (float)totalBits_compressed;
    return ratio;
}

const bool libecg::Encode::write(const std::string& path, STREAM_TYPE t) const {
    if (t == STREAM_TYPE::BINARY) {
        return this->writeAsBin(path);
    }
    if (t == STREAM_TYPE::DECIMAL) {
        return this->writeAsDec(path);
    }
    return false;
}
