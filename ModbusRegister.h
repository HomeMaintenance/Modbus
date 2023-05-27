#pragma once
#include <modbus.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <cassert>
#include "ModbusDevice.h"
#include "ModbusRegisterCache.h"
#include <iostream>

namespace mb{

    std::string printVector(std::vector<uint16_t> input);

    /**
     * @brief Modbus register for a #mb::Device
     *
     * @tparam T Type of value inside the register (short, unsigned int, int, long, float, double)
     */
    template<class T>
    class Register{
        public:
            /**
             * @brief Construct a new Register object
             *
             * @param device_ #mb::Device instance this register belongs to
             * @param addr_ Address of the register
             * @param factor_ Factor to multiply the value of the register
             * @param unit_ Unit of the register value
             */
            explicit Register(Device* device_, int addr_, float factor_ = 1., std::string unit_ = "") :
                addr(addr_),
                factor(factor_),
                unit(unit_),
                dataSize(sizeof(T)/2),
                device(device_)
            {
                data_cache = std::make_unique<RegisterCache>(dataSize);
            }
            Register(const Register& other) = delete;
            virtual ~Register(){
            }
            /**
             * @brief Address of the register
             *
             */
            int addr = 0;
            /**
             * @brief Factor to multiply the value of the register
             *
             */
            float factor = 0;
            /**
             * @brief Unit of the register value
             *
             */
            std::string unit = "";

            int cache_max_age{3000}; // milliseconds

            void enable_log(){
                _enable_log = true;
            }

            void disable_log(){
                _enable_log = false;
            }

            void log(std::string message) const {
                if(_enable_log)
                    std::cout << message << std::endl;
            }

        private:
            /**
             * @brief Data vector for raw data of the register
             *
             */

            /**
             * @brief Length of the register in numbers of words(16bit)
             *
             */
            const unsigned short dataSize;
            /**
             * @brief #mb::Device instance this register belongs to
             *
             */
            Device* device = nullptr;

            void setDeviceOnline(const bool& status) const {
                if(device != nullptr)
                    device->setOnline(status);
            }

            std::unique_ptr<RegisterCache> data_cache;

            bool _enable_log = false;

        public:
            /**
             * @brief Read raw data from the register
             *
             * @param ret Return status (true: success, false: fail)
             * @return std::vector<uint16_t> Data vector with raw data from the register
             */
            std::vector<uint16_t> readRawData(bool force = false, bool* ret = nullptr, int* status = nullptr) const
            {
                #ifdef MODBUS_DEBUG
                log("Reading  " + device->ipAddress + " register " + std::to_string(addr) + ", " + std::to_string(dataSize));
                #endif
                if(!force && !data_cache->dirty()){
                    #ifdef MODBUS_DEBUG
                    log("Use cache");
                    #endif
                    if(ret)
                        *ret = true;
                    return data_cache->get_data();
                }
                #ifdef MODBUS_DEBUG
                if(force)
                    log("Forced update");
                log("Update cache");
                #endif
                assert(device != nullptr && "Device must not be nullptr");
                std::vector<uint16_t> data(dataSize,0);
                std::lock_guard<std::mutex> lk(device->modbus_mtx);
                const int _status = modbus_read_registers(device->connection, addr, dataSize, data.data());
                data_cache->update(data, _status);
                if(status){
                    *status = _status;
                }
                if(ret) {
                    *ret = _status == dataSize;
                }
                return data_cache->get_data();
            }

            /**
             * @brief Get value from the register
             *
             * Uses #readRawData to get data and then converts it to T.
             *
             * @param ret Return status (true: success, false: fail)
             * @return T Value of the register
             */
            T getValue(bool force = false, bool* ret = nullptr) const
            {
                short temp16{0};
                int temp32{0};
                long temp64{0};
                T tempT{0};
                bool _ret = false;
                int status;
                const bool reconnectEnabled = device->reconnectEnabled();
                std::vector<uint16_t> rawData(dataSize);
                do{
                    rawData = readRawData(force, &_ret, &status);
                    setDeviceOnline(_ret);
                    if(!_ret && reconnectEnabled)
                        device->reconnect();
                }while(!_ret && reconnectEnabled);

                if(ret)
                    *ret = _ret;
                if(!_ret){
                    std::string assert_message = "\tInvalid data size read from device " + device->ipAddress + ", expected " + std::to_string(dataSize) + " got " + std::to_string(status) + ".\n\t";
                    assert_message += std::string("Error: \"" + std::string(modbus_strerror(errno)) + "\"\n");
                    std::cout<<assert_message<<std::endl;
                    return static_cast<T>(0);
                }
                #ifdef MODBUS_DEBUG
                log("\t success");
                #endif
                switch(rawData.size()) {
                    case 1:
                        temp16 = rawData[0];
                        tempT = static_cast<T>(temp16*factor);
                        break;
                    case 2:
                        temp32 = MODBUS_GET_INT32_FROM_INT16(rawData.data(), 0);
                        tempT = static_cast<T>(temp32*factor);
                        break;
                    case 4:
                        temp64 = MODBUS_GET_INT64_FROM_INT16(rawData.data(), 0);
                        tempT = static_cast<T>(temp64*factor);
                        return tempT;
                        break;
                    default:
                        tempT = std::numeric_limits<T>::quiet_NaN();
                    break;
                }
                return tempT;
            }

