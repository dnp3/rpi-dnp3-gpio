
#include <iostream>
#include <ini.h>
#include <stdexcept>
#include <wiringPi.h>

#include <asiodnp3/DNP3Manager.h>
#include <asiodnp3/MeasUpdate.h>
#include <asiopal/UTCTimeSource.h>
#include <asiodnp3/ConsoleLogger.h>
#include <opendnp3/LogLevels.h>

#include <thread>

#include "Config.h"
#include "GPIOCommandHandler.h"

// prototypes for handling ini file reading
int cfg_handler(void* user, const char* section, const char* name, const char* value);
bool safe_handler(Config& config, const std::string& section, const std::string& name, const std::string& value);

using namespace opendnp3;
using namespace asiodnp3;

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		std::cerr << "usage: rpi-dnp3-gpio <ini file path>" << std::endl;
		return -1;
	}

	auto filename = argv[1];

	Config config;
	if(ini_parse(filename, cfg_handler, &config) != 0)
	{
		std::cerr << "error parsing ini file: " << filename << std::endl;
		return -1;
	}

	// Initialize wiringPi -- using Broadcom pin numbers
	// this call automatically terminates program with an error msg if the setup cannot be completed
	wiringPiSetupGpio();

	// setup inputs and outputs
	for(auto pin : config.inputs) {
		pinMode(pin, INPUT);
		std::cout << "pin " << pin << " set as INPUT" << std::endl;
	}

	for(auto pin : config.outputs) {
		pinMode(pin, OUTPUT);
		std::cout << "pin " << pin << " set as OUTPUT" << std::endl;
	}

	GPIOCommandHandler commandHandler(config.outputs);

	const auto LOG_LEVELS = levels::NORMAL | levels::ALL_APP_COMMS;

	DNP3Manager manager(1);

	manager.AddLogSubscriber(ConsoleLogger::Instance());

	auto channel = manager.AddTCPServer("server", LOG_LEVELS, ChannelRetry::Default(), "0.0.0.0", 20000);

	OutstationStackConfig oconfig;
	oconfig.dbTemplate = DatabaseTemplate::BinaryOnly(config.inputs.size());

	//configure addressing
	oconfig.link.RemoteAddr = 1;
	oconfig.link.LocalAddr = 10;

	auto outstation = channel->AddOutstation("outstation", commandHandler, DefaultOutstationApplication::Instance(), oconfig);

	outstation->Enable();

	while(true) {

		uint16_t index = 0;
		DNPTime time(asiopal::UTCTimeSource::Instance().Now().msSinceEpoch);

		MeasUpdate update(outstation);

		for(auto pin : config.inputs) {
			bool value = digitalRead(pin);
			update.Update(Binary(value, 0x01, time), index);
			++index;
		}

		// determines the sampling rate
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

bool safe_handler(Config& config, const std::string& section, const std::string& name, const std::string& value)
{
	try
	{
		auto gpiopin = std::stoi(name);

		if(section == "input")
		{
			return config.AddInput(gpiopin);
		}
		else if(section == "output")
		{
			return config.AddOutput(gpiopin);
		}

		std::cerr << "Unknown section name: " << name << std::endl;
		return false;
	}
	catch(std::invalid_argument)
	{
		std::cerr << "Bad integer conversion on: " << name << std::endl;
		return false;
	}
}

int cfg_handler(void* user, const char* section, const char* name, const char* value)
{
	return safe_handler(*(Config*)user, section, name, value) ? 1 : 0;
}
