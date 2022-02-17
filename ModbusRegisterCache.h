#pragma once
#include <ctime>
#include <vector>
#include <type_traits>

namespace mb{
class RegisterCache{
public:
    RegisterCache();
    ~RegisterCache() = default;
    template<typename T>
    void update(std::vector<uint16_t>& _data, T last_read_status){
        static_assert(std::is_integral<T>::value, "Integral required.");
        time = std::clock();
        data = _data;
        _register_read_status = static_cast<int>(last_read_status);
    }
    std::vector<uint16_t> get_data() const;
    int register_read_status();
    bool dirty() const;
    int max_age{3000}; //milliseconds
private:
    std::clock_t time;
    std::vector<uint16_t> data;
    int _register_read_status;
};
}
