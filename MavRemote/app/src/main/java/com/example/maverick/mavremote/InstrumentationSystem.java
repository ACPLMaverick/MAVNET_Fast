package com.example.maverick.mavremote;

import android.util.Log;

import java.util.LinkedList;
import java.util.Queue;
import java.util.concurrent.Semaphore;
import java.util.concurrent.locks.ReentrantLock;

public final class InstrumentationSystem extends System
{
    public void EnqueueActionEvent(ActionEvent ev)
    {
        assert (_lock != null);

        _lock.lock();
        _queue.push(ev);
        _lock.unlock();
    }


    @Override
    protected void Start()
    {
        _queue = new LinkedList<>();
        _lock = new ReentrantLock();

        InitRootShell();
    }

    @Override
    protected void Finish()
    {

    }

    @Override
    protected void MainLoop()
    {
        while(true)
        {
            if(_queue.isEmpty())
                continue;

            _lock.lock();
            ActionEvent ev = _queue.pop();
            _lock.unlock();
            PerformActionEvent(ev);

            Thread.yield();
        }
    }


    private void PerformActionEvent(ActionEvent ev)
    {
        Log.d(App.TAG, ev.toString());
    }

    private void InitRootShell()
    {
        // TODO
    }


    private LinkedList<ActionEvent> _queue = null;
    private ReentrantLock _lock = null;
}
