#include <iostream>
#include "TestDevice.h"
#include "RpiDevice.h"
#include <chrono>
#include <thread>

void test_rpi_modbus(){
    RpiDevice rpi("192.168.178.107");
    bool ret = true;
    for(int i = 0; i < 20; i++){
        ret = rpi.testRegister1->setValue(i);
        assert(ret);
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<unsigned long>(5)));
        auto var = rpi.testRegister1->getValue(false, &ret);
        assert(ret);
        if(i == var)
            std::cout << i << ": success" << std::endl;
        else
            std::cout << i << ": error" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<unsigned long>(5)));
    }
}

void test_cache(){
    TestDevice testDevice("192.168.178.113",502);
    std::cout << "init done" << std::endl;
    testDevice.intRegister->getValue();
    testDevice.intRegister->getValue();
    testDevice.intRegister->getValue(true);
    std::cout << "Waiting " << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(static_cast<unsigned long>(5)));
    std::cout << "Resume" << std::endl;
    testDevice.intRegister->getValue();
    testDevice.intRegister->getValue();
}

void test_repeated_connection(){
    TestDevice testDevice("192.168.178.113",502);
    testDevice.disconnect();
    for(int i = 0; i < 20000; ++i){
        std::cout << i << " ----------------------------------------" << std::endl;
        testDevice.reportError(0);
    }
}

int main(int argc, char **argv){
    // test_rpi_modbus();
    test_repeated_connection();
    return 0;
}
