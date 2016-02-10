#include "GPIOCommandHandler.h"
#include <wiringPi.h>

using namespace opendnp3;

GPIOCommandHandler::GPIOCommandHandler(const std::vector<uint8_t> gpiopins)
{
    uint16_t index = 0;

    for(auto pin : gpiopins) {
       dnp2gpio[index] = pin;
       ++index;
    }
}

CommandStatus GPIOCommandHandler::Select(const ControlRelayOutputBlock& command, uint16_t index)
{
    uint8_t gpio = 0;
    bool state = false;

    return GetPinAndState(index, command.functionCode, gpio, state);
}

CommandStatus GPIOCommandHandler::Operate(const ControlRelayOutputBlock& command, uint16_t index, OperateType opType)
{
    uint8_t gpio = 0;
    bool state = false;

    auto ret = GetPinAndState(index, command.functionCode, gpio, state);

    if(ret == CommandStatus::SUCCESS)
    {
        digitalWrite(gpio, state);
    }

    return ret;
}

CommandStatus GPIOCommandHandler::GetPinAndState(uint16_t index, opendnp3::ControlCode code, uint8_t& gpio, bool& state)
{
    switch(code)
    {
        case(ControlCode::LATCH_ON):
            state = true;
            break;
        case(ControlCode::LATCH_OFF):
            state = false;
            break;
        default:
            return CommandStatus::NOT_SUPPORTED;
    }

    auto iter = dnp2gpio.find(index);
    if(iter == dnp2gpio.end()) {
        return CommandStatus::NOT_SUPPORTED;
    }

    gpio = iter->second;
    return CommandStatus::SUCCESS;
}
