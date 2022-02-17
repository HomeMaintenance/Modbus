#include "TestDevice.h"

TestDevice::TestDevice(std::string ipAddress, int port):
    mb::Device(ipAddress, port)
{
    intRegister = new mb::Register<int>(this, 10);
    shortRegister = new mb::Register<short>(this, 12);
    longRegister = new mb::Register<long>(this, 13);
    floatRegister = new mb::Register<float>(this, 15);
}
