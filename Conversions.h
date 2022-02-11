#pragma once
#include "stdint.h"
#include <vector>

unsigned int convertToUInt(std::vector<uint16_t> input);
int convertToInt(std::vector<uint16_t> input);
int convertToInt(unsigned int input, unsigned int length);
float convertToFloat(std::vector<uint16_t> input);
float convertToFloat(unsigned int input);
float convertToFloat(int input);
