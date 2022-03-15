#pragma once
#include <modbus.h>
#include <string>
#include <map>
#include <vector>
#include <thread>
#include <mutex>
#include <Subject.h>


namespace mb{

    /**
     * @brief Modbus device
     *
     */
    class Device
    {
        public:
            //properties
            /**
             * @brief Ip address of the device
             *
             */
            std::string ipAddress;
            /**
             * @brief Port number of the device
             *
             */
            int port;
            //methods
            /**
             * @brief Construct a new Device object
             *
             * @param ipAddress Ip address of the device
             * @param port Port number of the device
             */
            Device(const char* ipAddress, int port = 502);
            /**
             * @brief Construct a new Device object
             *
             * @param ipAddress Ip address of the device
             * @param port Port number of the device
             */
            Device(std::string ipAddress, int port = 502);
            Device(const Device& other) = delete;
            virtual ~Device();
            /**
             * @brief Modbus connection pointer
             *
             */
            modbus_t* connection;
            /**
             * @brief Modbus connection mutex
             *
             */
            std::mutex modbus_mtx;
            /**
             * @brief Connect to physical device
             *
             * @param ipAddress Ip address of the device
             * @param port Port number of the device
             * @return true Connecting succeeded
             * @return false Connecting failed
             */
            bool connect(const char* ipAddress, int port = 502);
            /**
             * @brief Disconnect from physical device
             *
             * @return true: Disconnecting succeeded
             * @return false: Disconnecting failed
             */
            bool disconnect();
            /**
             * @brief Online status of device
             *
             */
            bool online;

            void reportError(int addr);

            bool resetConnection();

        private:
            /**
             * @brief Initialize device. Called inside constructor
             *
             * @param ipAddress ipAddress of the device
             * @param port Port number of the device
             */
            void init(const char* ipAddress, int port = 502);

            std::map<int, unsigned int> errorMap;
    };

    /**
     * @brief Modbus test function
     *
     */
    void test_modbus();
}
