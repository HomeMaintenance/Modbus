#include "ModbusRegisterCache.h"

namespace mb {
RegisterCache::RegisterCache(unsigned int _size): size(_size)
{
    time = std::chrono::steady_clock::now().time_since_epoch();
}

void RegisterCache::update(std::vector<uint16_t>& _data, int last_read_status){
    time = std::chrono::steady_clock::now().time_since_epoch();
    if(!retain_last_valid || last_read_status != -1)
        data = _data;
    _register_read_status = last_read_status;
}

std::vector<uint16_t> RegisterCache::get_data() const{
    if(retain_last_valid || _register_read_status != -1)
        return data;
    return std::vector<uint16_t>(size, 0);
}

int RegisterCache::register_read_status(){
    return _register_read_status;
}

bool RegisterCache::dirty() const{
    if(_register_read_status == -1)
        return true;

    std::chrono::duration<float, std::milli> time_now = std::chrono::steady_clock::now().time_since_epoch();
    bool result = (time + max_age) < time_now;
    return result;
}

}
