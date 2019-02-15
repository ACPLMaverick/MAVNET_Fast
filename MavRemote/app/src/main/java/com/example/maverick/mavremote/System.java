package com.example.maverick.mavremote;

import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.locks.ReentrantLock;

public abstract class System
{
    public System()
    {
    }

    public void Run()
    {
        Start();

        _bIsRunning.set(true);

        while (_bIsRunning.get())
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
        _bIsRunning.set(false);
    }

    public boolean IsRunning()
    {
        return _bIsRunning.get();
    }

    protected abstract void Start();

    protected abstract void Finish();

    protected abstract void MainLoop();

    protected long GetTickSleepMs() { return 1; }


    protected ReentrantLock _systemMainLoopLock = new ReentrantLock();
    protected AtomicBoolean _bIsRunning = new AtomicBoolean(false);
}
