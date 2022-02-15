#include "ModbusRegisterCache.h"

namespace mb {
RegisterCache::RegisterCache(){
    time = std::clock();
}

void RegisterCache::update(std::vector<uint16_t>& _data, int& last_read_status){
    time = std::clock();
    data = _data;
    _register_read_status = last_read_status;
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

    std::clock_t time_now{std::clock()};
    bool result = (time + max_age) < time_now;
    return result;
}

}
