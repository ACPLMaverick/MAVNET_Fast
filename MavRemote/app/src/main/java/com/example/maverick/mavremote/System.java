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

        _systemLock.lock();
        _bIsRunning = true;
        _systemLock.unlock();

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

    public boolean IsRunning()
    {
        _systemLock.lock();
        boolean bRunning = _bIsRunning;
        _systemLock.unlock();
        return bRunning;
    }

    protected abstract void Start();

    protected abstract void Finish();

    protected abstract void MainLoop();


    protected ReentrantLock _systemLock = new ReentrantLock();
    protected boolean _bIsRunning = false;
}
