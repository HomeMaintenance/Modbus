#include "TestDevice.h"

TestDevice::TestDevice(std::string ipAddress, int port):
    mb::Device(ipAddress, port)
{
    intRegister = new mb::Register<int>(this, 40005);
    shortRegister = new mb::Register<short>(this, 40005);
    longRegister = new mb::Register<long>(this, 40005);
    floatRegister = new mb::Register<float>(this, 40005);
}

TestDevice::~TestDevice(){
    delete intRegister;
    intRegister = nullptr;
    delete shortRegister;
    shortRegister = nullptr;
    delete longRegister;
    longRegister = nullptr;
    delete floatRegister;
    floatRegister = nullptr;
}
