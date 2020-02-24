#pragma once
#include "BaseProcessor.h"
class GamepadProcessor :
	public BaseProcessor
{
public:

	GamepadProcessor();
	~GamepadProcessor();

	GamepadProcessor(const GamepadProcessor&) = delete;
	GamepadProcessor& operator=(const GamepadProcessor&) = delete;

	void GetDeviceLabels(std::vector<std::string>& outLabels);

protected:

	// Inherited via BaseProcessor
	virtual void Init_Internal() override;
	virtual void Cleanup_Internal() override;
};

