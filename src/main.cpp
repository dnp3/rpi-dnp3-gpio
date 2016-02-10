
#include <iostream>
#include <ini.h>
#include <stdexcept>
#include <wiringPi.h>

#include <asiodnp3/DNP3Manager.h>

#include <thread>

#include "Config.h"

// prototypes for handling ini file reading
int cfg_handler(void* user, const char* section, const char* name, const char* value);
bool safe_handler(Config& config, const std::string& section, const std::string& name, const std::string& value);

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
	// wiringPiSetupGpio();

	// setup inputs and outputs
	for(auto pin : config.inputs) {
		pinMode(pin, INPUT);
	}

	for(auto pin : config.outputs) {
		pinMode(pin, OUTPUT);
	}


	DNP3Manager manager(1);


	while(true) {
		std::cout << "enter x to exit" << std::endl;
		char input;
		std::cin >> input;
		switch(input) {
			case('x'):
				return 0;
			default:
				continue;
		}
	}

	return 0;
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