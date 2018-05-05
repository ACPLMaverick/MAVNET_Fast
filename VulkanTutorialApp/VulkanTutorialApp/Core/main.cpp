#include "HelloTriangle.h"

#include "GlobalIncludes.h"

#if !NDEBUG
#include <conio.h>
#endif

int main() 
{
	Core::HelloTriangle app;

	try
	{
		app.Run();
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

#if !NDEBUG
	std::cout << "\nPress any key to exit...\n";
	_getch();
#endif

    return EXIT_SUCCESS;
}