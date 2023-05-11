#pragma once
#include <chrono>
#include <vector>
#include <type_traits>
#include <stdint.h>

namespace mb{
class RegisterCache{
public:
    RegisterCache(unsigned int size);
    virtual ~RegisterCache() = default;
    void update(const std::vector<uint16_t>& _data, int last_read_status);
    std::vector<uint16_t> get_data() const;
    int register_read_status();
    bool dirty() const;
    std::chrono::duration<float, std::milli> max_age{3000};
    bool retain_last_valid = false;
private:
    std::chrono::duration<float, std::milli> time;
    std::vector<uint16_t> data;
    int _register_read_status;
    const int size;
    bool _dirty;
};
}
