#include "Conversions.h"
#include <cmath>

unsigned int convertToUInt(std::vector<uint16_t> input){
    int retVal = 0;
    for(int i = input.size()-1; i >= 0; i--){
        float factor(std::pow(2,16*i));
        retVal += input[i] * factor;
    }
    return retVal;
}

int convertToInt(std::vector<uint16_t> input){
    int retVal(static_cast<int>(convertToUInt(input)));
    int mask = 1 << (15 + (input.size()-1) * 16);
    if(retVal & mask){
        retVal = (retVal & ~mask) - mask;
    }
    else{
        retVal = retVal & ~mask;
    }
    return retVal;
}

int convertToInt(unsigned int input, unsigned int length){
    int retVal(static_cast<int>(input));
    int mask = 1 << (15 + (length-1) * 16);
    if(retVal & mask){
        retVal = (retVal & ~mask) - mask;
    }
    else{
        retVal = retVal & ~mask;
    }
    return retVal;
}

float convertToFloat(std::vector<uint16_t> input){
    return static_cast<float>(convertToInt(input));
}

float convertToFloat(unsigned int input){
    return static_cast<float>(input);
}

float convertToFloat(int input){
    return static_cast<float>(input);
}
