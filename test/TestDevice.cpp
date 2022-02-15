#include "TestDevice.h"

TestDevice::TestDevice(std::string ipAddress, int port):
    mb::Device(ipAddress, port),
    intRegister(this, 10),
    shortRegister(this, 12),
    longRegister(this, 13),
    floatRegister(this, 15)
{

}

bool TestDevice::read_all_registers()
{
    intRegister.getValue();
    shortRegister.getValue();
    longRegister.getValue();
    floatRegister.getValue();
    return true;
};
