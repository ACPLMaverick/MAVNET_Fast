#include "HelloTriangle.h"

#if !NDEBUG
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
		_getch();
		return EXIT_FAILURE;
	}

#if !NDEBUG
	std::cout << "\nPress any key to exit...\n";
	_getch();
#endif

    return EXIT_SUCCESS;
}