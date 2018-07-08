package com.example.maverick.mavremote;

public abstract class System
{
    public System()
    {
    }

    public void Run()
    {
        Start();

        while (_bIsRunning)
        {
            MainLoop();
        }

        Finish();
    }

    protected abstract void Start();

    protected abstract void Finish();

    protected abstract void MainLoop();

    protected void Stop() { _bIsRunning = false; }


    protected boolean _bIsRunning = true;
}
