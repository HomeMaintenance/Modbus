#include <iostream>
#include <cassert>
#include <exception>
#include <ModbusDevice.h>
#include <chrono>
#include <exception>
#include <thread>
#include <chrono>

namespace mb{

    void Device::init(const char* ipAddress_, int port_)
    {
        ipAddress = ipAddress_;
        port = port_;
        _online = connect(ipAddress.c_str(), port);
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
        if(!_reconnectEnabled)
            assert(modbus_set_error_recovery(connection, static_cast<modbus_error_recovery_mode>(MODBUS_ERROR_RECOVERY_LINK | MODBUS_ERROR_RECOVERY_PROTOCOL)) == 0);
        assert(modbus_set_response_timeout(connection, 3, 0) == 0);
        assert(modbus_set_byte_timeout(connection, 3, 0) == 0);
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

    void Device::setOnline(bool status) const {
        if(status != _online)
            _online = status;
    }

    bool Device::online() const {
        return _online;
    }

    void Device::reconnect() {
        std::lock_guard<std::mutex> lk(modbus_mtx);
        std::cout << "modbus " << ipAddress << ":" << port << " reconnecting..." << std::endl;
        disconnect();
        int reconnectCounter = 0;
        while(!connection){
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            connect(ipAddress.c_str(), port);
            reconnectCounter++;
            std::cout << "\ttry: " << reconnectCounter << std::endl;
        }
        _online = true;
        std::cout << "reconnected" << std::endl;
    }

    void Device::enableReconnect(bool reconnect) {
        if(_reconnectEnabled == reconnect)
            return;
        if(connection)
            modbus_set_error_recovery(connection, static_cast<modbus_error_recovery_mode>(MODBUS_ERROR_RECOVERY_NONE));
        _reconnectEnabled = reconnect;
    }

    bool Device::reconnectEnabled() const {
        return _reconnectEnabled;
    }
}
