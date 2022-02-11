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
    class Device: public Subject
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
            ~Device();
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
             * @brief Print statistics of connection
             *  Success- and Error counters
             */
            virtual void print_counters() final;
            /**
             * @brief Period time for reading values
             *
             */
            float read_values_period{5.}; //seconds
            /**
             * @brief Online status of device
             *
             */
            bool online;
            /**
             * @brief Stop #read_values_thread for reading values
             *
             */
            void stop_thread();
            /**
             * @brief Start #read_values_thread for reading values
             *
             */
            void start_thread();
            /**
             * @brief Function to read all values. Called inside #read_values_task
             *
             * @return true: Read succeeded
             * @return false: Read failed
             */
            virtual bool read_all_registers();

        private:
            /**
             * @brief Test this class
             *
             */
            void test_class();
            /**
             * @brief Test all registers
             *
             */
            void test_all_registers();
            /**
             * @brief Initialize device. Called inside constructor
             *
             * @param ipAddress ipAddress of the device
             * @param port Port number of the device
             */
            void init(const char* ipAddress, int port = 502);
            /**
             * @brief Thread for cyclic reading values.<br>
             * Cycle time: #read_values_period
             *
             */
            std::unique_ptr<std::thread> read_values_thread;
            /**
             * @brief Running state of #read_values_thread
             *
             */
            bool read_values_run{false};
            /**
             * @brief Update connection statistics
             *
             * @param connection_state Connection state to be added to statistics
             */
            void update_connections_stats(char connection_state);
            /**
             * @brief Task of #read_values_thread
             *
             */
            void read_values_task();

        protected:
            /**
             * @brief Counter of transmissions
             *
             */
            long test_counter{0};
            /**
             * @brief Counter of successful transmissions
             *
             */
            long test_counter_success{0};
            /**
             * @brief Counter of failed transmissions
             *
             */
            long test_counter_fail{0};
            /**
             * @brief Connection statistics
             *
             */
            std::string test_history{""};
    };

    /**
     * @brief Modbus test function
     *
     */
    void test_modbus();
}
