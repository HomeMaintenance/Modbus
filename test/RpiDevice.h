#pragma once
#include <ModbusDevice.h>
#include <ModbusRegister.h>

class RpiDevice: public mb::Device{
public:
    RpiDevice(std::string ipAddress, int port = 502);
    virtual ~RpiDevice();

    mb::Register<int>* testRegister1;
};
