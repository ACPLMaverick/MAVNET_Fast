#include <iostream>

int main()
{
#if JE_CONFIG_DEBUG
	std::cout << "Hello world! Debug." << std::endl;
#elif JE_CONFIG_PROFILE
	std::cout << "Hello world! Profile." << std::endl;
#elif JE_CONFIG_RELEASE
	std::cout << "Hello world! Release." << std::endl;
#else
#error "Configuration not specified."
#endif
	return 0;
}