#ifndef RPI_DNP3_GPIO_CONFIG_H
#define RPI_DNP3_GPIO_CONFIG_H

#include <stdint.h>
#include <vector>
#include <set>

class Config
{
    typedef std::vector<uint8_t> assignvec;

public:

    bool AddInput(uint8_t gpiopin);
    bool AddOutput(uint8_t gpiopin);


    std::vector<uint8_t> inputs;
    std::vector<uint8_t> outputs;

private:

    bool Add(uint8_t gpiopin, assignvec& vec);

    std::set<uint8_t> assigned;
};

#endif //RPI_DNP3_GPIO_CONFIG_H
