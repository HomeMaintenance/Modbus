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
        #ifdef MODBUS_DEBUG
            std::cerr << "modbus device "+ ipAddress + ":" << port << " destroyed" << std::endl;
        #endif // DEBUG
    }

    bool Device::connect(const char* ipAddress_, int port_)
    {
        connection = modbus_new_tcp(ipAddress_,port_);
        bool connect_error = modbus_connect(connection) < 0;
        if (connect_error)
        {
            #ifdef MODBUS_DEBUG
                std::cerr << "modbus connection error \"" << modbus_strerror(errno) <<  "\" to TCP/IP " + ipAddress + ":" << port_ << std::endl;
            #endif // DEBUG
            modbus_free(connection);
            connection = nullptr;
            return false;
        }
        else
        {
            #ifdef MODBUS_DEBUG
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
        #ifdef MODBUS_DEBUG
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
        volatile int read_registers = modbus_read_registers(mb,2,5,tab_reg);
        modbus_close(mb);
        modbus_free(mb);
        for(int i=0; i<32; ++i){
            std::cout << "tab_reg[" << i << "]: " << tab_reg[i] << std::endl;
        }
    }

    bool Device::resetConnection(){
        std::lock_guard<std::mutex> lk(modbus_mtx);
        disconnect();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        for(int tries = 1; tries < 11; ++tries){
            std::cout << "\t\t Try " << tries << std::endl;
            connect(ipAddress.c_str(), port);
            if(connection != nullptr){
                std::cout << "\t\t Got new connection" << std::endl;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        errorMap.clear();
        return connection != nullptr;
    }

    void Device::reportError(int addr){
        unsigned int& errorCounter = errorMap[addr];
        if(errorCounter > 10){
            std::cout << "Resetting connection ..." << std::endl;
            if(resetConnection()){
                std::cout << "\t Success!" << std::endl;
            }
            else{
                std::cerr << "\t Error! Try again later!" << std::endl;
            }
            return;
        }
        std::cerr << "Error Reported. Counter: " << errorCounter << std::endl;
        errorCounter++;
    }
}
