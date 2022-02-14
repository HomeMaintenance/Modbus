#include "TestDevice.h"

TestDevice::TestDevice(std::string ipAddress, int port):
    mb::Device(ipAddress, port),
    intRegister(this, 10),
    shortRegister(this, 11),
    longRegister(this, 12),
    floatRegister(this, 13)
{

}

bool TestDevice::read_all_registers() const
{
    intRegister.getValue();
    shortRegister.getValue();
    longRegister.getValue();
    floatRegister.getValue();
    return true;
};
