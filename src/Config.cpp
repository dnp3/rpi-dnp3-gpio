
#include "Config.h"

bool Config::AddInput(uint8_t gpiopin)
{
    return Add(gpiopin, inputs);
}

bool Config::AddOutput(uint8_t gpiopin)
{
    return Add(gpiopin, outputs);
}

bool Config::Add(uint8_t gpiopin, assignvec& vec)
{
    // first check if the pin is assigned
    if(assigned.find(gpiopin) != assigned.end())
    {
        return false;
    }

    vec.push_back(gpiopin);
    assigned.insert(gpiopin);

    return true;
}



