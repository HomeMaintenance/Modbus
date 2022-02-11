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
        start_thread();
    }

    void Device::start_thread()
    {
        if(read_values_run == false){
            read_values_run = true;
            read_values_thread = std::make_unique<std::thread>(&Device::read_values_task, this);
            #ifdef DEBUG
                std::cerr << "modbus connection " + ipAddress + ":" << port << " thread started" << std::endl;
            #endif // DEBUG
        }
    }

    Device::Device(const char* ipAddress_, int port_/*=502*/){
        init(ipAddress_, port_);
    }

    Device::Device(std::string ipAddress_, int port_/*=502*/){
        init(ipAddress_.c_str(), port_);
    }

    Device::~Device()
    {
        stop_thread();
        disconnect();
        #ifdef DEBUG
            std::cerr << "modbus device "+ ipAddress + ":" << port << " destroyed" << std::endl;
        #endif // DEBUG
    }

    void Device::stop_thread()
    {
        if(read_values_run){
            std::thread::id thread_id = read_values_thread->get_id();
            if(read_values_thread->joinable())
                read_values_run = false;
                read_values_thread->join();
            #ifdef DEBUG
                std::cerr << "modbus connection " + ipAddress + ":" << port << " thread stopped"<< std::endl;
            #endif // DEBUG
        }
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

    void Device::test_class(){
        std::cout << "ipAddress: " << ipAddress << std::endl;
        std::cout << "port: " << port << std::endl;
        // modbus_read_registers(connection,2,5,tab_reg);
        // for(int i=0; i<32; ++i){
        //     std::cout << "tab_reg[" << i << "]: " << tab_reg[i] << std::endl;
        // }
    }

    void Device::test_all_registers()
    {
        // for(auto& x: registers){
        //     auto registerValue = x.second.readRawData(connection);
        // }
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

    bool Device::read_all_registers()
    {
        return true;
    }

    void Device::read_values_task()
    {
        #ifdef DEBUG
            std::cerr << "modbus connection " + ipAddress + ":" << port << " thread started"<< std::endl;
        #endif // DEBUG
        while(read_values_run){
            #ifdef DEBUG
                std::cerr << "modbus connection " + ipAddress + ":" << port << " thread running"<< std::endl;
            #endif // DEBUG
            try
            {
                bool connection_state = read_all_registers();
                if(connection_state){
                    update_connections_stats('1');
                    notify();
                }
                else{
                    update_connections_stats('0');
                }
            }
            catch(std::exception& e)
            {
                std::cout << e.what() << std::endl;
                update_connections_stats('e');
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<unsigned long>(1000*read_values_period)));
        }
    }

    void Device::update_connections_stats(char connection_state)
    {
        test_counter++;
        test_history += "|";
        switch(connection_state){
            case '1':
                online = true;
                test_counter_success++;
                test_history += "+";
                break;
            case '0':
                online = false;
                test_counter_fail++;
                test_history += "-";
                break;
            case 'e':
                online = false;
                test_counter_fail++;
                test_history += "e";
                break;
        }
    }

    void Device::print_counters()
    {
        std::cout << "total: "<<test_counter<<\
            ", success: "<<test_counter_success<<\
            ", failure: "<<test_counter_fail<<std::endl;
        std::cout<<"history: "<<test_history<<std::endl;
    }
}
