#include <ModbusRegister.h>

namespace mb{
    std::string printVector(std::vector<uint16_t> input) {
        std::string retval = "[";
        for (int i = 0; i < input.size(); i++) {
            retval += std::to_string(input[i]);
            if (i < input.size() - 1) {
                retval += ", ";
            }
        }
        retval += "]";
        return retval;
    }
}
