package com.example.maverick.mavremote.Server.Instrumentation;

class InputDeviceEvent
{
    public InputDeviceEvent()
    {
        _evType = 0;
        _evCode = 0;
        _evValue = 0;
        _delay = 0.0f;
    }

    public InputDeviceEvent(int evType, int evCode, int evValue)
    {
        _evType = evType;
        _evCode = evCode;
        _evValue = evValue;
        _delay = 0.0f;
    }

	public InputDeviceEvent(int evType, int evCode, int evValue, float delay)
	{
		this(evType, evCode, evValue);
		_delay = delay;
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

	public float GetDelay() { return _delay; }


	private int _evType;
    private int _evCode;
    private int _evValue;
    private float _delay;
}
