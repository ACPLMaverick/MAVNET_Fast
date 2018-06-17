package com.example.maverick.mavremote;

public abstract class System
{
    public System()
    {
    }

    public void Run()
    {
        Start();
        MainLoop();
        Finish();
    }

    protected abstract void Start();

    protected abstract void Finish();

    protected abstract void MainLoop();

}
