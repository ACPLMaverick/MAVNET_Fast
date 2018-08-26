#include "Input.h"

#include "App.h"

Input::Input()
{
}


Input::~Input()
{
}

bool Input::IsKeyDown(Key key)
{
	return glfwGetKey(App::GetInstance().GetWindowPtr(), key) == GLFW_PRESS;
}

void Input::Init()
{
}

void Input::Shutdown()
{
}

void Input::Update()
{

}
