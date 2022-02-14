#include <iostream>
#include "TestDevice.h"

int main(int argc, char **argv){
    TestDevice testDevice("127.0.0.1");
    testDevice.read_all_registers();
    return 0;
}
