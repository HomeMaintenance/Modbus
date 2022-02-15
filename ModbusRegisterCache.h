#pragma once
#include <ctime>
#include <vector>

namespace mb{
class RegisterCache{
public:
    RegisterCache();
    ~RegisterCache() = default;
    void update(std::vector<uint16_t>& data, int& last_read_status);
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
