#include "HelloTriangle.h"

#if !NDEBUG
#if PLATFORM_LINUX
#elif PLATFORM_WINDOWS
#define CHECK_KEY_AT_END 1
#endif
#endif

#if PLATFORM_WINDOWS
#include <conio.h>
#endif

int main() 
{
#if RENDERDOC
	std::cout << "\nPress any key to start after you hook up Render Doc.\n";
	_getch();
	std::cout << std::endl;
#endif

	Core::HelloTriangle app;

	try
	{
		app.Run();
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << e.what() << std::endl;
		#if CHECK_KEY_AT_END
		_getch();
		#endif
		return EXIT_FAILURE;
	}

#if CHECK_KEY_AT_END
	std::cout << "\nPress any key to exit...\n";
	_getch();
#endif

    return EXIT_SUCCESS;
}