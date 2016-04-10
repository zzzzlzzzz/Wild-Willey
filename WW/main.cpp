#include <stdexcept>
#include <iostream>

#include "config.h"
#include "core.h"

int main(int argc, char* argv[])
{
	const char* configFileName = "config.xml";
	try
	{
		GameSpace::Config config(configFileName);
		GameSpace::GameCore core(&config);
		core.runGameLoop();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unhandled exception" << std::endl;
	}
	return 0;
}