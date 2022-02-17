#pragma once
#include <ModbusDevice.h>
#include <ModbusRegister.h>

class TestDevice : public mb::Device {
public:
    TestDevice(std::string ipAddress, int port = 502);
    ~TestDevice();

    int test = 0;

    mb::Register<int>* intRegister;
    mb::Register<short>* shortRegister;
    mb::Register<long>* longRegister;
    mb::Register<float>* floatRegister;
private:
};
