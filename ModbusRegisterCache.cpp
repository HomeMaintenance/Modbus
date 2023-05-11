#include "ModbusRegisterCache.h"

namespace mb {
RegisterCache::RegisterCache(unsigned int _size): size(_size)
{
    time = std::chrono::steady_clock::now().time_since_epoch();
}

void RegisterCache::update(const std::vector<uint16_t>& _data, int last_read_status){
    time = std::chrono::steady_clock::now().time_since_epoch();
    _dirty = last_read_status != size;
    if(!_dirty)
        data = _data;
    _register_read_status = last_read_status;
}

std::vector<uint16_t> RegisterCache::get_data() const{
    if(retain_last_valid || !_dirty)
        return data;
    return std::vector<uint16_t>(0);
}

int RegisterCache::register_read_status(){
    return _register_read_status;
}

bool RegisterCache::dirty() const{
    if(_dirty)
        return true;

    std::chrono::duration<float, std::milli> time_now = std::chrono::steady_clock::now().time_since_epoch();
    bool result = (time + max_age) < time_now;
    return result;
}

}
