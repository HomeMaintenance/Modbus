#pragma once
#include <chrono>
#include <vector>
#include <type_traits>
#include <stdint.h>

namespace mb{
class RegisterCache{
public:
    RegisterCache();
    virtual ~RegisterCache() = default;
    template<typename T>
    void update(std::vector<uint16_t>& _data, T last_read_status){
        static_assert(std::is_integral<T>::value, "Integral required.");
        time = std::chrono::steady_clock::now().time_since_epoch();
        data = _data;
        _register_read_status = static_cast<int>(last_read_status);
    }
    std::vector<uint16_t> get_data() const;
    int register_read_status();
    bool dirty() const;
    std::chrono::duration<float, std::milli> max_age{3000};
private:
    std::chrono::duration<float, std::milli> time;
    std::vector<uint16_t> data;
    unsigned int _register_read_status;
};
}
