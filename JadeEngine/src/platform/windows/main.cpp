#include <iostream>

int main()
{
#if JE_CONFIG_DEBUG
	std::cout << "Hello world! Debug.";
#elif JE_CONFIG_PROFILE
	std::cout << "Hello world! Profile.";
#elif JE_CONFIG_RELEASE
	std::cout << "Hello world! Release.";
#else
#error "Configuration not specified."
#endif
	return 0;
}