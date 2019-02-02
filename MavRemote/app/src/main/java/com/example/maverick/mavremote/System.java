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
            _systemMainLoopLock.lock();
            MainLoop();
            _systemMainLoopLock.unlock();
            Utility.SleepThread(GetTickSleepMs());
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

    protected long GetTickSleepMs() { return 0; }


    protected ReentrantLock _systemLock = new ReentrantLock();
    protected ReentrantLock _systemMainLoopLock = new ReentrantLock();
    protected boolean _bIsRunning = false;
}
