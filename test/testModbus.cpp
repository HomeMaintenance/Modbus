#include <iostream>
#include "TestDevice.h"
#include <chrono>
#include <thread>

int main(int argc, char **argv){
    TestDevice testDevice("192.168.178.113",502);
    testDevice.intRegister.getValue();
    testDevice.intRegister.getValue();
    testDevice.intRegister.getValue(true);
    std::this_thread::sleep_for(std::chrono::seconds(static_cast<unsigned long>(5)));
    testDevice.intRegister.getValue();
    testDevice.intRegister.getValue();
    return 0;
}
