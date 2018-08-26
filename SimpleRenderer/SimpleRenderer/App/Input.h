#pragma once

class App;

class Input
{
public:

	typedef int Key;

	void Init();
	void Shutdown();
	void Update();

	bool IsKeyDown(Key key);

private:

	Input();
	~Input();

	friend class App;
};

