package com.example.maverick.mavremote;

import java.util.ArrayDeque;
import java.util.concurrent.locks.ReentrantLock;

public class EventQueue<T>
{
    public EventQueue()
    {
    }

    public void Init()
    {
        _queue = new ArrayDeque<>();
        _lock = new ReentrantLock();
    }

    public void Enqueue(T obj)
    {
        Utility.Assert(_lock != null && _queue != null);

        _lock.lock();
        _queue.push(obj);
        _lock.unlock();
    }

    public T Dequeue()
    {
        Utility.Assert(_lock != null && _queue != null);

        _lock.lock();
        T ev = _queue.pollLast();
        _lock.unlock();
        return ev;
    }

    public boolean IsEmpty()
    {
        Utility.Assert(_lock != null && _queue != null);
        return _queue.isEmpty();
    }

    public int GetSize()
    {
        Utility.Assert(_queue != null);
        return _queue.size();
    }

    public void Clear()
    {
        _lock.lock();
        _queue.clear();
        _lock.unlock();
    }

    private ArrayDeque<T> _queue = null;
    private ReentrantLock _lock = null;
}
