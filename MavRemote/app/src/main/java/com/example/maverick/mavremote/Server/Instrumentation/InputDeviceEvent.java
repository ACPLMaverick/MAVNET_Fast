package com.example.maverick.mavremote.Server.Instrumentation;

class InputDeviceEvent
{
    public InputDeviceEvent()
    {
        _evType = 0;
        _evCode = 0;
        _evValue = 0;
    }

    public InputDeviceEvent(int evType, int evCode, int evValue)
    {
        _evType = evType;
        _evCode = evCode;
        _evValue = evValue;
    }

	public int GetEvType()
	{
		return _evType;
	}

	public int GetEvCode()
	{
		return _evCode;
	}

	public int GetEvValue()
	{
		return _evValue;
	}


	private int _evType;
    private int _evCode;
    private int _evValue;
}