            /**
             * @brief Write raw data to the register
             *
             * @param input Data vector to be written to the register
             * @param ret Return status (true: success, false: fail)
             */
            bool writeRawData(const std::vector<uint16_t>& input, bool* ret = nullptr)
            {
                assert(device != nullptr);
                std::lock_guard<std::mutex> lk(device->modbus_mtx);
                int status = -1;
                if(input.size() == 1){
                    status = modbus_write_register(device->connection, addr, input[0]);
                }
                if(status < 0){ // try again if write_register fails
                    status = modbus_write_registers(device->connection, addr, dataSize, input.data());
                }
                bool result = status == dataSize;
                if (ret) {
                    *ret = result;
                }
                return result;
            }

        public:
            /**
             * @brief Set the Value of the register
             *
             * @param input Data to be written to the register
             * @param ret Return status (true: success, false: fail)
             */
            bool setValue(unsigned short input, bool* ret = nullptr)
            {
                input /= factor;
                std::vector<uint16_t> buffer{input};
                bool status = writeRawData(buffer,&status);
                if(ret)
                    *ret = status;
                if(status)
                    data_cache->update(buffer, dataSize);
                else
                    data_cache->update(buffer, -1);
                return status;
            };

            /**
             * @brief Set the Value of the register
             *
             * @param input Data to be written to the register
             * @param ret Return status (true: success, false: fail)
             */
            bool setValue(short input, bool* ret = nullptr)
            {
                input /= factor;
                std::vector<uint16_t> buffer(dataSize);
                buffer[0] = input & 0xFFFFFFFF;
                bool status = writeRawData(buffer,&status);
                if(ret)
                    *ret = status;
                if(status)
                    data_cache->update(buffer, dataSize);
                else
                    data_cache->update(buffer, -1);
                return status;
            };

            /**
             * @brief Set the Value of the register
             *
             * @param input Data to be written to the register
             * @param ret Return status (true: success, false: fail)
             */
            bool setValue(unsigned int input, bool* ret = nullptr)
            {
                input /= factor;
                std::vector<uint16_t> buffer(dataSize);
                MODBUS_SET_INT32_TO_INT16(buffer.data(), 0, input);
                bool status = writeRawData(buffer,ret);
                if(ret)
                    *ret = status;
                if(status)
                    data_cache->update(buffer, dataSize);
                else
                    data_cache->update(buffer, -1);
                return status;
            }

            /**
             * @brief Set the Value of the register
             *
             * @param input Data to be written to the register
             * @param ret Return status (true: success, false: fail)
             */
            bool setValue(int input, bool* ret = nullptr)
            {
                input /= factor;
                std::vector<uint16_t> buffer(dataSize);
                MODBUS_SET_INT32_TO_INT16(buffer.data(), 0, input);
                bool status = writeRawData(buffer);
                if(ret)
                    *ret = status;
                if(status)
                    data_cache->update(buffer, dataSize);
                else
                    data_cache->update(buffer, -1);
                return status;
            }

            /**
             * @brief Set the Value of the register
             *
             * @param input Data to be written to the register
             * @param ret Return status (true: success, false: fail)
             */
            bool setValue(long input, bool* ret = nullptr)
            {
                input /= factor;
                std::vector<uint16_t> buffer(dataSize);
                MODBUS_SET_INT64_TO_INT16(buffer.data(), 0, input);
                bool status = writeRawData(buffer);
                if(ret)
                    *ret = status;
                if(status)
                    data_cache->update(buffer, dataSize);
                else
                    data_cache->update(buffer, -1);
                return status;
            }

            /**
             * @brief Set the Value of the register
             *
             * @param input Data to be written to the register
             * @param ret Return status (true: success, false: fail)
             */
            bool setValue(float input, bool* ret = nullptr)
            {
                int temp = input/factor;
                std::vector<uint16_t> buffer(dataSize);
                MODBUS_SET_INT32_TO_INT16(buffer.data(), 0, temp);
                bool status = writeRawData(buffer);
                if(ret)
                    *ret = status;
                if(status)
                    data_cache->update(buffer, dataSize);
                else
                    data_cache->update(buffer, -1);
                return status;
            }

            /**
             * @brief Set the Value of the register
             *
             * @param input Data to be written to the register
             * @param ret Return status (true: success, false: fail)
             */
            bool setValue(double input, bool* ret = nullptr)
            {
                long temp = input/factor;
                std::vector<uint16_t> buffer(dataSize);
                MODBUS_SET_INT64_TO_INT16(buffer.data(), 0, temp);
                bool status = writeRawData(buffer);
                if(ret)
                    *ret = status;
                if(status)
                    data_cache->update(buffer, dataSize);
                else
                    data_cache->update(buffer, -1);
                return status;
            }
    };
}
