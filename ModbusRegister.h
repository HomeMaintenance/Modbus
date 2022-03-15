#pragma once
#include <modbus.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <cassert>
#include <ctime>
#include "ModbusDevice.h"
#include "ModbusRegisterCache.h"
#ifdef MODBUS_DEBUG
#include <iostream>
#endif

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
                data_cache = new RegisterCache();
                auto data = readRawData(true);
            }
            Register(const Register& other) = delete;
            virtual ~Register(){
                delete data_cache;
                data_cache = nullptr;
            };
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

        private:
            /**
             * @brief Data vector for raw data of the register
             *
             */

            /**
             * @brief Length of the register in numbers of words(16bit)
             *
             */
            unsigned short dataSize{0};
            /**
             * @brief #mb::Device instance this register belongs to
             *
             */
            Device* device = nullptr;

            RegisterCache* data_cache = nullptr;

        public:
            /**
             * @brief Read raw data from the register
             *
             * @param ret Return status (true: success, false: fail)
             * @return std::vector<uint16_t> Data vector with raw data from the register
             */
            std::vector<uint16_t> readRawData(bool force = false, bool* ret = nullptr) const
            {
                #ifdef MODBUS_DEBUG
                std::cout << "Reading  " << device->ipAddress << " register " << addr << ", " << dataSize << std::endl;
                #endif
                if(!force && !data_cache->dirty()){
                    #ifdef MODBUS_DEBUG
                    std::cout << "Use cache" << std::endl;
                    #endif
                    if(ret)
                        *ret = true;
                    return data_cache->get_data();
                }
                #ifdef MODBUS_DEBUG
                if(force)
                    std::cout << "Forced update, ";
                std::cout << "Update cache" << std::endl;
                #endif
                assert(device != nullptr && "Device must not be nullptr");
                std::vector<uint16_t> data(dataSize,0);
                device->modbus_mtx.lock();
                int status = modbus_read_registers(device->connection, addr, dataSize, data.data());
                device->modbus_mtx.unlock();
                data_cache->update(data, status);
                if (ret) {
                    *ret = status == dataSize;
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
                std::vector<uint16_t> rawData = readRawData(force, ret);
                if(rawData.size() != dataSize){
                    std::string assert_message = "\tInvalid data size read from device "+device->ipAddress+", expected " + std::to_string(dataSize) + " got "+std::to_string(rawData.size())+".";
                    std::cout<<assert_message<<std::endl;
                    device->reportError();
                    if(ret)
                        *ret = false;
                    return static_cast<T>(0);
                }
                #ifdef MODBUS_DEBUG
                std::cout << "\t success" << std::endl;
                #endif
                switch(rawData.size()) {
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
                        temp16 = rawData[0];
                        tempT = static_cast<T>(temp16*factor);
                        break;
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
                device->modbus_mtx.lock();
                int status = modbus_write_registers(device->connection, addr, dataSize, input.data());
                device->modbus_mtx.unlock();
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
            bool setValue(short input, bool* ret = nullptr)
            {
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
