
#include <iostream>
#include <ini.h>

#include "Config.h"

bool safe_handler(Config& config, const std::string& section, const std::string& name, const std::string& value)
{
	std::cout << "section: " << section << " name: " << name << " value: " << value << std::endl;
	return true;
}

int cfg_handler(void* user, const char* section, const char* name, const char* value)
{
	return safe_handler(*(Config*)user, section, name, value) ? 1 : 0;
}

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

	return 0;
}

