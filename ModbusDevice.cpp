#include <iostream>
#include <exception>
#include <ModbusDevice.h>
#include <chrono>
#include <exception>

namespace mb{

    void Device::init(const char* ipAddress_, int port_)
    {
        ipAddress = ipAddress_;
        port = port_;
        connect(ipAddress.c_str(), port);
    }


    Device::Device(const char* ipAddress_, int port_ /* = 502 */){
        init(ipAddress_, port_);
    }

    Device::Device(std::string ipAddress_, int port_ /* = 502 */){
        init(ipAddress_.c_str(), port_);
    }

    Device::~Device()
    {
        disconnect();
        #ifdef DEBUG
            std::cerr << "modbus device "+ ipAddress + ":" << port << " destroyed" << std::endl;
        #endif // DEBUG
    }

    bool Device::connect(const char* ipAddress_, int port_)
    {
        connection = modbus_new_tcp(ipAddress_,port_);
        if (modbus_connect(connection) == -1)
        {
            #ifdef DEBUG
                std::cerr << "modbus connection error to ip " + ipAddress + ":" << port_ << std::endl;
            #endif // DEBUG
            modbus_free(connection);
            connection = nullptr;
            return false;
        }
        else
        {
            #ifdef DEBUG
                std::cerr << "modbus successfully connected to ip " + ipAddress + ":" << port_ << std::endl;
            #endif // DEBUG
            return true;
        }
    }

    bool Device::disconnect()
    {
        modbus_close(connection);
        modbus_free(connection);
        connection = nullptr;
        #ifdef DEBUG
            std::cerr << "modbus connection " + ipAddress + ":" << port << " disconnected"<< std::endl;
        #endif // DEBUG
        return true;
    }

    void test_modbus()
    {
        modbus_t *mb;
        uint16_t tab_reg[32] = {0};
        mb = modbus_new_tcp("192.168.178.107",502);
        modbus_connect(mb);
        int read_registers = modbus_read_registers(mb,2,5,tab_reg);
        modbus_close(mb);
        modbus_free(mb);
        for(int i=0; i<32; ++i){
            std::cout << "tab_reg[" << i << "]: " << tab_reg[i] << std::endl;
        }
    }
}
