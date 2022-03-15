#include "ModbusRegisterCache.h"

namespace mb {
RegisterCache::RegisterCache(){
    time = std::chrono::steady_clock::now().time_since_epoch();
}

std::vector<uint16_t> RegisterCache::get_data() const{
    if(_register_read_status == data.size())
        return data;
    return std::vector<uint16_t>(0);
}

int RegisterCache::register_read_status(){
    return _register_read_status;
}

bool RegisterCache::dirty() const{
    if(_register_read_status != data.size())
        return true;

    std::chrono::duration<float, std::milli> time_now = std::chrono::steady_clock::now().time_since_epoch();
    bool result = (time + max_age) < time_now;
    return result;
}

}
