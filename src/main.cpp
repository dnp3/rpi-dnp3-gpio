
#include <iostream>
#include <ini.h>
#include <stdexcept>
#include <wiringPi.h>

#include <asiodnp3/DNP3Manager.h>
#include <asiodnp3/UpdateBuilder.h>
#include <asiodnp3/PrintingChannelListener.h>
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
using namespace asiopal;
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
		std::cout << "pin " << static_cast<int>(pin) << " set as INPUT" << std::endl;
	}

	for(auto pin : config.outputs) {
		pinMode(pin, OUTPUT);
		std::cout << "pin " << static_cast<int>(pin) << " set as OUTPUT" << std::endl;
	}

	const auto commandHandler = std::make_shared<GPIOCommandHandler>(config.outputs);

	const auto LOG_LEVELS = levels::NORMAL | levels::ALL_APP_COMMS;

	DNP3Manager manager(1, ConsoleLogger::Create());

	auto channel = manager.AddTCPServer("server", LOG_LEVELS, ChannelRetry::Default(), "0.0.0.0", config.port, PrintingChannelListener::Create());

	OutstationStackConfig stack(DatabaseSizes::BinaryOnly(config.inputs.size()));
	stack.link = config.link;

	stack.outstation.eventBufferConfig = EventBufferConfig(50);
	stack.outstation.params.allowUnsolicited = true;

	auto outstation = channel->AddOutstation("outstation", commandHandler, DefaultOutstationApplication::Create(), stack);

	outstation->Enable();

	std::cout << "Sample period is: " << config.sample_period_ms << std::endl;
        const auto SAMPLE_PERIOD = std::chrono::milliseconds(config.sample_period_ms);



	while(true) {

		uint16_t index = 0;
		DNPTime time(asiopal::UTCTimeSource::Instance().Now().msSinceEpoch);

		UpdateBuilder builder;
		for(auto pin : config.inputs) {
			bool value = digitalRead(pin);
			builder.Update(Binary(value, 0x01, time), index);
			++index;
		}
		outstation->Apply(builder.Build());

		

		// determines the sampling rate
		std::this_thread::sleep_for(SAMPLE_PERIOD);
	}
}

bool safe_handler(Config& config, const std::string& section, const std::string& name, const std::string& value)
{
	try
	{		
		if(section == "input")
		{
			return config.AddInput(std::stoi(name));
		}
		else if(section == "output")
		{
			return config.AddOutput(std::stoi(name));
		}
		else if(section == "program")
		{
			if(name == "sample-period-ms")
			{
				config.sample_period_ms = std::stoi(value);
				return true;
			}
		}
		else if(section == "link")
		{
			if(name == "remote-addr")
			{
				config.link.RemoteAddr = std::stoi(value);
				return true;
			}
			else if(name == "local-addr")
			{
				config.link.LocalAddr = std::stoi(value);
				return true;
			}
			else if (name == "port")
			{
				config.port = std::stoi(value);
				return true;
			}
		}

		std::cerr << "Unknown parameter, section: " << section << " name: " << name << " value: " << value << std::endl;
		return false;
	}
	catch(std::invalid_argument)
	{
		std::cerr << "Bad integer conversion, section: " << section << " name: " << name << " value: " << value << std::endl;
		return false;
	}
}

int cfg_handler(void* user, const char* section, const char* name, const char* value)
{
	return safe_handler(*(Config*)user, section, name, value) ? 1 : 0;
}
