package com.example.maverick.mavremote;

import java.util.concurrent.locks.ReentrantLock;

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
            _systemLock.lock();
            MainLoop();
            _systemLock.unlock();
        }

        Finish();
    }

    public void Stop()
    {
        _systemLock.lock();
        _bIsRunning = false;
        _systemLock.unlock();
    }

    protected abstract void Start();

    protected abstract void Finish();

    protected abstract void MainLoop();


    protected ReentrantLock _systemLock = new ReentrantLock();
    protected boolean _bIsRunning = true;
}
