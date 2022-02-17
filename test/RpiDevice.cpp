#include "RpiDevice.h"

RpiDevice::RpiDevice(std::string ipAddress, int port):
    mb::Device(ipAddress, port)
{
    testRegister1 = new mb::Register<int>(this,75);
}

RpiDevice::~RpiDevice(){
    delete testRegister1;
    testRegister1 = nullptr;
}
