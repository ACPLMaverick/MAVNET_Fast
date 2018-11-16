#include "App\App.h"

#include <conio.h>

int main()
{
	App::GetInstance().Run();

	/*
#if _DEBUG
	_getch();
#endif
	*/
	return 0;
}